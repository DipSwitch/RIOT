/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     sys_net_tcp
 *
 * @file
 * @brief       Basic TCP implementation
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "random.h"
#include "vtimer.h"
#include "net/ng_tcp.h"
#include "net/ng_nettype.h"
#include "net/ng_netapi.h"
#include "net/ng_netreg.h"
#include "net/ng_pktbuf.h"
#include "net/ng_pkt.h"
#include "net/ng_inet_csum.h"

#ifdef MODULE_NG_IPV6
#include "net/ng_ipv6.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Save the TCP's threads PID for later reference
 */
static kernel_pid_t _tcp_pid = KERNEL_PID_UNDEF;

/**
 * @brief Head of tcb linked list. The eventloop needs 
 *        to look up the affected tcbs in case of an incomming paket.
 *        ng_tcp_tcb_init() adds tcb to this list.
 */
static ng_tcp_tcb_t* _head_tcb_list = NULL;

/**
 * @brief Allocate memory for the TCP-Thread Stack
 */
#if ENABLE_DEBUG
static char _tcp_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _tcp_stack[NG_TCP_STACK_SIZE];
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Internal Functions: Implementation in Header Files.      */
/* Depend on each other, need to be included in that order! */
#include "ng_tcp_internal_constants.h"
#include "ng_tcp_internal_helpers.h"
#include "ng_tcp_internal_fsm.h"
#include "ng_tcp_internal_tcp_thread.h"

/* External NG_TCP API */
int8_t ng_tcp_tcb_init(ng_tcp_tcb_t *tcb)
{
    /* Clear Transmision Control Block */
    tcb->state = CLOSED;               /* CLOSED: State before any connection occurs */
    tcb->peer_addr = NULL;             /* Peer Address Unspecified */
    tcb->peer_addr_len = 0;            /* Peer Address Length is zero */
    tcb->peer_port = PORT_UNSPEC;  /* Port 0: Reserved. Marks Unassigned Port */
    tcb->local_port = PORT_UNSPEC; /* Port 0: Reserved. Marks Unassigned Port */

    /* Init Send Pointers */
    tcb->snd_una = 0;
    tcb->snd_nxt = 0;
    tcb->snd_wnd = 0;
    tcb->snd_ur = 0;
    tcb->snd_wl1 = 0;
    tcb->snd_wl2 = 0;
    tcb->iss = 0;

    /* Init Receive Pointers */
    tcb->rcv_nxt = 0;
    tcb->rcv_wnd = 0;
    tcb->irs = 0;

    /* Maximum Window Sizes */
    tcb->mss = 0;

    /* Clear Timeout */
    tcb->timeout.seconds = 0;
    tcb->timeout.microseconds = 0;

    tcb->owner = thread_getpid();
    tcb->next = NULL;

    /* Add this tcb to the tcb list */
    _add_tcb_to_list(tcb);
    return 0;
}

int8_t ng_tcp_tcb_destroy(ng_tcp_tcb_t *tcb)
{
    /* Guard: only tcb owner is allowed to use this on supplied tcb call */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    _rem_tcb_from_list(tcb);
    return 0;
}

int8_t ng_tcp_open(ng_tcp_tcb_t *tcb, uint16_t local_port, uint8_t *peer_addr,
                   size_t peer_addr_len, uint16_t peer_port, uint8_t options)
{
    msg_t msg;
    int8_t res = 0;

    /* Guard: only tcb owner is allowed to use this call on tcb  */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    /* Guard: tcb must be in state CLOSED */
    if(tcb->state != CLOSED){
        return -EALREADY;
    }

    /* Guard: address buffer must be supplied */
    if(peer_addr == NULL || peer_addr_len == 0){
        return -EFAULT;
    }

    /* Check parameters based on Mode */
    if((options & AI_PASSIVE)){
        /* Guard: passive connection, local_port must specified */
        if(local_port == PORT_UNSPEC){
            return -1;
        }
    }else{
        /* Guard: active connection, peer addr/port must be specified */
        if(peer_port == PORT_UNSPEC){
            return -EDESTADDRREQ;
        }
#ifdef MODULE_NG_IPV6
        if(ng_ipv6_addr_is_unspecified((ng_ipv6_addr_t*) peer_addr)){
            return -EDESTADDRREQ;
        }
#endif
    }

    /* Assign connection information.*/
    tcb->local_port = (options & AI_PASSIVE) ? local_port : _get_free_port(peer_port);
    tcb->peer_port = (options & AI_PASSIVE) ? PORT_UNSPEC : peer_port;
    tcb->peer_addr = peer_addr;
    tcb->peer_addr_len = peer_addr_len;
    tcb->options = options;

    /* Event CALL_OPEN happens */
    _fsm(tcb, NULL, CALL_OPEN);

    /* Wait till connection is established or closed or in close wait*/
    while(tcb->state != CLOSED && tcb->state != ESTABLISHED && tcb->state != CLOSE_WAIT){
        /* Wait for notification(state change or timer expired) */
        res = vtimer_msg_receive_timeout(&msg, tcb->timeout);

        /* Timer expired: Event TIME_TIMEOUT happend */
        if(res < 0){
            res = _fsm(tcb, NULL, TIME_TIMEOUT);
        }
    }

    return res;
}

int8_t ng_tcp_send(ng_tcp_tcb_t *tcb, uint8_t *buf, size_t byte_count, bool push, bool urgent)
{
    (void) buf;
    (void) byte_count;
    (void) push;
    (void) urgent;

    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    if(tcb->state != ESTABLISHED && tcb->state != CLOSE_WAIT){
        return -1;
    }
    return 0;
}

int8_t ng_tcp_recv(ng_tcp_tcb_t *tcb)
{
    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    if(tcb->state != ESTABLISHED){
        return -1;
    }
    return 0;
}

int8_t ng_tcp_close(ng_tcp_tcb_t *tcb)
{
    int8_t res = 0;

    /* Guard: only tcb owner is allowed to use this call, on tcb */
    if(tcb->owner != thread_getpid()){
        return -EPERM;
    }

    /* Connection must be established or in close_wait for this call */
    if(tcb->state != ESTABLISHED && tcb->state != CLOSE_WAIT){
        return -1;
    }

    /* Event CALL_CLOSE happens */
    res = _fsm(tcb, NULL, CALL_CLOSE);

    while(tcb->state != CLOSED){
        /* Sleep */
    }

    return res;
}

int ng_tcp_init(void)
{
    /* Using uninitialized stack contents as seed. */
    uint32_t seed[SEED_LEN];

    /* Guard: Check if thread is already running */
    if (_tcp_pid != KERNEL_PID_UNDEF) {
        return -1;
    }

    /* initialize Pseudo Random Number Generator */
    genrand_init_by_array(seed, sizeof(seed));

    /* start TCP thread */
    return thread_create(_tcp_stack, sizeof(_tcp_stack), NG_TCP_PRIO,
                         0, _tcp_event_loop, NULL, NG_TCP_THREAD_NAME);
}


int ng_tcp_calc_csum(const ng_pktsnip_t *hdr, const ng_pktsnip_t *pseudo_hdr)
{
    uint16_t csum = 0;

    if (hdr == NULL || pseudo_hdr == NULL) {
        DEBUG("tcp: hdr or pseudo_hdr were NULL\n");
        return -EFAULT;
    }

    if (hdr->type != NG_NETTYPE_TCP) {
        DEBUG("tcp: header was not tcp header\n");
        return -EBADMSG;
    }

    if((csum = _calc_csum(hdr, pseudo_hdr, hdr->next)) == 0){
        return -ENOENT;
    }

    ((ng_tcp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);
    return 0;
}

ng_pktsnip_t *ng_tcp_hdr_build(ng_pktsnip_t *payload,
                               uint8_t *src, size_t src_len,
                               uint8_t *dst, size_t dst_len)
{
    ng_pktsnip_t *res;
    ng_tcp_hdr_t *hdr;

    /* check parameters */
    if( src == NULL
     || dst == NULL
     || src_len != sizeof(uint16_t)
     || dst_len != sizeof(uint16_t)
    ){
        DEBUG("tcp: Address length was not sizeof(uint16_t)\n");
        return NULL;
    }

    /* allocate header */
    res = ng_pktbuf_add(payload, NULL, sizeof(ng_tcp_hdr_t), NG_NETTYPE_TCP);
    if(res == NULL) {
        DEBUG("tcp: No space left in packet buffer\n");
        return NULL;
    }

    hdr = (ng_tcp_hdr_t *) res->data;

    /* Clear Header */
    memset(hdr, 0, sizeof(ng_tcp_hdr_t));

    /* Initialize Header with sane Defaults */
    hdr->src_port = byteorder_htons(*((uint16_t *)src));
    hdr->dst_port = byteorder_htons(*((uint16_t *)dst));
    hdr->checksum = byteorder_htons(0);
    hdr->off_ctl = byteorder_htons(OFFSET_MAX);

    return res;
}

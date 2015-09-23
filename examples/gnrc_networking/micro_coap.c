/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       microcoap example server
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6.h"
#include "thread.h"

#include "coap.h"

#define ENABLE_DEBUG                (1)
#include "debug.h"

#if MODULE_MICROCOAP

#define COAP_LISTEN_PORT            5683
#define BUFSZ                       128

static void *_microcoap_server_thread(void *arg);
static void _coap_send(gnrc_pktsnip_t *buf, size_t len, udp_hdr_t *src_udp, ipv6_hdr_t *src_ip);
static int handle_get_response(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt);

#define MAX_RESPONSE_LEN 1500
static uint8_t response[MAX_RESPONSE_LEN] = "";

static const coap_endpoint_path_t path1 = PATH_ELEMENT2(foo1, bar1);
static const coap_endpoint_path_t path2 = PATH_ELEMENT2(foo2, bar2);
static const coap_endpoint_path_t path3 = PATH_ELEMENT2(foo3, bar3);
static const coap_endpoint_path_t path4 = PATH_ELEMENT2(foo4, bar4);
static const coap_endpoint_path_t path5 = PATH_ELEMENT2(foo5, bar5);
static const coap_endpoint_path_t path6 = PATH_ELEMENT2(foo6, bar6);
static const coap_endpoint_path_t path7 = PATH_ELEMENT2(foo7, bar7);
static const coap_endpoint_path_t path8 = PATH_ELEMENT2(foo8, bar8);

static const coap_endpoint_path_t path11 = PATH_ELEMENT2(foo1, bar2);
static const coap_endpoint_path_t path12 = PATH_ELEMENT2(foo2, bar3);
static const coap_endpoint_path_t path13 = PATH_ELEMENT2(foo3, bar4);
static const coap_endpoint_path_t path14 = PATH_ELEMENT2(foo4, bar5);
static const coap_endpoint_path_t path15 = PATH_ELEMENT2(foo5, bar6);
static const coap_endpoint_path_t path16 = PATH_ELEMENT2(foo6, bar7);
static const coap_endpoint_path_t path17 = PATH_ELEMENT2(foo7, bar8);
static const coap_endpoint_path_t path18 = PATH_ELEMENT2(foo8, bar9);

const coap_endpoint_t endpoints[] =
{
        { COAP_METHOD_GET, handle_get_response, &path1, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path2, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path3, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path4, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path5, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path6, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path7, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path8, COAP_CONTENTTYPE_TEXT_PLAIN },

        { COAP_METHOD_GET, handle_get_response, &path11, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path12, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path13, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path14, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path15, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path16, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_GET, handle_get_response, &path17, COAP_CONTENTTYPE_TEXT_PLAIN },
        { COAP_METHOD_POST, handle_get_response, &path18, COAP_CONTENTTYPE_TEXT_PLAIN },

    { (coap_method_t)0, NULL, NULL, COAP_CONTENTTYPE_NONE } /* marks the end of the endpoints array */
};

void create_response_payload(const uint8_t *buffer)
{
    char *response = "1337";
    memcpy((void*)buffer, response, strlen(response));
}

/* The handler which handles the path /foo/bar */
int handle_get_response(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    DEBUG("[endpoints]  %s()\n",  __func__);
    create_response_payload(response);
    /* NOTE: COAP_RSPCODE_CONTENT only works in a packet answering a GET. */
    return coap_make_response(scratch, outpkt, response, strlen((char*)response),
                              inpkt, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN, COAP_TYPE_ACK);
}

char _rcv_stack_buf[THREAD_STACKSIZE_DEFAULT];

int sock_rcv, if_id;

int coap_main(void)
{

    DEBUG("Starting example microcoap server...\n");

    //msg_init_queue(msg_q, RCV_MSG_Q_SIZE);

    thread_create(_rcv_stack_buf, sizeof(_rcv_stack_buf), THREAD_PRIORITY_MAIN - 2,
                  CREATE_STACKTEST, _microcoap_server_thread, NULL ,"_microcoap_server_thread");

    DEBUG("Ready to receive requests.\n");

    return 0;
}

static void *_microcoap_server_thread(void *arg)
{
    int rc;
    msg_t msg;
    coap_packet_t pkt;

    (void) arg;

    puts("initializing receive socket...");

    /* create our netreg entry to register our listening port */
    gnrc_netreg_entry_t entry;
    entry.next = NULL;
    entry.pid = thread_getpid();
    entry.demux_ctx = COAP_LISTEN_PORT;

    /* register our DNS response listener */
    if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &entry)) {
        DEBUG("coap: error starting service.");
        return NULL;
    }

    for (;;) {
        msg_receive(&msg);

        if (msg.type != GNRC_NETAPI_MSG_TYPE_RCV) {
            msg_send_to_self(&msg);         /* return the message to the queue */
            continue;
        }

        gnrc_pktsnip_t *pckt = (gnrc_pktsnip_t*)msg.content.ptr;
        udp_hdr_t *udp = pckt->next->data;
        ipv6_hdr_t *ipv6 = pckt->next->next->data;

        rc = coap_parse(&pkt, pckt->data, pckt->size);
        if (rc) {
            printf("Bad packet rc=%d\n", rc);
            continue;
        }

        uint8_t dat_buffer[256];
        coap_rw_buffer_t scratch_buf = { dat_buffer, sizeof(dat_buffer) };

        gnrc_pktsnip_t *outbuf = gnrc_pktbuf_add(NULL, NULL, 1028, GNRC_NETTYPE_UNDEF);

        if (!outbuf) {
            /* TODO couldn't allocate output buffer */
            continue;
        }

        coap_packet_t rsppkt;
        size_t rsplen = outbuf->size;

        coap_handle_req(&scratch_buf, &pkt, &rsppkt);
        rc = coap_build(outbuf->data, &rsplen, &rsppkt);
        if (rc) {
            printf("coap_build failed rc=%d\n", rc);
            continue;
        }

        _coap_send(outbuf, rsplen, udp, ipv6);
    }

    return NULL;
}

void _coap_send(gnrc_pktsnip_t *buf, size_t len, udp_hdr_t *src_udp, ipv6_hdr_t *src_ip) {
    network_uint16_t src_port, dst_port;
    gnrc_pktsnip_t *udp, *ip;

    if (gnrc_pktbuf_realloc_data(buf, len) != 0) {
        DEBUG("coap: failed to reallocate data snippet");
        return;
    }

    /* allocate UDP header, set source port := destination port */
    src_port.u16 = byteorder_ntohs(src_udp->dst_port);
    dst_port.u16 = byteorder_ntohs(src_udp->src_port);
    udp = gnrc_udp_hdr_build(buf, src_port.u8, sizeof(src_port),
                             dst_port.u8, sizeof(dst_port));
    if (udp == NULL) {
        DEBUG("coap: error unable to allocate UDP header");
        gnrc_pktbuf_release(buf);
        return;
    }

    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, 0, src_ip->src.u8, sizeof(ipv6_addr_t));
    if (ip == NULL) {
        DEBUG("coap: error unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return;
    }

    /* send packet */
    if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip) == 0) {
        /* if send failed inform the user */
        DEBUG("coap: error unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return;
    }
}

#endif

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

#define COAP_LISTEN_PORT            5683
#define BUFSZ                       128

static void *_microcoap_server_thread(void *arg);
static void _coap_send(gnrc_pktsnip_t *buf, size_t len, udp_hdr_t *src_udp, ipv6_hdr_t *src_ip);

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
            continue;
        }

        gnrc_pktsnip_t *pckt = (gnrc_pktsnip_t*)msg.content.ptr;
        udp_hdr_t *udp = pckt->next->data;
        ipv6_hdr_t *ipv6 = pckt->next->next->data;

        printf("Received packet: ");
        coap_dump(pckt->data, pckt->size, true);
        puts("\n");

        rc = coap_parse(&pkt, pckt->data, pckt->size);
        if (rc != 0) {
            printf("Bad packet rc=%d\n", rc);
        }
        else {
            gnrc_pktsnip_t *outbuf = gnrc_pktbuf_add(NULL, NULL, 1028, GNRC_NETTYPE_UNDEF);
            uint8_t dat_buffer[256];
            //coap_rw_buffer_t scratch_buf = { (uint8_t*)outbuf->data, outbuf->size };
            coap_rw_buffer_t scratch_buf = { dat_buffer, sizeof(dat_buffer) };

            if (!outbuf) {
                /* TODO couldn't allocate output buffer */
                continue;
            }

            coap_packet_t rsppkt;
            size_t rsplen = outbuf->size;

            printf("\ncontent: ");
            coap_dumpPacket(&pkt);
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(outbuf->data, &rsplen, &rsppkt))) {
                printf("coap_build failed rc=%d\n", rc);
            }
            else {
                printf("\nSending packet: ");
                coap_dump(outbuf->data, rsplen, true);
                puts("\n");
                printf("content:\n");
                coap_dumpPacket(&rsppkt);
                _coap_send(outbuf, rsplen, udp, ipv6);
            }
        }
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

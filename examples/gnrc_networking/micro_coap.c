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
static int handle_response(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, coap_method_t method, const coap_endpoint_t *endpoint, void *args);

typedef struct
{
    coap_endpoint_request request;
} coap_context_t;

static coap_context_t context;

#define MAX_RESPONSE_LEN                    500
static uint8_t response[MAX_RESPONSE_LEN] = "";

static const coap_endpoint_path_t str_ep1 = PATH_ELEMENT2(AxAvior String EP1, axav, str_ep1);
static const coap_endpoint_path_t str_ep2 = PATH_ELEMENT2(AxAvior String EP2, axav, str_ep2);
static const coap_endpoint_path_t str_ep3 = PATH_ELEMENT2(AxAvior String EP3, axav, str_ep3);
static const coap_endpoint_path_t str_ep4 = PATH_ELEMENT2(AxAvior String EP4, axav, str_ep4);
static const coap_endpoint_path_t str_ep5 = PATH_ELEMENT2(AxAvior String EP5, axav, str_ep5);
static const coap_endpoint_path_t bin_ep1 = PATH_ELEMENT2(AxAvior Binary EP1, axav, bin_ep1);
static const coap_endpoint_path_t bin_ep2 = PATH_ELEMENT2(AxAvior Binary EP2, axav, bin_ep2);
static const coap_endpoint_path_t bin_ep3 = PATH_ELEMENT2(AxAvior Binary EP3, axav, bin_ep3);
static const coap_endpoint_path_t bin_ep4 = PATH_ELEMENT2(AxAvior Binary EP4, axav, bin_ep4);
static const coap_endpoint_path_t bin_ep5 = PATH_ELEMENT2(AxAvior Binary EP5, axav, bin_ep5);

char _rcv_stack_buf[THREAD_STACKSIZE_DEFAULT];

const coap_endpoint_t endpoints[] =
{
        // AxAvior String Endpoint 1
        { COAP_METHOD_ALL,      handle_response, &str_ep1, COAP_CONTENTTYPE_TEXT_PLAIN },
        // AxAvior String Endpoint 2
        { COAP_METHOD_ALL,      handle_response, &str_ep2, COAP_CONTENTTYPE_TEXT_PLAIN },
        // AxAvior String Endpoint 3
        { COAP_METHOD_ALL,      handle_response, &str_ep3, COAP_CONTENTTYPE_TEXT_PLAIN },
        // AxAvior String Endpoint 4
        { COAP_METHOD_ALL,      handle_response, &str_ep4, COAP_CONTENTTYPE_TEXT_PLAIN },
        // AxAvior String Endpoint 5
        { COAP_METHOD_ALL,      handle_response, &str_ep5, COAP_CONTENTTYPE_TEXT_PLAIN },
        // AxAvior Binary Endpoint 1
        { COAP_METHOD_ALL,      handle_response, &bin_ep1, COAP_CONTENTTYPE_APPLICATION_OCT_STREAM },
        // AxAvior Binary Endpoint 2
        { COAP_METHOD_ALL,      handle_response, &bin_ep2, COAP_CONTENTTYPE_APPLICATION_OCT_STREAM },
        // AxAvior Binary Endpoint 3
        { COAP_METHOD_ALL,      handle_response, &bin_ep3, COAP_CONTENTTYPE_APPLICATION_OCT_STREAM },
        // AxAvior Binary Endpoint 4
        { COAP_METHOD_ALL,      handle_response, &bin_ep4, COAP_CONTENTTYPE_APPLICATION_OCT_STREAM },
        // AxAvior Binary Endpoint 5
        { COAP_METHOD_ALL,      handle_response, &bin_ep5, COAP_CONTENTTYPE_APPLICATION_OCT_STREAM },

        // marks the end of the endpoints array
        { (coap_method_t)0, NULL, NULL, COAP_CONTENTTYPE_NONE }
};

#if 0
#define COAP_MSG_OBSERVE_TIMER              31337
static msg_t _observe_timer;
static int _coap_observe_counter;
static const coap_packet_t _observe_req;
static coap_endpoint_t *_observe_endpoint;
#endif

/* The handler which handles the path /foo/bar */
int handle_response(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, coap_method_t method, const coap_endpoint_t *endpoint, void *arg)
{
#if 0
    uint8_t count;
    const coap_option_t *opt;
#endif
    coap_responsecode_t respcode;
    coap_context_t *ctxt = (coap_context_t*)arg;

    DEBUG("[endpoints]  %s()\n",  __func__);

#if 0
    // check and validate the observe option
    opt = coap_findOptions(inpkt, COAP_OPTION_OBSERVE, &count);
    if (opt)
    {
        if (!opt->buf.len)
        {
            // check if we have a token
            if (!inpkt->tok.len)
                return COAP_ERR_TOKEN_TOO_SHORT;

            // add to observer list
            memcpy(&_observe_req, inpkt, sizeof(_observe_req));
            _observe_timer.sender_pid = thread_getpid();
            _observe_timer.type = COAP_MSG_OBSERVE_TIMER;
            xtimer_msg_receive_timeout(_observe_timer, 2000000);
        }
        else
            return COAP_ERR_OPTION_LEN_INVALID;
    }
#endif

    size_t length = inpkt->payload.len;
    memcpy(response, inpkt->payload.p, inpkt->payload.len);
    respcode = ctxt->request(method, endpoint->path->elems[endpoint->path->count - 1].str, response, &length, sizeof(response));
    return coap_make_response(scratch, outpkt, response, length, inpkt, respcode, endpoint->core_attr, COAP_TYPE_ACK);
}

int coap_main(coap_endpoint_request request)
{
    DEBUG("Starting example microcoap server...\n");

    context.request = request;
    thread_create(_rcv_stack_buf, sizeof(_rcv_stack_buf), THREAD_PRIORITY_MAIN - 2,
                  CREATE_STACKTEST, _microcoap_server_thread, &context,"_microcoap_server_thread");

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

#if 0
        if (msg.type == COAP_MSG_OBSERVE_TIMER) {
            size_t resplen;
            coap_packet_t packet;
            coap_rw_buffer_t *scratch;
            coap_responsecode_t respcode;

            // resent the CoAP response
            int length = create_response_payload(response, COAP_METHOD_GET, &respcode);
            coap_make_response(scratch, &packet, response, length, &_observe_req, respcode, _observe_endpoint->core_attr, COAP_TYPE_ACK);
            rc = coap_build(packet->data, &rsplen, &packet);
            if (rc) {
                printf("coap_build failed rc=%d\n", rc);
                continue;
            }

            _coap_send(outbuf, rsplen, udp, ipv6);

            // add to observer list
            _observe_timer.sender_pid = thread_getpid();
            _observe_timer.type = COAP_MSG_OBSERVE_TIMER;
            xtimer_msg_receive_timeout(_observe_timer, 2000000);
        }
        else
#endif
        if (msg.type != GNRC_NETAPI_MSG_TYPE_RCV) {
#if 0
            msg_send_to_self(&msg);         /* return the message to the queue */
#endif
            continue;
        }

        printf("start processing request\n");

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
            printf("error: failed to allocate all packets\n");
            continue;
        }

        coap_packet_t rsppkt;
        size_t rsplen = outbuf->size;

        coap_handle_req(&scratch_buf, &pkt, &rsppkt, arg);
        rc = coap_build(outbuf->data, &rsplen, &rsppkt);
        if (rc) {
            printf("coap_build failed rc=%d\n", rc);
            continue;
        }

        _coap_send(outbuf, rsplen, udp, ipv6);

        printf("end processing request\n");
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

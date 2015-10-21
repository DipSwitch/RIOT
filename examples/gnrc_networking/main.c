/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "coap.h"

#include "shell.h"

extern int coap_main(coap_endpoint_request request);
extern int udp_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

static const char *_coap_method_translate(coap_method_t method)
{
    switch (method)
    {
    case COAP_METHOD_GET:
        return "get";

    case COAP_METHOD_POST:
        return "post";

    case COAP_METHOD_PUT:
        return "put";

    case COAP_METHOD_DELETE:
        return "delete";

    default:
        return "fault";
    }
}

#define MIN(a, b)               ((a) > (b) ? (b) : (a))

coap_responsecode_t create_response_payload(coap_method_t method, const char *name, uint8_t *value, size_t *len, size_t max_len)
{
    const char *mthd = _coap_method_translate(method);

    int offset = sprintf(NULL, "%s|%s|%d|", name, mthd, *len);
    memcpy(value + offset, value, *len);
    snprintf((char*)value, max_len, "%s|%s|%d|", name, mthd, *len);
    *len += offset;

    switch (method)
    {
    case COAP_METHOD_GET:
        return COAP_RSPCODE_CONTENT;

    case COAP_METHOD_PUT:
        return COAP_RSPCODE_CHANGED;

    case COAP_METHOD_POST:
        return COAP_RSPCODE_CREATED;

    case COAP_METHOD_DELETE:
        return COAP_RSPCODE_DELETED;

    default:
        *value = '-';
        return COAP_RSPCODE_METHOD_NOT_ALLOWED;
    }
}

int main(void)
{
    coap_main(create_response_payload);

    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}

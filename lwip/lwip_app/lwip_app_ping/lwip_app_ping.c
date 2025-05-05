/**
 * @file
 * Ping sender module
 *
 */

/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */

/**
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 *
 */
#include "lwip_app_ping.h"

#ifdef LWIP_APP_PING
#include "lwip/opt.h"
#include "lwip/icmp.h"
#include "lwip/ip.h"
#include "lwip/inet_chksum.h"
//#include "lwip/timers.h"
#include "lwip/raw.h"
#include "FreeRTOS.h"
#include "task.h"


//struct ip_addr ping_target;
extern struct ip_addr rmtipaddr;

/* ping target - should be a "ip_addr_t" */
#ifndef PING_TARGET
#define PING_TARGET (netif_default ? netif_default->gw : ip_addr_any)
#endif

/* ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

/* ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY 1000
#endif

/* ping identifier - must fit on a uint16_t */
#ifndef PING_ID
#define PING_ID 0xAFAF
#endif
/* ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

/* ping variables */
static uint16_t ping_seq_num;
static uint32_t ping_time;
static struct raw_pcb *ping_pcb = NULL;

void* pingThread;

/* Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++)
    {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the raw ip */
static uint8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;

    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);

    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) && pbuf_header(p, -PBUF_IP_HLEN) == 0)
    {
        iecho = (struct icmp_echo_hdr *)p->payload;

        if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num)))
        {
            printf("\r\n ping: recv ");
            printf("%u.%u.%u.%u ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3]);
            printf(" %u ms \r\n", (unsigned int) ((xTaskGetTickCount() - ping_time) * portTICK_RATE_MS));
            /* do some ping result processing */
            PING_RESULT(1);
            pbuf_free(p);
            return 1; /* eat the packet */
        }
        /* not eaten, restore original packet */
        pbuf_header(p, PBUF_IP_HLEN);
     }

     return 0; /* don't eat the packet */
}

static void ping_raw_init(void)
{
    ping_pcb = raw_new(IP_PROTO_ICMP);
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

    raw_recv(ping_pcb, ping_recv, NULL);
    raw_bind(ping_pcb, IP_ADDR_ANY);
}
void ping_raw_deinit(void)
{
    if(ping_pcb)
    {
        raw_remove(ping_pcb);
        ping_pcb = NULL;
    }
}
int ping(ip_addr_t *addr,int data_size)
{
    struct pbuf *p;
    struct icmp_echo_hdr *iecho;
    size_t ping_size;

    if((data_size < 0)||(data_size > MAX_PING_DATA_SIZE))
        return -1;
    else
        ping_size = sizeof(struct icmp_echo_hdr) + data_size;

    printf("\r\n ping: send ");
    printf("%u.%u.%u.%u \r", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3]);
    printf("\r\n");
    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

    taskENTER_CRITICAL();
    p = pbuf_alloc(PBUF_IP, (uint16_t)ping_size, PBUF_RAM);
    if (!p)
    {
        taskEXIT_CRITICAL();
        return -3;
    }

    if ((p->len == p->tot_len) && (p->next == NULL))
    {
        iecho = (struct icmp_echo_hdr *)p->payload;
        ping_prepare_echo(iecho, (uint16_t)ping_size);
        if(ping_pcb == NULL){
            ping_raw_init();
        }
        raw_sendto(ping_pcb, p, addr);
        ping_time = xTaskGetTickCount();
    }
    pbuf_free(p);
    taskEXIT_CRITICAL();

    return 0;
}
#endif

////////////////////////////////////////////////







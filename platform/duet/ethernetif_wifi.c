/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include <string.h>
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dhcp6.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "lwip/err.h"
#include "ethernetif_wifi.h"
#include "lwipopts.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "duet_common.h"
#include "lega_wlan_api.h"
#ifdef LWIP_DUALSTACK
#include "lwip/ethip6.h"
#endif
/* Define those to better describe your network interface. */
#define IFNAME0 'w'
#define IFNAME1 'n'
#define ETH_RX_BUF_SIZE_WIFI 1500

void low_level_init(struct netif *netif)
{
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    netif->mtu=1500;

    netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP|NETIF_FLAG_IGMP;
#ifdef LWIP_DUALSTACK
    netif->flags |=  NETIF_FLAG_MLD6;
#endif
}

extern int lega_lwip_low_level_output(TCPIP_PACKET_INFO_T *pkt, void *eif);
FLASH_COMMON_SEG err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint16_t index = 0;
    uint16_t pbuf_count = 0;
    err_t err = ERR_OK;
    int ret;

    TCPIP_PACKET_INFO_T    tcpip_packet;
    TCPIP_PACKET_INFO_T    *p_packet = &tcpip_packet;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    //20210826 bug3317
    for(q = p; q != NULL; q = q->next)
    {
        pbuf_count++;
    }
    p_packet->packet_len = 0;
    p_packet->pbuf_num = pbuf_count;
    p_packet->data_ptr = lega_rtos_malloc(pbuf_count*sizeof(uint8_t*));
    if(NULL == p_packet->data_ptr)
    {
        err = ERR_MEM;
        return err;
    }
    p_packet->data_len = lega_rtos_malloc(pbuf_count*sizeof(uint16_t));
    if(NULL == p_packet->data_len)
    {
        err = ERR_MEM;
        lega_rtos_free(p_packet->data_ptr);
        return err;
    }
    for(q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        p_packet->data_ptr[index] = q->payload;
           p_packet->data_len[index] = q->len;
           p_packet->packet_len += q->len;
           index++;
    }
    p_packet->pbuf_num = index;

    ret = lega_lwip_low_level_output(p_packet, netif->state);
    if(ret != 0) {
        //printf("tx drop! %d",ret);
        err = ERR_WOULDBLOCK;
    }

    lega_rtos_free(p_packet->data_len);
    lega_rtos_free(p_packet->data_ptr);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    return err;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
struct pbuf * low_level_input(struct netif *netif, RX_PACKET_INFO_T *rx_packet)
{
    struct pbuf *p=NULL;
    struct pbuf *q;
    u16_t len;
    int index;

    unsigned char *rx_buf = rx_packet->data_ptr;

    len = rx_packet->data_len;

    if(len==0){
        //DEBUG_INFO("eth rx no data");
        return NULL;
    }

    if(len==(ETH_RX_BUF_SIZE_WIFI-1)){
        printf("eth rx buf size not enough\n");
    }

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL) {
#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    index = 0;
    /* We iterate over the pbuf chain until we have read the entire
    * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
    /* Read enough bytes to fill this pbuf in the chain. The
    * available data in the pbuf is given by the q->len
    * variable.
    * This does not necessarily have to be a memcpy, you can also preallocate
    * pbufs for a DMA-enabled MAC and after receiving truncate it to the
    * actually received size. In this case, ensure the tot_len member of the
    * pbuf is the sum of the chained pbuf len members.
    */
    MEMCPY(q->payload, (rx_buf+index), q->len);
        index += q->len;
    }

    //DEBUG_ASSERT(index == p->tot_len);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    }

    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void lwip_interface_ethernetif_input(void *net_if,  RX_PACKET_INFO_T *rx_packet)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;
    struct netif *netif = (struct netif*)net_if;


    /* move received packet into a new pbuf */
    p = low_level_input(netif, rx_packet);

    /* no packet could be read, silently ignore this */
    if (p == NULL) return;
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type)) {
        /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
        #if PPPOE_SUPPORT
        /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
        #endif /* PPPOE_SUPPORT */
#ifdef LWIP_DUALSTACK
        case ETHTYPE_IPV6:
#endif
        /* full packet send to tcpip_thread to process */
        if (netif->input(p, netif)!=ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
        break;

        default:
            pbuf_free(p);
            p = NULL;
            break;
    }
}

#define DEFAULT_HOSTNAME "asr5821"
__attribute__((weak)) char* lega_wlan_netif_get_hostname(void)
{
    char* host_name = DEFAULT_HOSTNAME;
    return host_name;
}

extern void *lega_lwip_ethernetif_init(struct netif *netif, void *fn);
err_t lwip_interface_ethernetif_init(struct netif *netif)
{
    void *p;

    LWIP_ASSERT("netif!=NULL",(netif!=NULL));

    p = lega_lwip_ethernetif_init(netif, (void*)lwip_interface_ethernetif_input);

#if LWIP_NETIF_HOSTNAME
    if(netif->hostname == NULL)
        netif->hostname = lega_wlan_netif_get_hostname();
#endif

    netif->state        = p;
    netif->name[0]      = IFNAME0;
    netif->name[1]      = IFNAME1;
    netif->output       = etharp_output;
#ifdef LWIP_DUALSTACK
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput   = low_level_output;

    low_level_init(netif);

    return ERR_OK;
}

#ifdef CFG_STA_AP_COEX
extern struct netif lwip_netif[2];
void lwip_interface_init(uint8_t openmode, struct netif *netif, uint8_t netif_num)
#else
void lwip_interface_init(struct netif *netif, uint8_t netif_num)
#endif
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
#ifdef LWIP_DUALSTACK
    uint8_t mac[6];
#endif

    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);

#ifdef LWIP_DUALSTACK
    netif->hwaddr_len = ETH_HWADDR_LEN;
    lega_wlan_get_mac_address(mac);
    printf("get mac :%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    memcpy(netif->hwaddr,mac,ETH_HWADDR_LEN);
#endif

    netifapi_netif_add(netif,&ipaddr,&netmask,&gw,NULL,&lwip_interface_ethernetif_init,&tcpip_input);

#ifdef CFG_STA_AP_COEX
    if(openmode == 0x01 || (openmode == 0x02 && !netif_is_up(&lwip_netif[0])))
#endif
    netifapi_netif_set_default(netif);

    netif->num = netif_num;
}

extern void lega_lwip_dhcp_disconnect_wifi(void);
#ifdef LWIP_DUALSTACK
extern int lega_lwip_dhcp_get_store_info_ptr(char **ip, char **gate, char **mask, char **bcastip, char **ip6);
extern void lega_lwip_got_ip6_action(void);
#else
extern int lega_lwip_dhcp_get_store_info_ptr(char **ip, char **gate, char **mask, char **bcastip);
#endif
extern void lega_lwip_got_ip_action(void);
extern void lega_lwip_got_ip_fail_action(void);
void lwip_interface_ethernetif_status_callback(struct netif *netif)
{
    char *ip = 0;
    char *gate = 0;
    char *mask = 0;
    char *bcastip = 0;
#ifdef LWIP_DUALSTACK
    char *ip6 = 0;
#endif
    int ret;
#if LWIP_DHCP
    ip_addr_t old_ipaddr;
    ip4_addr_t bcast_ip;
#endif
#ifdef LWIP_DUALSTACK
    ret = lega_lwip_dhcp_get_store_info_ptr(&ip, &gate, &mask, &bcastip, &ip6);
#else
    ret = lega_lwip_dhcp_get_store_info_ptr(&ip, &gate, &mask, &bcastip);
#endif
    if(ret != 0){
        return;
    }

    // maybe link down, need reconnect
    if (netif == NULL || !netif_is_up(netif) || !netif_is_link_up(netif))
    {
        printf("netif is NULL or down\n");
        lega_lwip_dhcp_disconnect_wifi();
        return;
    }

#ifdef LWIP_DUALSTACK
    int i=0;
    int valid_ip6 = 0;

    if(netif_is_flag_set(netif,NETIF_FLAG_CALLBACK_IPV6_EVENT)){
        for(i=0;i<LWIP_IPV6_NUM_ADDRESSES;i++){
            //printf("ipv6addr #%d: %s state 0x%x \n", i, inet6_ntoa(netif->ip6_addr[i].u_addr.ip6), netif->ip6_addr_state[i]);
            inet6_ntoa_r(netif->ip6_addr[i].u_addr.ip6, ip6 + i*sizeof(lega_ip6_addr_t), 40); //v6 ip
            ip6[(i+1)*sizeof(lega_ip6_addr_t) - 1] = netif->ip6_addr_state[i]; //v6 state

            if(netif->ip6_addr_state[i] & (IP6_ADDR_TENTATIVE|IP6_ADDR_VALID))
                valid_ip6++;
        }
        if(valid_ip6)
            lega_lwip_got_ip6_action();
        netif_clear_flags(netif,NETIF_FLAG_CALLBACK_IPV6_EVENT);
        return;
    }
#endif

#if LWIP_DHCP
    if (!ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), IP4_ADDR_ANY4))
    {
        /* update ip to config info */
        bcast_ip.addr = (ip_2_ip4(&netif->gw)->addr | (~(ip_2_ip4(&netif->netmask)->addr)));//need to confirm
        inet_ntoa_r(*(ip_2_ip4(&netif->ip_addr)), ip, 16);
        inet_ntoa_r(*(ip_2_ip4(&netif->gw)), gate, 16);
        inet_ntoa_r(*(ip_2_ip4(&netif->netmask)), mask, 16);
        inet_ntoa_r(bcast_ip, bcastip, 16);

        lega_lwip_got_ip_action();
    }
    else
    {
        inet_aton((const char*)ip, &old_ipaddr);
        if (!ip4_addr_cmp(ip_2_ip4(&old_ipaddr), IP4_ADDR_ANY4))
        {
            lega_lwip_got_ip_fail_action();
        }
    }
#endif
}

void lwip_interace_static_ip_config_start(struct netif *netif, char *ip, char *gate, char *mask, char *dns)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
    ip4_addr_t dns_;

    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&dns_, 0, 0, 0, 0);

    printf("static ip start\n");

    netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw);
    netifapi_netif_set_up(netif);
    netif_set_status_callback(netif, lwip_interface_ethernetif_status_callback);

    ipaddr_aton(ip, (ip_addr_t*)(&ipaddr));
    ipaddr_aton(gate, (ip_addr_t*)(&gw));
    ipaddr_aton(mask, (ip_addr_t*)(&netmask));
    ipaddr_aton(dns, (ip_addr_t*)(&dns_));

    //set ip/gw/netmask
    if(!(ipaddr.addr && gw.addr && netmask.addr))
        printf("static ip config not valid\n");
    else
        netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw);

    //set dns
    if(!dns_.addr)
        dns_setserver(0,(ip_addr_t*)(&gw));
    else
        dns_setserver(0,(ip_addr_t*)(&dns_));
}

void lwip_interace_static_ip_config_stop(struct netif *netif)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);

    printf("static ip stop\n");

    netif_set_status_callback(netif, NULL);
    netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw);
    netifapi_netif_set_down(netif);
}

#if LWIP_DHCP
#if 1 //defined ALIOS_SUPPORT || defined LWIP_2_1_2
#define NET_DHCP_DATA(nif)      netif_dhcp_data(nif)
#define NET_DHCP_STATE(nif)     (netif_dhcp_data(nif)->state)
#define NET_DHCP_STATE_OFF      DHCP_STATE_OFF
#else
#define NET_DHCP_DATA(nif)      (nif->dhcp)
#define NET_DHCP_STATE(nif)     (nif->dhcp->state)
#define NET_DHCP_STATE_OFF      DHCP_OFF
#endif
#endif
int lwip_comm_dhcp_start(struct netif *netif)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    if (NET_DHCP_DATA(netif) &&
        NET_DHCP_STATE(netif) != NET_DHCP_STATE_OFF)
    {
        printf("DHCP is already started\n");
        return ERR_USE;
    }

    printf("dhcp start\n");

    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);
#ifdef LWIP_DUALSTACK
    nd6_restart_netif(netif);
    nd6_restart_rs(netif);
    netif_set_ip6_autoconfig_enabled(netif,1);
    //enable manual
    if(lwip_comm_dhcp6_status() == 1)
        dhcp6_enable_stateless(netif);
#endif
    netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw);
    netifapi_netif_set_up(netif);
    netif_set_status_callback(netif, lwip_interface_ethernetif_status_callback);
#ifdef LWIP_DUALSTACK
    netif_create_ip6_linklocal_address(netif,1);
#endif
    return netifapi_dhcp_start(netif);
}

void lwip_comm_dhcp_stop(struct netif *netif)
{
    printf("dhcp stop\n");

    netif_set_status_callback(netif, NULL);
#ifdef LWIP_DUALSTACK
    int i = 0;
    for(i=0; i<LWIP_IPV6_NUM_ADDRESSES ;i++)
    {
      ip_addr_set_zero_ip6(&(netif->ip6_addr[i]));
      netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
    }
    if(lwip_comm_dhcp6_status() == 1)
        dhcp6_disable(netif);
#endif
    netifapi_dhcp_release(netif);
    netifapi_dhcp_stop(netif);
    netifapi_netif_set_down(netif);
}

extern void uwifi_msg_lwip2u_peer_sta_ip_update(uint32_t ipstat,uint32_t macstat, uint32_t net_if);
uint8_t client_macaddr[6]={0};
uint32_t client_ipaddr;
void ethernetif_ap_status_d_callback(ip4_addr_t *client_ip, uint8_t *macaddr, struct netif *netif)
{
     client_ipaddr = *(uint32_t*)client_ip;
     memcpy(client_macaddr, macaddr, 6);
     uwifi_msg_lwip2u_peer_sta_ip_update((uint32_t)(&client_ipaddr),(uint32_t)(client_macaddr),(uint32_t)(netif));
}


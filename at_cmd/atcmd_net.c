/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef AT_USER_DEBUG

#include "lega_at_api.h"
#include "lwip/ip_addr.h"
#ifdef LWIP_APP_PING
#include "lwip_app_ping.h"
#endif
#ifdef LWIP_APP_IPERF
#include "lwip_app_iperf.h"
#endif
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/def.h"
#ifdef CFG_PLF_DUET
#include "duet_common.h"
#else
#include "lega_common.h"
#endif

#define     BUFFER_LEN                          1025
#define     MAX_DATA_LEN                        1024
#define     TCPUDP_CLIENT_NUM                   4
#define     HAL_WAIT_FOREVER                    0xFFFFFFFFU
#define     NET_UART_BUF_QUEUE_BYTES            1024

#define     MAX_SERVER_CLIENT_NUM               1
#define     NET_SERVER_TASK_NAME                "net_server_task"
#define     NET_SERVER_STACK_SIZE               1024
#define     NET_SERVER_TASK_PRIORITY            3
lega_thread_t Net_Server_Task_Handler = NULL;

int g_sockfd_tcp_server = -1;
int g_connfd_tcp_server = -1;
int g_sockfd_tcp_client[TCPUDP_CLIENT_NUM] = {0};
int g_sockfd_udp_client[TCPUDP_CLIENT_NUM] = {0};

#ifdef CFG_PLF_DUET
lega_queue_t net_uart_buf_queue[DUET_UART_NUM];
#else
lega_queue_t net_uart_buf_queue[LEGA_UART_NUM];
#endif

struct sockaddr_in g_udp_remote_addr[TCPUDP_CLIENT_NUM];


#ifdef CFG_PLF_DUET
char * net_uart_buf_queue_name[DUET_UART_NUM] =
#else
char * net_uart_buf_queue_name[LEGA_UART_NUM] =
#endif
{
    "uart0_buffer_queue",
    "uart1_buffer_queue",
    "uart2_buffer_queue"
};

extern _at_user_info at_user_info;
extern uint8_t g_printf_uart;
extern void at_uart_init(void);

 /*
 ************************************************************
 *              TCP/IP  AT CMD START
 *
 ************************************************************
 */

void lega_at_net_socket_init(void)
{
    int i = 0;
    for(i = 0; i < TCPUDP_CLIENT_NUM; i++)
    {
        g_sockfd_tcp_client[i] = -1;
        g_sockfd_udp_client[i] = -1;
    }
    g_sockfd_tcp_server = -1;
    g_connfd_tcp_server = -1;
}

static int lega_at_net_status(int argc, char **argv)
{
    struct sockaddr_in local, guest;
    int i = 0;
    int ret = 0;
    socklen_t local_len = sizeof(local);
    socklen_t guest_len = sizeof(guest);

    for(i = 0; i < TCPUDP_CLIENT_NUM; i++)
    {
         memset(&local, 0, sizeof(struct sockaddr_in));
         memset(&guest, 0, sizeof(struct sockaddr_in));
         if(g_sockfd_tcp_client[i] == -1)
         {
             at_rspdata("tcp%d is not use\r\n", i);
         }
         else
         {
            at_rspdatann("tcp%d ", i);
            ret = getsockname(g_sockfd_tcp_client[i], (struct sockaddr *)&local, &local_len);
            if(ret == 0)
             at_rspinfornn("local=%s:%d",
                inet_ntoa(local.sin_addr), (ntohs(local.sin_port)));

            ret = getpeername(g_sockfd_tcp_client[i], (struct sockaddr *)&guest, &guest_len);
            if(ret == 0)
             at_rspinfornn(", remote=%s:%d",
                inet_ntoa(guest.sin_addr), (ntohs(guest.sin_port)));

            at_rspinfor("\r\n");
         }
         if(g_sockfd_udp_client[i] == -1)
         {
            at_rspdata("udp%d is not use\r\n", i);
         }
         else
         {
            at_rspdatann("udp%d ", i);
            ret = getsockname(g_sockfd_udp_client[i], (struct sockaddr *)&local, &local_len);
            if(ret == 0)
                at_rspinfornn("local=%s:%d",
                    inet_ntoa(local.sin_addr), (ntohs(local.sin_port)));

            ret = getpeername(g_sockfd_udp_client[i], (struct sockaddr *)&guest, &guest_len);
            if(ret == 0)
                at_rspinfornn(", remote=%s:%d",
            inet_ntoa(guest.sin_addr), (ntohs(guest.sin_port)));

            at_rspinfor("\r\n");
         }

    }
    at_rspdatann("tcp server: ");
    if(g_sockfd_tcp_server != -1)
    {
        if(g_connfd_tcp_server != -1)
        {
            ret = getsockname(g_sockfd_tcp_server, (struct sockaddr *)&local, &local_len);
            if(ret == 0)
                at_rspinfornn("local=%s:%d",
                    inet_ntoa(local.sin_addr), (ntohs(local.sin_port)));

            ret = getpeername(g_sockfd_tcp_server, (struct sockaddr *)&guest, &guest_len);
            if(ret == 0)
                at_rspinfornn(", remote=%s:%d",
                    inet_ntoa(guest.sin_addr), (ntohs(guest.sin_port)));

             at_rspinfor("\r\n");
        }
        else
        {
            at_rspinfor("is not connected\r\n");
        }
    }
    else
    {
        at_rspinfor("is not use\r\n");
    }

    return CONFIG_OK;
}


static int lega_at_net_conn(int argc, char **argv)
{
    int tcpudp_num = convert_str_to_int(argv[2]);
    int tempValue;
    struct sockaddr_in remote_addr;
    if((at_user_info.sta_connected == 0) && (at_user_info.sap_opend == 0))
    {
        dbg_at("connect wifi first\r\n");
        return CONFIG_FAIL;
    }

    if(argc != 5)
    {
        dbg_at("para error\r\n");
        dbg_at("net_conn tcp 3 192.168.0.1 8000[]\r\n");
        return PARAM_MISS;
    }

    if(tcpudp_num < 0 || tcpudp_num >= 4)
    {
        dbg_at("tcp/udp num error: %d\r\n", tcpudp_num);
        return PARAM_RANGE;
    }
    /*setting server ip*/
    memset(&remote_addr, 0, sizeof(struct sockaddr_in));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(argv[3]);
    if(remote_addr.sin_addr.s_addr == INADDR_NONE)
    {
        dbg_at("host or ipaddr input error\r\n");
        return CONFIG_FAIL;
    }
    dbg_at("remote_addr = %s\r\n", inet_ntoa(remote_addr.sin_addr));

    tempValue = convert_str_to_int(argv[4]);
    if(tempValue == LEGA_STR_TO_INT_ERR || tempValue <= 0)
    {
        dbg_at("set remote port, para num err\r\n");
        return CONFIG_FAIL;
    }
    remote_addr.sin_port = htons(tempValue);

    if(strcmp(argv[1],"tcp") == 0)
    {
        if(g_sockfd_tcp_client[tcpudp_num] != -1)
        {
            dbg_at("tcp %d already exist, please close first\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        /*create socket*/
        if((g_sockfd_tcp_client[tcpudp_num]= socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            dbg_at("client socket create error\r\n");
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("client socket create success\r\n");
        }
        /*connect server*/
        if(connect(g_sockfd_tcp_client[tcpudp_num], (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
        {
            dbg_at("client connect server error\r\n");
            dbg_at("connect errno: %d\r\n", errno);
            g_sockfd_tcp_client[tcpudp_num] = -1;
            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("client connect server success\r\n");

            return CONFIG_OK;
        }
    }
    else if(strcmp(argv[1],"udp") == 0)
    {
        if(g_sockfd_udp_client[tcpudp_num] != -1)
        {
            dbg_at("udp %d already exist, please close first\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }

        if((g_sockfd_udp_client[tcpudp_num] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            dbg_at("udp client create error\r\n");

            return CONFIG_FAIL;
        }
        else
        {
            dbg_at("udp client create succeed\r\n");
            g_udp_remote_addr[tcpudp_num] = remote_addr;

            return CONFIG_OK;
        }
    }
    else
    {
        dbg_at("send type error\r\n");

        return PARAM_RANGE;
    }
}

static int lega_at_net_send(int argc, char *argv[])
{
    int data_len = 0;
    char data_buf[BUFFER_LEN] = {0};
    int sin_size;
    int tcpudp_num = 0;
    sin_size = sizeof(struct sockaddr_in);

    if(strcmp(argv[1], "tcp") == 0)
    {
        if(argc != 4)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_tcp_client[tcpudp_num] == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }
        memset(data_buf, 0, sizeof(data_buf));
        strcpy(data_buf, argv[3]);
        if(strlen(data_buf) > 112)
            dbg_at("waring: data length must less than 112, now is %d\n", strlen(data_buf));
        data_len = send(g_sockfd_tcp_client[tcpudp_num], data_buf, strlen(data_buf), 0);
        if(data_len != strlen(argv[3]))
        {
            dbg_at("tcp send error, data_len = %d", data_len);
            return CONFIG_FAIL;
        }
        else
        {
            return CONFIG_OK;
        }
    }
    else if(strcmp(argv[1], "udp") == 0)
    {
        if(argc != 4)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_udp_client[tcpudp_num] == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }
        memset(data_buf, 0, sizeof(data_buf));
        strcpy(data_buf, argv[3]);
        data_len = sendto(g_sockfd_udp_client[tcpudp_num], data_buf, strlen(data_buf), 0, (struct sockaddr *)&g_udp_remote_addr[tcpudp_num], sin_size);
        if(data_len <= 0)
        {
            dbg_at("udp send error,data_len = %d", data_len);
            return CONFIG_FAIL;
        }
        else
        {
            return CONFIG_OK;
        }
    }
    else if(strcmp(argv[1], "tcpserver") == 0)
    {
        if(argc != 3)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        if(g_sockfd_tcp_server == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }
        memset(data_buf, 0, sizeof(data_buf));
        strcpy(data_buf, argv[2]);
        data_len = send(g_connfd_tcp_server, data_buf, strlen(data_buf), 0);
        if(data_len != strlen(argv[2]))
        {
            if(data_len == -1)
            {
                if(errno == ENOTCONN || errno == ECONNABORTED)
                {
                    dbg_at("connection closed");
                    return CONFIG_FAIL;
                }
            }
            dbg_at("tcp send error, please retry");
            return CONFIG_FAIL;
        }
        else
        {
            return CONFIG_OK;
        }
    }
    else
    {
        dbg_at("unsupport cmd cfg: %s\r\n", argv[1]);
        return CONFIG_FAIL;
    }
}

static int lega_at_net_recv(int argc, char **argv)
{
    char data_buf[BUFFER_LEN] = {0};
    int default_timeout = 100;//ms
    struct timeval timeout;
    int data_len = 0;
    socklen_t sin_size = sizeof(data_buf);
    int tcpudp_num = 0;

    if(strcmp(argv[1], "tcp") == 0)
    {
        if(argc != 3 && argc != 4)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_tcp_client[tcpudp_num] == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }

        if(argc == 4)
        {
            default_timeout = convert_str_to_int(argv[3]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout)%1000)*1000;
        if(setsockopt(g_sockfd_tcp_client[tcpudp_num], SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }

        data_len = recv(g_sockfd_tcp_client[tcpudp_num], data_buf, MAX_DATA_LEN, 0);
        if(data_len > 0)
        {
            at_rspdata("rx tcp:%s", data_buf);
        }
        else if(data_len < 0)
        {
            dbg_at("no data");
        }

    }
    else if(strcmp(argv[1], "udp") == 0)
    {
        if(argc != 3 && argc != 4)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_udp_client[tcpudp_num] == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }

        if(argc == 4)
        {
            default_timeout = convert_str_to_int(argv[3]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout)%1000)*1000;
        if(setsockopt(g_sockfd_udp_client[tcpudp_num], SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }

        data_len = recvfrom(g_sockfd_udp_client[tcpudp_num], data_buf, sizeof(data_buf), 0, (struct sockaddr *)&g_udp_remote_addr[tcpudp_num], &sin_size);
        if(data_len > 0)
            at_rspdata("rx udp:%s", data_buf);
        else if(data_len == 0)
            dbg_at("data_len = 0");
        else
        {
            if(errno == ENOTCONN || errno == ECONNABORTED)
            {
                dbg_at("connection closed");
                return CONFIG_FAIL;
            }
        }
    }
    else if(strcmp(argv[1], "tcpserver") == 0)
    {
        if(argc != 2 && argc != 3)
        {
            dbg_at("para num error: %d", argc);
            return PARAM_MISS;
        }
        if(g_sockfd_tcp_server == -1)
        {
            dbg_at("socket not set");
            return CONFIG_FAIL;
        }

        if(argc == 3)
        {
            default_timeout = convert_str_to_int(argv[2]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout)%1000)*1000;
        if(setsockopt(g_connfd_tcp_server, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }

        data_len = recv(g_connfd_tcp_server, data_buf, sizeof(data_buf), 0);
        if(data_len > 0)
            at_rspdata("rx server:%s", data_buf);
        else if(data_len == 0)
            dbg_at("no data");
        else
        {
            if(errno == ENOTCONN || errno == ECONNABORTED)
            {
                dbg_at("connection closed");
                return CONFIG_FAIL;
            }
            else if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                dbg_at("no data");
            }
        }
    }
    else
    {
        dbg_at("net_recv para error");
        return PARAM_RANGE;
    }

    return CONFIG_OK;
}

static int lega_at_net_close(int argc, char **argv)
{
    int tcpudp_num = 0;
    if(argc != 2 && argc != 3)
    {
        dbg_at("para num error: %d", argc);
        return PARAM_MISS;
    }
    if(argc == 3)
    {
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d", tcpudp_num);
            return PARAM_RANGE;
        }
    }

    if(strcmp(argv[1], "tcp") == 0)
    {
        if(g_sockfd_tcp_client[tcpudp_num] == -1)
        {
            dbg_at("tcp %d already closed", tcpudp_num);
            return CONFIG_OK;
        }
        close(g_sockfd_tcp_client[tcpudp_num]);
        g_sockfd_tcp_client[tcpudp_num] = -1;
    }
    else if(strcmp(argv[1], "udp") == 0)
    {
        if(g_sockfd_udp_client[tcpudp_num] == -1)
        {
            dbg_at("udp %d already closed", tcpudp_num);
            return CONFIG_OK;
        }
        close(g_sockfd_udp_client[tcpudp_num]);
        g_sockfd_udp_client[tcpudp_num] = -1;
    }
    else if(strcmp(argv[1], "tcpserver") == 0)
    {
        if(Net_Server_Task_Handler != NULL)
        {
            lega_rtos_delete_thread(&Net_Server_Task_Handler);
            Net_Server_Task_Handler = NULL;
        }
        if(g_connfd_tcp_server != -1)
        {
            close(g_connfd_tcp_server);
            g_connfd_tcp_server = -1;
        }
        if(g_sockfd_tcp_server != -1)
        {
            close(g_sockfd_tcp_server);
            g_sockfd_tcp_server = -1;
        }
    }
    else if(strcmp(argv[1], "tcpserver_client") == 0)
    {
        if(g_connfd_tcp_server != -1)
        {
            close(g_connfd_tcp_server);
            g_connfd_tcp_server = -1;
        }
    }
    else
    {
        dbg_at("net_close para error");
        return PARAM_RANGE;
    }

    return CONFIG_OK;
}

static int lega_at_net_gethostbyname(int argc, char **argv)
{
    struct hostent *hostent;
    int i;
    if(argc != 2)
    {
        dbg_at("para num error\r\n");
        return PARAM_MISS;
    }
    hostent = gethostbyname(argv[1]);
    if(!hostent)
    {
        dbg_at("gethostbyname error\r\n");
        return CONFIG_FAIL;
    }

    for(i=0; hostent->h_addr_list[i]; i++)
    {
        at_rspdata("ip[%d]=%s\r\n",i,ipaddr_ntoa((ip_addr_t *)hostent->h_addr_list[i]));
    }

    return CONFIG_OK;
}
static void net_server_task(lega_thread_arg_t arg)
{
    int timestamp = 10;
    int tmp_connfd = -1;
    struct sockaddr_in c_addr;
    socklen_t sin_size = 0;
    sin_size = sizeof(struct sockaddr_in);
    while(1)
    {
        tmp_connfd = -1;
        if((tmp_connfd = accept(g_sockfd_tcp_server, (struct sockaddr *)&c_addr, &sin_size)) >= 0 )
        {
            if(g_connfd_tcp_server != tmp_connfd)
            {
                close(g_connfd_tcp_server);
                g_connfd_tcp_server = tmp_connfd;
            }
            dbg_at("client connected\r\n");
        }
        lega_rtos_delay_milliseconds(timestamp);
    }
}

static int lega_at_net_serv(int argc, char **argv)
{
    unsigned int port;
    int default_timeout = 10;//ms
    struct timeval timeout;
    int opt = 1;
    if((at_user_info.sta_connected == 0) && (at_user_info.sap_opend == 0))
    {
        dbg_at("connect wifi first\r\n");
        return CONFIG_FAIL;
    }
    if(argc != 3 && argc != 4)
    {
        dbg_at("para num error\r\n");
        dbg_at("net_server tcp [port] [timeout(s)] \r\n");
        return PARAM_MISS;
    }
    if(strcmp(argv[1], "tcp"))
    {
        dbg_at("only support tcp\r\n");
        return PARAM_RANGE;
    }
    port = convert_str_to_int(argv[2]);
    if(port <= 0 || port >= 65535)
    {
        dbg_at("port num error:%s\r\n", argv[2]);
        return PARAM_RANGE;
    }
    struct sockaddr_in s_addr;

    /*setting server pram*/
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;//allow any local address
    s_addr.sin_port = htons(port);

    /*create socket*/
    if((g_sockfd_tcp_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        dbg_at("server socket create error\r\n");
        g_sockfd_tcp_server = -1;
        return CONFIG_FAIL;
    }else{
        dbg_at("server socket create success\r\n");
    }

    if(setsockopt(g_sockfd_tcp_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        dbg_at("setsockopt reuseaddr error\r\n");
    }
    /*bind socket to net addr*/
    if(bind(g_sockfd_tcp_server, (struct sockaddr *)&s_addr, sizeof(struct sockaddr)) < 0)
    {
        dbg_at("server bind error\r\n");
        close(g_sockfd_tcp_server);
        g_sockfd_tcp_server = -1;
        return CONFIG_FAIL;
    }else{
        dbg_at("server bind success\r\n");
    }
    /*listening*/
    if(listen(g_sockfd_tcp_server, MAX_SERVER_CLIENT_NUM) < 0)
    {
        dbg_at("listen error,errno = %d\r\n", errno);
        close(g_sockfd_tcp_server);
        g_sockfd_tcp_server = -1;
        return CONFIG_FAIL;
    }
    dbg_at("server listen success\r\n");
    dbg_at("waitting for connecting...\r\n");
    if(argc == 4)
    {
        default_timeout = convert_str_to_int(argv[3]);
        if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
        {
            dbg_at("set timeout err");
            return PARAM_RANGE;
        }
        if (default_timeout < 1)
            default_timeout = 1;
    }
    timeout.tv_sec = ((long)default_timeout)/1000;
    timeout.tv_usec = (((long)default_timeout)%1000)*1000;
    if(setsockopt(g_sockfd_tcp_server, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
    {
        dbg_at("setsockopt errno: %d", errno);
    }

    // if(setsockopt(g_sockfd_tcp_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    // {
    //     dbg_at("setsockopt reuseaddr error\r\n");
    // }
    /*accept*/
    lega_rtos_create_thread(&Net_Server_Task_Handler, NET_SERVER_TASK_PRIORITY, NET_SERVER_TASK_NAME, net_server_task, NET_SERVER_STACK_SIZE, 0);

    return CONFIG_OK;
}
static void net_uartAT_callback_handler(char ch)
{
    if(lega_rtos_push_to_queue(&net_uart_buf_queue[lega_at_uart.port], &ch, LEGA_NEVER_TIMEOUT))
        dbg_at("queue full\r\n");
}

#ifdef CFG_PLF_DUET
static int32_t net_uart_init(duet_uart_dev_t *uart)
#else
static int32_t net_uart_init(lega_uart_dev_t *uart)
#endif
{
    int32_t ret = 0;

    //change callback, net uart buf big than cmd.
    uart->priv = (void *)(net_uartAT_callback_handler);
#ifdef CFG_PLF_DUET
    ret = duet_uart_init((duet_uart_dev_t *)uart);
#else
    ret = lega_uart_init((lega_uart_dev_t *)uart);
#endif
    if(!ret)
    {
        ret = lega_rtos_init_queue(&net_uart_buf_queue[uart->port], net_uart_buf_queue_name[uart->port], sizeof(char), NET_UART_BUF_QUEUE_BYTES);
    }
    return ret;
}
/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */

#ifdef CFG_PLF_DUET
static int32_t net_uart_send(duet_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
#else
static int32_t net_uart_send(lega_uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
#endif

{
    if(NULL == uart){
        return -1;
    }
#ifdef CFG_PLF_DUET
    return duet_uart_send((duet_uart_dev_t *)uart, data, size, timeout);
#else
    return lega_uart_send((lega_uart_dev_t *)uart, data, size, timeout);
#endif
}
/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */

#ifdef CFG_PLF_DUET
static int32_t net_uart_recv(duet_uart_dev_t *uart, void *data, uint32_t expect_size,
                  uint32_t *recv_size, uint32_t timeout)
#else
static int32_t net_uart_recv(lega_uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
#endif

{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if((uart == NULL) || (data == NULL))
    {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = lega_rtos_pop_from_queue(&net_uart_buf_queue[uart->port], &pdata[i], timeout);

        if(!ret)
        {
            rx_count++;
        }
        else
        {
            if(recv_size)
            {
                *recv_size = rx_count;
                return ret;
            }
        }
        if(pdata[i] == '\0')
        {
            if(i == 3)
            {
                if((pdata[i-1] == '+')&&(pdata[i-2] == '+')&&(pdata[i-3] == '+'))
                {
                    if(recv_size)
                    {
                        *recv_size = rx_count;
                    }
                    return (int32_t)rx_count;
                }
            }
            break;
        }
    }
    if(recv_size)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */

#ifdef CFG_PLF_DUET
static int32_t net_uart_finalize(duet_uart_dev_t *uart)
#else
static int32_t net_uart_finalize(lega_uart_dev_t *uart)
#endif

{
    int32_t ret;

    if(NULL == uart){
        return -1;
    }

    ret = lega_rtos_deinit_queue(&net_uart_buf_queue[uart->port]);
    if(!ret)
    {
#ifdef CFG_PLF_DUET
        return duet_uart_finalize((duet_uart_dev_t *)uart);
#else
        return lega_uart_finalize((lega_uart_dev_t *)uart);
#endif
    }
    else
    {
        return ret;
    }
}

static int lega_at_net_senddata(int argc, char **argv)
{
    int ret = 0;
    char buf[BUFFER_LEN] = {0};
    char state_buf[35] = {0};
    uint32_t recv_size = 0;
    int data_len = -1;
    int pack_num = 1;
    int send_num = 1;
    int err_flag = 0;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int tcpudp_num = -1;
    if(argc > 4 || argc < 3)
    {
        dbg_at("para num error: %d\r\n", argc);
        return PARAM_MISS;
    }
    if(!strcmp(argv[1], "tcp"))
    {
        if(argc != 4)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        data_len = convert_str_to_int(argv[3]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d\r\n", tcpudp_num);
            return PARAM_RANGE;
        }
        if(data_len <= 0 || data_len == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data_len error\r\n");
            return PARAM_RANGE;
        }
        if(g_sockfd_tcp_client[tcpudp_num] < 0)
        {
            dbg_at("tcp %d not open\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        if(data_len > MAX_DATA_LEN)
        {
            pack_num = data_len/MAX_DATA_LEN + 1;
            dbg_at("send %d byte data one time, and when read >> , send next\r\n", (int)MAX_DATA_LEN);
        }
        dbg_at(">");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(send_num <= pack_num)
        {
            memset(buf, 0, sizeof(buf));
            ret = net_uart_recv(&lega_at_uart, buf, ((data_len<MAX_DATA_LEN)? data_len: MAX_DATA_LEN), &recv_size, HAL_WAIT_FOREVER);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            data_len = send(g_sockfd_tcp_client[tcpudp_num], buf, /*strlen(buf)*/recv_size, 0);
            if(data_len <= 0)
            {
                err_flag = 1;
                goto process_end;
            }
            memset(state_buf, 0, sizeof(state_buf));
            sprintf(state_buf, "sendnum: %d, length: %d >>  \r\n", send_num, data_len);
            send_num++;
            net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
            if(ret > 0)
            {
                goto process_end;
            }
        }
        memset(state_buf, 0, sizeof(state_buf));
        strcpy(state_buf, "send over !\r\n");
        net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
    }
    else if(!strcmp(argv[1], "udp"))
    {
        if(argc != 4)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        data_len = convert_str_to_int(argv[3]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d\r\n", tcpudp_num);
            return PARAM_RANGE;
        }
        if(data_len <= 0 || data_len == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data_len error\r\n");
            return PARAM_RANGE;
        }
        if(g_sockfd_udp_client[tcpudp_num] < 0)
        {
            dbg_at("udp %d not open\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        if(data_len > MAX_DATA_LEN)
        {
            pack_num = data_len/MAX_DATA_LEN + 1;
            dbg_at("send %d byte data one time, and when read >> , send next\r\n", (int)MAX_DATA_LEN);
        }
        dbg_at(">");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(send_num <= pack_num)
        {
            memset(buf, 0, sizeof(buf));
            ret = net_uart_recv(&lega_at_uart, buf, ((data_len<MAX_DATA_LEN)? data_len: MAX_DATA_LEN), &recv_size, HAL_WAIT_FOREVER);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            data_len = sendto(g_sockfd_udp_client[tcpudp_num], buf, /*strlen(buf)*/recv_size, 0, (struct sockaddr *)&g_udp_remote_addr[tcpudp_num], sin_size);
            if(data_len <= 0)
            {
                err_flag = 1;
                goto process_end;
            }
            memset(state_buf, 0, sizeof(state_buf));
            sprintf(state_buf, "sendnum: %d, length: %d >>  \r\n", send_num, data_len);
            send_num++;
            net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
            if(ret > 2)
            {
                goto process_end;
            }
        }
        memset(state_buf, 0, sizeof(state_buf));
        strcpy(state_buf, "send over !\r\n");
        net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
    }
    else if(!strcmp(argv[1], "tcpserver"))
    {
        if(argc != 3)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        data_len = convert_str_to_int(argv[2]);
        if(data_len <= 0 || data_len == LEGA_STR_TO_INT_ERR)
        {
            dbg_at("data_len error\r\n");
            return PARAM_RANGE;
        }
        if(argc != 3)
        {
            dbg_at("para num error:%d\r\n", argc);
            return PARAM_MISS;
        }
        if(g_sockfd_tcp_server < 0 || g_connfd_tcp_server < 0)
        {
            dbg_at("tcp %d not open\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        if(data_len > MAX_DATA_LEN)
        {
            pack_num = data_len/MAX_DATA_LEN + 1;
            dbg_at("send %d byte data one time, and when read >> , send next\r\n", (int)MAX_DATA_LEN);
        }
        dbg_at(">");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(send_num <= pack_num)
        {
            memset(buf, 0, sizeof(buf));
            ret = net_uart_recv(&lega_at_uart, buf, ((data_len<MAX_DATA_LEN)? data_len: MAX_DATA_LEN), &recv_size, HAL_WAIT_FOREVER);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            data_len = send(g_connfd_tcp_server, buf, /*strlen(buf)*/recv_size, 0);
            if(data_len <= 0)
            {
                err_flag = 1;
                goto process_end;
            }
            memset(state_buf, 0, sizeof(state_buf));
            sprintf(state_buf, "sendnum: %d, length: %d >>  \r\n", send_num, data_len);
            send_num++;
            net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
            if(ret > 0)
            {
                goto process_end;
            }
        }
        memset(state_buf, 0, sizeof(state_buf));
        strcpy(state_buf, "send over !\r\n");
        net_uart_send(&lega_at_uart, state_buf, strlen(state_buf), HAL_WAIT_FOREVER);
    }
    else
    {
        dbg_at("para error\r\n");
        return PARAM_RANGE;
    }
process_end:
    net_uart_finalize(&lega_at_uart);
#ifdef CFG_PLF_DUET
    g_printf_uart = DUET_UART_NUM;
#else
    g_printf_uart = LEGA_UART_NUM;
#endif
    at_uart_init();

    if(err_flag == 1)
        return CONFIG_FAIL;
    else
        return CONFIG_OK;
}

static int lega_at_net_trandata(int argc, char **argv)
{
    int ret = 0;
    int data_len = -1;
    char buf[BUFFER_LEN] = {0};
    int default_timeout = 1;//ms
    struct timeval timeout;
    uint32_t recv_size = 0;
    int err_flag = 0;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int tcpudp_num = -1;
    if(argc > 4 || argc < 2)
    {
        dbg_at("para num error: %d\r\n", argc);
        return PARAM_MISS;
    }
    if(!strcmp(argv[1], "tcp"))
    {
        if(argc < 3)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d\r\n", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_tcp_client[tcpudp_num] < 0)
        {
            dbg_at("tcp %d not open\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        if(argc == 4)
        {
            default_timeout = convert_str_to_int(argv[3]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
            else if(default_timeout > 100)
                default_timeout = 100;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout)%1000)*1000;
        if(setsockopt(g_sockfd_tcp_client[tcpudp_num], SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }
        at_rspinfor(">>");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(1)
        {
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            ret = net_uart_recv(&lega_at_uart, buf, MAX_DATA_LEN, &recv_size, default_timeout);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            else
            {
                if(recv_size > 0)
                {
                    data_len = send(g_sockfd_tcp_client[tcpudp_num], buf, recv_size, 0);
                    if(data_len < 0)
                    {
                        err_flag = 1;
                        goto process_end;
                    }
                    if(ret > 2)
                        goto process_end;
                    net_uart_send(&lega_at_uart, ">>\r\n", strlen(">>\r\n"), default_timeout);
                }
            }
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            data_len = recv(g_sockfd_tcp_client[tcpudp_num], buf, MAX_DATA_LEN, 0);
            if(data_len > 0)
            {
                net_uart_send(&lega_at_uart, buf, /*strlen(buf)*/ data_len, default_timeout);
            }
            sys_msleep(1);
        }
    }
    else if(!strcmp(argv[1], "udp"))
    {
        if(argc < 3)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        tcpudp_num = convert_str_to_int(argv[2]);
        if(tcpudp_num < 0 || tcpudp_num >= 4)
        {
            dbg_at("tcpudp_num error: %d\r\n", tcpudp_num);
            return PARAM_RANGE;
        }
        if(g_sockfd_udp_client[tcpudp_num] < 0)
        {
            dbg_at("tcp %d not open\r\n", tcpudp_num);
            return CONFIG_FAIL;
        }
        if(argc == 4)
        {
            default_timeout = convert_str_to_int(argv[3]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
            else if(default_timeout > 100)
                default_timeout = 100;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout) - (((long)default_timeout)/1000 * 1000)) * 1000;
        if(setsockopt(g_sockfd_udp_client[tcpudp_num], SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }
        at_rspinfor(">>");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(1)
        {
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            recv_size = 0;
            ret = net_uart_recv(&lega_at_uart, buf, MAX_DATA_LEN, &recv_size, default_timeout);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            else
            {
                if(recv_size > 0)
                {
                    data_len = sendto(g_sockfd_udp_client[tcpudp_num], buf, recv_size, 0, (struct sockaddr *)&g_udp_remote_addr[tcpudp_num], sin_size);
                    if(data_len <= 0)
                    {
                        err_flag = 1;
                        goto process_end;
                    }
                    if(ret > 2)
                        goto process_end;
                    net_uart_send(&lega_at_uart, ">>\r\n", strlen(">>\r\n"), default_timeout);
                }
            }
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            data_len = recvfrom(g_sockfd_udp_client[tcpudp_num], buf, MAX_DATA_LEN, 0, (struct sockaddr *)&g_udp_remote_addr[tcpudp_num], &sin_size);
            if(data_len > 0)
            {
                net_uart_send(&lega_at_uart, buf, /*strlen(buf)*/ data_len, default_timeout);
            }
            sys_msleep(1);
        }
    }
    else if(!strcmp(argv[1], "tcpserver"))
    {
        if(argc < 2)
        {
            dbg_at("para num error: %d\r\n", argc);
            return PARAM_MISS;
        }
        if(argc == 3)
        {
            default_timeout = convert_str_to_int(argv[2]);
            if(default_timeout == LEGA_STR_TO_INT_ERR || default_timeout <= 0)
            {
                dbg_at("set timeout err");
                return PARAM_RANGE;
            }
            if (default_timeout < 1)
                default_timeout = 1;
            else if(default_timeout > 100)
                default_timeout = 100;
        }
        timeout.tv_sec = ((long)default_timeout)/1000;
        timeout.tv_usec = (((long)default_timeout) - (((long)default_timeout)/1000 * 1000)) * 1000;
        if(setsockopt(g_connfd_tcp_server, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0)
        {
            dbg_at("setsockopt errno: %d", errno);
        }
        at_rspinfor(">>");
        delay(5000);
#ifdef CFG_PLF_DUET
        duet_uart_finalize(&lega_at_uart);
#else
        lega_uart_finalize(&lega_at_uart);
#endif
        net_uart_init(&lega_at_uart);
        while(1)
        {
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            recv_size = 0;
            ret = net_uart_recv(&lega_at_uart, buf, MAX_DATA_LEN, &recv_size, default_timeout);
            if(ret < 0)
            {
                err_flag = 1;
                goto process_end;
            }
            else
            {
                if(recv_size > 0)
                {
                    data_len = send(g_connfd_tcp_server, buf, recv_size, 0);
                    if(data_len < 0)
                    {
                        err_flag = 1;
                        goto process_end;
                    }
                    if(ret > 2)
                        goto process_end;
                    net_uart_send(&lega_at_uart, ">>\r\n", strlen(">>\r\n"), default_timeout);
                }
            }
            memset(buf, 0, sizeof(buf));
            data_len = 0;
            data_len = recv(g_connfd_tcp_server, buf, MAX_DATA_LEN, 0);
            if(data_len > 0)
            {
                net_uart_send(&lega_at_uart, buf, data_len, default_timeout);
            }
            sys_msleep(1);
        }
    }
    else
    {
        dbg_at("para error\r\n");
        return PARAM_RANGE;
    }
process_end:
    net_uart_finalize(&lega_at_uart);
#ifdef CFG_PLF_DUET
    g_printf_uart = DUET_UART_NUM;
#else
    g_printf_uart = LEGA_UART_NUM;
#endif
    at_uart_init();
    if(err_flag == 1)
        return CONFIG_FAIL;
    else
        return CONFIG_OK;
}

/*
 ************************************************************
 *              TCP/IP  AT CMD END
 *
 ************************************************************
 */

cmd_entry comm_net_status = {
    .name = "net_status",
    .help = "cmd format:   net_status\r\n\
cmd function: get current tcp or udp client info and tcpserver status\r\n",
    .function = lega_at_net_status,
};
cmd_entry comm_net_conn = {
    .name = "net_conn",
    .help = "cmd format:   net_conn {tcp|udp} <id> <ipaddress> <port>\r\n\
cmd funcction:\r\n\
tcp|udp   : choose one connect mode, tcp or udp\r\n\
id        : 0-3 refers to total four connection(tcp or udp)\r\n\
ipaddress : ip address\r\n\
port      : port\r\n\
(e.g. net_conn tcp 3 192.168.0.101 8888)\r\n",
    .function = lega_at_net_conn,
};
cmd_entry comm_net_send = {
    .name = "net_send",
    .help = "cmd format:   net_send {tcp|udp|tcpserver} <id> <data>\r\n\
cmd funcction:\r\n\
tcp|udp|tcpserver : choose one connect mode, tcp or udp or tcpserver\r\n\
id                : 0-3 refers to total four connection(tcp or udp), not input when tcpserver\r\n\
data              : your data. data length must less than 112\r\n",
    .function = lega_at_net_send,
};
cmd_entry comm_net_recv = {
    .name = "net_recv",
    .help = "cmd format:   net_recv {tcp|udp|tcpserver} <id> [timeout]\r\n\
cmd funcction:\r\n\
tcp|udp|tcpserver : choose one connect mode, tcp or udp or tcpserver\r\n\
id                : 0-3 refers to total four connection(tcp or udp), not input when tcpserver\r\n\
timeout           : timeout (unit ms)\r\n",
    .function = lega_at_net_recv,
};
cmd_entry comm_net_senddata = {
    .name = "net_senddata",
    .help = "cmd format:   net_senddata {tcp|udp|tcpserver} [id] [length]\r\n\
cmd function:\r\n\
tcp|udp|tcpserver : choose one connect mode, tcp or udp or tcpserver\r\n\
id                : 0-3 refers to total four connection(tcp or udp), not input when tcpserver\r\n\
length            : data length you will send\r\n\
when read >, you can input your data\r\n",
    .function = lega_at_net_senddata,
};
cmd_entry comm_net_trandata = {
    .name = "net_trandata",
    .help = "cmd format:   net_trandata {tcp|udp|tcpserver} [id] [timeout]\r\n\
cmd function:\r\n\
tcp|udp|tcpserver : choose one connect mode, tcp or udp or tcpserver\r\n\
id                : 0-3 refers to total four connection(tcp or udp), not input when tcpserver\r\n\
timeout           : timeout (unit ms)\r\n\
when read >, you can input your data\r\n",
    .function = lega_at_net_trandata,
};
cmd_entry comm_net_server = {
.name = "net_server",
.help = "cmd format:   net_server tcp <port> [timeout]\r\n\
cmd funcction:\r\n\
tcp     : choose one connect mode, support tcp only\r\n\
port    : port\r\n\
timeout : timeout (unit ms)\r\n\
(e.g. net_server tcp 8888)\r\n",
.function = lega_at_net_serv,
};
cmd_entry comm_net_close = {
    .name = "net_close",
    .help = "cmd format:   net_close {tcp|udp|tcpserver} [id]\r\n\
cmd funcction:\r\n\
tcp|udp|tcpserver : choose one connect mode, tcp or udp or tcpserver\r\n\
id                : 0-3 refers to total four connection(tcp or udp), not input when tcpserver\r\n",
    .function = lega_at_net_close,
};
cmd_entry comm_net_gethost = {
    .name = "net_gethost",
    .help = "cmd format:   net_gethost <url>\r\n\
cmd funcction:\r\n\
url : a web site\r\n",
    .function = lega_at_net_gethostbyname,
};

void lega_at_net_cmd_register(void)
{
    lega_at_cmd_register(&comm_net_status);
    lega_at_cmd_register(&comm_net_conn);
    lega_at_cmd_register(&comm_net_send);
    lega_at_cmd_register(&comm_net_recv);
    lega_at_cmd_register(&comm_net_trandata);
    lega_at_cmd_register(&comm_net_server);
    lega_at_cmd_register(&comm_net_close);
    lega_at_cmd_register(&comm_net_gethost);
}
#endif

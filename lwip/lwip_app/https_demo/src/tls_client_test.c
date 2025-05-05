/*
 *  SSL client demonstration program
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "mbedtls/config.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time            time
#define mbedtls_time_t          time_t
#define mbedtls_fprintf         fprintf
#define mbedtls_printf          printf
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
#endif /* MBEDTLS_PLATFORM_C */

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "mbedtls_net_sock.h"

#include <string.h>

#define SERVER_PORT "443"

#define SERVER_NAME "www.baidu.com"
// #define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"
#define GET_REQUEST "GET / HTTP/1.0\r\nHost: www.baidu.com\r\n\
Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n\r\n"
// #define SERVER_NAME "storage.jdtiot.com"
// #define GET_REQUEST "GET /devh5/duet_demo.ota_1666601655933.bin HTTP/1.0\r\nHost: %s\r\n\r\n"

#define DEBUG_LEVEL 1

static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);

    mbedtls_fprintf( (FILE *) ctx, "%s:%04d: %s", file, line, str );
    fflush(  (FILE *) ctx  );
}

#define OTA_HTTPS_USED  1
#define OTA_NO_WRITE_FLASH
static int ota_recv_process(char *p_buf, uint32_t buf_len);
static int ota_download_bin(void);

#define tls_ssl_ctx mbedtls_ssl_context
static tls_ssl_ctx local_tls_ssl_ctx;
static void set_ssl_ctx(tls_ssl_ctx *ssl) {
    memcpy(&local_tls_ssl_ctx, ssl, sizeof(tls_ssl_ctx));
}

#include "lwip/arch.h"
static int
mbedtls_entropy_dummy_rng_func(void *ctx, unsigned char *buffer, size_t len)
{
  static size_t ctr;
  size_t i;
  LWIP_UNUSED_ARG(ctx);
  for (i = 0; i < len; i++) {
    buffer[i] = (unsigned char)++ctr;
  }
  return 0;
}

void tls_ota_task(void *args) {

    int ret = 1, len;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    const char *pers = "ssl_client1";

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_printf( "\n  . Seeding the random number generator..." );
    fflush( stdout );

    mbedtls_entropy_init( &entropy );
    // mbedtls_entropy_dummy_rng_func mbedtls_entropy_func
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_dummy_rng_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 0. Initialize certificates
     */
    mbedtls_printf( "  . Loading the CA root certificate ..." );
    fflush( stdout );

    ret = mbedtls_x509_crt_parse( &cacert, (const unsigned char *) mbedtls_test_cas_pem,
                          mbedtls_test_cas_pem_len );
    if( ret < 0 )
    {
        mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
        goto exit;
    }

    mbedtls_printf( " ok (%d skipped)\n", ret );

    /*
     * 1. Start the connection
     */
    mbedtls_printf( "  . Connecting to tcp/%s/%s...", SERVER_NAME, SERVER_PORT );
    fflush( stdout );

    if( ( ret = mbedtls_net_connect( &server_fd, SERVER_NAME,
                                         SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 2. Setup stuff
     */
    mbedtls_printf( "  . Setting up the SSL/TLS structure..." );
    fflush( stdout );

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, SERVER_NAME ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    /*
     * 4. Handshake
     */
    mbedtls_printf( "  . Performing the SSL/TLS handshake..." );
    fflush( stdout );

    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
            goto exit;
        }
    }

    mbedtls_printf( " ok\n" );

    /*
     * 5. Verify the server certificate
     */
    mbedtls_printf( "  . Verifying peer X.509 certificate..." );

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 )
    {
        char vrfy_buf[512];

        mbedtls_printf( " failed\n" );

        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

        mbedtls_printf( "%s\n", vrfy_buf );
    }
    else
        mbedtls_printf( " ok\n" );

    /*
     * 3. Write the GET request
     */
    mbedtls_printf( "  > Write to server:" );
    fflush( stdout );

    len = sprintf( (char *) buf, GET_REQUEST, SERVER_NAME);

    while( ( ret = mbedtls_ssl_write( &ssl, buf, len ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
            goto exit;
        }
    }

    len = ret;
    mbedtls_printf( " %d bytes written\n\n%s", len, (char *) buf );

    /*
     * 7. Read the HTTP response
     */
    mbedtls_printf( "  < Read from server:" );
    fflush( stdout );

    uint32_t recv_sum_size = 0;
    do
    {
        len = sizeof( buf ) - 1;
        memset( buf, 0, sizeof( buf ) );
        ret = mbedtls_ssl_read( &ssl, buf, len );

        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
            continue;

        if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY ) {
            mbedtls_printf( "unknown\n  ! mbedtls_ssl_read closed by peer \r\n" );
            printf("recv_sum_size = %lu\r\n", recv_sum_size);
            break;
        } 

        if( ret < 0 )
        {
            mbedtls_printf( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret );
            break;
        }

        if( ret == 0 )
        {
            mbedtls_printf( "\n\nEOF\n\n" );
            break;
        }

        len = ret;
        // mbedtls_printf( " %d bytes read\n\n%s", len, (char *) buf );
        recv_sum_size += len;
        mbedtls_printf( " %d bytes read\n", len);
    }
    while( 1 );

    set_ssl_ctx(&ssl);
    // ota_download_bin();

    mbedtls_ssl_close_notify( &ssl );

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

#ifdef MBEDTLS_ERROR_C
    if( exit_code != MBEDTLS_EXIT_SUCCESS )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif

    mbedtls_net_free( &server_fd );

    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

#if defined(_WIN32)
    mbedtls_printf( "  + Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    // return( exit_code );
    mbedtls_printf( "%s Task Closing... exit_code: %d\n", __FUNCTION__ , exit_code);
    vTaskDelete(NULL);
}

static int ota_recv_process(char *p_buf, uint32_t buf_len) {

    int received_len = 0;
#if (OTA_HTTPS_USED == 0)
    received_len = recv(fd, p_buf, buf_len, 0);
    printf("download from server:%d\r\n", received_len);

#else
    int ret;
    ret = mbedtls_ssl_read( &local_tls_ssl_ctx, (unsigned char *)p_buf, buf_len );
    do
    {
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
            mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret < 0)
        {
            mbedtls_printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret == 0)
        {
            mbedtls_printf("\n\nEOF\n\n");
            break;
        }

        received_len = ret;
        mbedtls_printf(" %d bytes read\n\n%s", received_len, (char *)p_buf);
    } while (0);

#endif
    return received_len;
}

#include "lega_rtos_api.h"
static int ota_download_bin(void) {
    char *server_reply;
    int total_len = 0;
    unsigned long file_len = 0;
    int received_len = 0;
    int temp = 10;

    server_reply = (char *)lega_rtos_malloc(600);
    if (server_reply  == NULL)
    {
        printf("malloc memory failed \r\n");
        return -1;
    }
    memset(server_reply,'\0',600);

    received_len = ota_recv_process(server_reply, 600);

    if( received_len < 0 )
    {
        printf("Recv Failed\r\n");
        lega_rtos_free(server_reply);
        server_reply = NULL;
        return -1;
    }
    if (strstr(server_reply, "HTTP/1.1 200 OK") == NULL)
    {
        printf("Response not 200 OK\n");
        lega_rtos_free(server_reply);
        server_reply = NULL;
        return -1;
    }
    else
    {
        printf("get  receive message:%s \r\n", server_reply);
        char *p1, *p2, *ptemp;
        char file_lens[50];
        int data_cnt = 0;

        //get the OTA bin file length
        if ((p1 = strstr(server_reply, "Content-Length: ")) != NULL && (p2 = strstr(server_reply, "Connection:")) != NULL)
        {
            p1 += strlen("Content-Length: ");
            strncpy(file_lens, p1, p2 - p1);
            file_len = strtol((const char *)file_lens, NULL, 10);
            printf("download file length:%ld\r\n", file_len);
        }
        ptemp = server_reply;
        for(temp = 0;temp < received_len;temp++)
        {
            if (strncmp(ptemp,"WIFI 5501 A0V2",strlen("WIFI 5501 A0V2")) == 0)  //find the header of OTA bin
            {
                data_cnt = received_len - (ptemp - server_reply);//data_cnt is how many datas in first recv
                break;
            }
            ptemp++;
        }
        if(temp == received_len)
        {
            printf("received the ASR OTA bin falied \r\n");
            lega_rtos_free(server_reply);
            server_reply = NULL;
            return -1;
        }

        printf("Firmware upgrade start\r\n");
        #ifndef OTA_NO_WRITE_FLASH
        ret = lega_ota_write(ota_offset, ptemp, data_cnt);
        if(ret != 0 )
        {
            printf("ota write flash Failed\r\n");
            lega_rtos_free(server_reply);
            server_reply = NULL;
            return -1;
        }
        #endif
        total_len += data_cnt;
        printf("data byte size = %d total length = %d\r\n", data_cnt, total_len);
        data_cnt = 0;
        while(1)
        {
            received_len = ota_recv_process(server_reply, 600);
            /* check if the connection is closed */
            if( received_len < 0 )
            {
                printf("Recv Failed\r\n");
                lega_rtos_free(server_reply);
                server_reply = NULL;
                return -1;
            }
            if( total_len >= file_len )
            {
                printf("image has been received!\r\n\r\n\r\n");
                break;
            }
            else
            {
                if((file_len-total_len) >= 600)
                {
                    total_len += received_len;
                    #ifndef OTA_NO_WRITE_FLASH
                    ret = lega_ota_write(ota_offset, server_reply, received_len);
                    if(ret != 0 )
                    {
                        printf("ota write flash Failed\r\n");
                        lega_rtos_free(server_reply);
                        server_reply = NULL;
                        return -1;
                    }
                    #endif
                    printf("data byte size = %d total length = %d\r\n", received_len, total_len);
                }
                else
                {
                    data_cnt = file_len-total_len;          //get the length with the last packet received
                    total_len += data_cnt;
                    #ifndef OTA_NO_WRITE_FLASH
                    ret = lega_ota_write(ota_offset, server_reply, data_cnt);
                    if(ret != 0)
                    {
                        printf("ota write flash Failed\r\n");
                        lega_rtos_free(server_reply);
                        server_reply = NULL;
                        return -1;
                    }
                    #endif
                    printf("data byte size = %d total length = %d\r\n", data_cnt, total_len);
                    memset(server_reply,'\0',received_len);
                    printf("recv end\r\n");
                    break;
                }
                memset(server_reply,'\0',received_len);
            }
        }
    }

    lega_rtos_free(server_reply);
    server_reply = NULL;

    printf("total data byte size:%d\n", total_len);
}
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
#include "FreeRTOS.h"
#include "queue.h"

QueueHandle_t usr_sig_queue = NULL;
uint32_t usr_msg_val = 0;

void usr_init_sig(void)
{
    usr_sig_queue = xQueueCreate(
        /* The number of items the queue can hold. */
        10,
        /* Size of each item is big enough to hold the
        whole structure. */
        sizeof(usr_msg_val));
    if (usr_sig_queue == NULL)
    {
        printf("usr_init_sig create fail\r\n");
    }
}

void usr_snd_sig(uint32_t type) {
    usr_msg_val = type;
    if (usr_sig_queue == NULL) {
        printf("usr_snd_sig fail, handle not exist\r\n");
    }
    /* Send the entire structure to the queue created to hold 10 structures. */
   xQueueSend( /* The handle of the queue. */
               usr_sig_queue,
               /* The address of the xMessage variable.  sizeof( struct AMessage )
               bytes are copied from here into the queue. */
               ( void * ) &usr_msg_val,
               /* Block time of 0 says don't block if the queue is already full.
               Check the value returned by xQueueSend() to know if the message
               was sent to the queue successfully. */
               ( TickType_t ) 0 );
}

void usr_wait_sig_forever(uint32_t type)
{
    printf("enter usr_wait_sig_forever\r\n");
    if (usr_sig_queue == NULL)
    {
        printf("usr_wait_sig_forever fail, handle not exist\r\n");
    }

    /* Receive a message from the created queue to hold complex struct AMessage
      structure.  Block for 10 ticks if a message is not immediately available.
      The value is read into a struct AMessage variable, so after calling
      xQueueReceive() xRxedStructure will hold a copy of xMessage. */
    if (xQueueReceive(usr_sig_queue,
                      &(usr_msg_val),
                      (TickType_t)portMAX_DELAY) == pdPASS)
    {
        /* xRxedStructure now contains a copy of xMessage. */
    }
    else
    {
        goto USR_SIG_EXIT;
    }

    if (type == usr_msg_val)
    {
        printf("usr_wait_sig_forever suc, val=%lu\r\n", usr_msg_val);
    }

USR_SIG_EXIT:
    printf("exit usr_wait_sig_forever\r\n");
}
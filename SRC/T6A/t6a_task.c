/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under 
 * the Apache License, Version 2.0  (the "License"); you may not use this file
 * except in compliance with the License.  
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#if HAVE_CONFIG_H
#  include "config.h"
#endif
#include "intertask_interface.h"
#include "t6a_defs.h"
#include "t6a_messages.h"
#include "common_types.h"
#include "assertions.h"
#include "msc.h"
#include "log.h"
#include "timer.h"

#define T6A_PEER_CONNECT_TIMEOUT_MICRO_SEC  (0)
#define T6A_PEER_CONNECT_TIMEOUT_SEC        (1)

static long                             timer_id = 0;
t6a_fd_cnf_t                            t6a_fd_cnf;

void                                   *t6a_thread (void *args);
static void t6a_exit(void);

//------------------------------------------------------------------------------
void *t6a_thread (void *args)
{
  itti_mark_task_ready (TASK_T6A);
  OAILOG_START_USE ();
  MSC_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    /*
     * Trying to fetch a message from the message queue.
     * * If the queue is empty, this function will block till a
     * * message is sent to the task.
     */
    itti_receive_msg (TASK_T6A, &received_message_p);
    DevAssert (received_message_p );

    switch (ITTI_MSG_ID (received_message_p)) {
    case T6A_REPORT_INFO_REQ:{
        t6a_generate_report_info_req (&received_message_p->ittiMsg.t6a_report_info_req);
      }
      break;
    /*
    case T6A_CONFIG_INFO_ANS:{
        t6a_generate_config_info_ans (&received_message_p->ittiMsg.t6a_config_info_ans);
      }
      break;
    case T6A_CONN_MGMT_REQ:{
        t6a_generate_conn_mgmt_req (&received_message_p->ittiMsg.t6a_conn_mgmt_req);
      }
      break;
    case T6A_CONN_MGMT_ANS:{
        t6a_generate_conn_mgmt_ans (&received_message_p->ittiMsg.t6a_conn_mgmt_ans);
      }
      break;
    case T6A_MO_DATA_REQ:{
        t6a_generate_mo_data_req (&received_message_p->ittiMsg.t6a_mo_data_req);
      }
      break;
    case T6A_MT_DATA_ANS:{
        t6a_generate_mt_data_ans (&received_message_p->ittiMsg.t6a_mt_data_ans);
      }
      break;
    */
    case TIMER_HAS_EXPIRED:{
        /*
         * Trying to connect to peers
         */
        if (t6a_fd_new_peer() != RETURNok) {
          /*
           * On failure, reschedule timer.
           * * Preferred over TIMER_PERIODIC because if t6a_fd_new_peer takes
           * * longer to return than the period, the timer will schedule while
           * * the previous one is active, causing a seg fault.
           */
          OAILOG_ERROR(LOG_T6A, "t6a_fd_new_peer has failed (%s:%d)\n",
                       __FILE__, __LINE__);
          timer_setup(T6A_PEER_CONNECT_TIMEOUT_SEC,
                      T6A_PEER_CONNECT_TIMEOUT_MICRO_SEC, TASK_T6A,
                      INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL, &timer_id);
        }
      }
      break;
    case TERMINATE_MESSAGE:{
        t6a_exit();
        itti_exit_task ();
      }
      break;
    default:{
        OAILOG_DEBUG (LOG_T6A, "Unkwnon message ID %d: %s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }
    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }
  return NULL;
}

//------------------------------------------------------------------------------
int t6a_init (
  const mme_config_t * mme_config_p)
{
  int                                     ret;

  OAILOG_DEBUG (LOG_T6A, "Initializing T6a interface\n");

  memset (&t6a_fd_cnf, 0, sizeof (t6a_fd_cnf_t));

  OAILOG_DEBUG (LOG_T6A, "Default ext path: %s\n", DEFAULT_EXTENSIONS_PATH);

  ret = t6a_fd_init_dict_objs ();
  if (ret) {
    OAILOG_ERROR (LOG_T6A, "An error occurred during t6a_fd_init_dict_objs.\n");
    return ret;
  } else {
    OAILOG_DEBUG (LOG_T6A, "t6a_fd_init_dict_objs done\n");
  }

  if (itti_create_task (TASK_T6A, &t6a_thread, NULL) < 0) {
    OAILOG_ERROR (LOG_T6A, "t6a create task\n");
    return RETURNerror;
  }
  OAILOG_DEBUG (LOG_T6A, "Initializing T6a interface: DONE\n");

  /* Add timer here to send message to connect to peer */
  timer_setup(T6A_PEER_CONNECT_TIMEOUT_SEC, T6A_PEER_CONNECT_TIMEOUT_MICRO_SEC,
              TASK_T6A, INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL, &timer_id);

  return RETURNok;
}

//------------------------------------------------------------------------------
static void t6a_exit(void)
{
  // Nothing to do.
  return NULL;
}

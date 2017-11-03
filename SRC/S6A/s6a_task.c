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
#include "s6a_defs.h"
#include "s6a_messages.h"
#include "common_types.h"
#include "assertions.h"
#include "msc.h"
#include "log.h"
#include "timer.h"

#define S6A_PEER_CONNECT_TIMEOUT_MICRO_SEC  (0)
#define S6A_PEER_CONNECT_TIMEOUT_SEC        (1)

static long                             timer_id = 0;
// KMAC: Comment out. Not used
//struct session_handler                 *ts_sess_hdl;

s6a_fd_cnf_t                            s6a_fd_cnf;

void                                   *s6a_thread (void *args);
static void s6a_exit(void);

//------------------------------------------------------------------------------
void *s6a_thread (void *args)
{
  itti_mark_task_ready (TASK_S6A);
  OAILOG_START_USE ();
  MSC_START_USE ();

  while (1) {
    MessageDef                             *received_message_p = NULL;

    /*
     * Trying to fetch a message from the message queue.
     * * If the queue is empty, this function will block till a
     * * message is sent to the task.
     */
    itti_receive_msg (TASK_S6A, &received_message_p);
    DevAssert (received_message_p );

    switch (ITTI_MSG_ID (received_message_p)) {
    case S6A_UPDATE_LOCATION_REQ:{
        s6a_generate_update_location (&received_message_p->ittiMsg.s6a_update_location_req);
      }
      break;
    case S6A_AUTH_INFO_REQ:{
        s6a_generate_authentication_info_req (&received_message_p->ittiMsg.s6a_auth_info_req);
      }
      break;
    case TIMER_HAS_EXPIRED:{
        /*
         * Trying to connect to peers
         */
        if (s6a_fd_new_peer() != RETURNok) {
          /*
           * On failure, reschedule timer.
           * * Preferred over TIMER_PERIODIC because if s6a_fd_new_peer takes
           * * longer to return than the period, the timer will schedule while
           * * the previous one is active, causing a seg fault.
           */
          OAILOG_ERROR(LOG_S6A, "s6a_fd_new_peer has failed (%s:%d)\n",
                       __FILE__, __LINE__);
          timer_setup(S6A_PEER_CONNECT_TIMEOUT_SEC,
                      S6A_PEER_CONNECT_TIMEOUT_MICRO_SEC, TASK_S6A,
                      INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL, &timer_id);
        }
      }
      break;
    case TERMINATE_MESSAGE:{
        s6a_exit();
        itti_exit_task ();
      }
      break;
    default:{
        OAILOG_DEBUG (LOG_S6A, "Unkwnon message ID %d: %s\n", ITTI_MSG_ID (received_message_p), ITTI_MSG_NAME (received_message_p));
      }
      break;
    }
    itti_free (ITTI_MSG_ORIGIN_ID (received_message_p), received_message_p);
    received_message_p = NULL;
  }
  return NULL;
}

// KMAC - Note: Moved FreeDiameter initialization and shutdown to main MME application.
//        Multiple protocols (S6A, T6A) are using FreeDiameter now so it does not belong under S6A

//------------------------------------------------------------------------------
int s6a_init (
  const mme_config_t * mme_config_p)
{
  int                                     ret;

  OAILOG_DEBUG (LOG_S6A, "Initializing S6a interface\n");

  memset (&s6a_fd_cnf, 0, sizeof (s6a_fd_cnf_t));

  OAILOG_DEBUG (LOG_S6A, "Default ext path: %s\n", DEFAULT_EXTENSIONS_PATH);

  ret = s6a_fd_init_dict_objs ();
  if (ret) {
    OAILOG_ERROR (LOG_S6A, "An error occurred during s6a_fd_init_dict_objs.\n");
    return ret;
  } else {
    OAILOG_DEBUG (LOG_S6A, "s6a_fd_init_dict_objs done\n");
  }

  if (itti_create_task (TASK_S6A, &s6a_thread, NULL) < 0) {
    OAILOG_ERROR (LOG_S6A, "s6a create task\n");
    return RETURNerror;
  }
  OAILOG_DEBUG (LOG_S6A, "Initializing S6a interface: DONE\n");

  /* Add timer here to send message to connect to peer */
  timer_setup(S6A_PEER_CONNECT_TIMEOUT_SEC, S6A_PEER_CONNECT_TIMEOUT_MICRO_SEC,
              TASK_S6A, INSTANCE_DEFAULT, TIMER_ONE_SHOT, NULL, &timer_id);

  return RETURNok;
}

//------------------------------------------------------------------------------
static void s6a_exit(void)
{
  // Nothing to do.
  return NULL;
}

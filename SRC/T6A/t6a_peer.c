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

#include <stdint.h>
#include <unistd.h>

#include "common_types.h"
#include "intertask_interface.h"
#include "t6a_defs.h"
#include "t6a_messages.h"
#include "assertions.h"
#include "dynamic_memory_check.h"
#include "log.h"

#define NB_MAX_TRIES  (8)

extern __pid_t g_pid;

void
t6a_peer_connected_cb (
  struct peer_info *info,
  void *arg)
{
  if (info == NULL) {
    OAILOG_ERROR (LOG_T6A, "Failed to connect to SCEF entity\n");
  } else {
    MessageDef                             *message_p;

    OAILOG_DEBUG (LOG_T6A, "Peer %*s is now connected...\n", (int)info->pi_diamidlen, info->pi_diamid);
    /*
     * Inform S1AP that connection to SCEF is established
     */
    message_p = itti_alloc_new_message (TASK_T6A, ACTIVATE_MESSAGE);
    itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);
  }
}

int
t6a_fd_new_peer (
  void)
{
  int                                     ret = 0;
#if FD_CONF_FILE_NO_CONNECT_PEERS_CONFIGURED
  struct peer_info                        info = {0};
#endif

  if (mme_config_read_lock (&mme_config) ) {
    OAILOG_ERROR (LOG_T6A, "Failed to lock configuration for reading\n");
    return RETURNerror;
  }

  // KMAC: Free Diameter common config struct
  bstring                                 scef_name = bstrcpy(mme_config.fdiam_config.scef_host_name);
  bconchar(scef_name, '.');
  bconcat (scef_name, mme_config.realm);

#if FD_CONF_FILE_NO_CONNECT_PEERS_CONFIGURED
  info.pi_diamid    = bdata(scef_name);
  info.pi_diamidlen = blength (scef_name);
  OAILOG_DEBUG (LOG_T6A, "Diameter identity of SCEF: %s with length: %zd\n", info.pi_diamid, info.pi_diamidlen);
  info.config.pic_flags.sec     = PI_SEC_NONE;
  info.config.pic_flags.pro3    = PI_P3_DEFAULT;
  info.config.pic_flags.pro4    = PI_P4_TCP;
  info.config.pic_flags.alg     = PI_ALGPREF_TCP;
  info.config.pic_flags.exp     = PI_EXP_INACTIVE;
  info.config.pic_flags.persist = PI_PRST_NONE;
  info.config.pic_port          = 3868;
  info.config.pic_lft           = 3600;
  info.config.pic_tctimer       = 7; // retry time-out connection
  info.config.pic_twtimer       = 60; // watchdog
  CHECK_FCT (fd_peer_add (&info, "", t6a_peer_connected_cb, NULL));

  if (mme_config_unlock (&mme_config) ) {
    OAILOG_ERROR (LOG_T6A, "Failed to unlock configuration\n");
    return RETURNerror;
  }
  return ret;
#else
  DiamId_t          diamid    = bdata(scef_name);
  size_t            diamidlen = blength (scef_name);
  struct peer_hdr  *peer      = NULL;
  int               nb_tries  = 0;
  for (nb_tries = 0; nb_tries < NB_MAX_TRIES; nb_tries++) {
    OAILOG_DEBUG (LOG_T6A, "T6a peer connection attempt %d / %d\n",
                  1 + nb_tries, NB_MAX_TRIES);
    ret = fd_peer_getbyid( diamid, diamidlen, 0, &peer );
    if (!ret) {
      if (peer) {
        ret = fd_peer_get_state(peer);
        if (STATE_OPEN == ret) {
          MessageDef                             *message_p;

          OAILOG_DEBUG (LOG_T6A, "Peer %*s is now connected...\n", (int)diamidlen, diamid);
          /*
           * Inform S1AP that connection to SCEF is established
           */
          message_p = itti_alloc_new_message (TASK_T6A, ACTIVATE_MESSAGE);
          itti_send_msg_to_task (TASK_S1AP, INSTANCE_DEFAULT, message_p);

          {
            FILE *fp = NULL;
            bstring  filename = bformat("/tmp/mme_%d.status", g_pid);
            fp = fopen(bdata(filename), "w+");
            bdestroy(filename);
            fprintf(fp, "ESTABLISHED Connection to SCEF\n");
            fflush(fp);
            fclose(fp);
          }
          bdestroy(scef_name);
          return RETURNok;
        } else {
          OAILOG_DEBUG (LOG_T6A, "T6a peer state is %d\n", ret);
        }
      }
    } else {
      OAILOG_DEBUG (LOG_T6A, "Could not get T6a peer\n");
    }
    sleep(1);
  }
  bdestroy(scef_name);
  return RETURNerror;
#endif
}

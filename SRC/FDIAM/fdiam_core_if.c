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
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "fdiam_defs.h"
#include "intertask_interface.h"
#include "common_types.h"
#include "assertions.h"
#include "msc.h"
#include "log.h"
#include "timer.h"

static int                              gnutls_log_level = 9;
static long                             timer_id = 0;

static void fd_gnutls_debug (int level, const char *str);

//------------------------------------------------------------------------------
static void fd_gnutls_debug (
  int loglevel,
  const char *str)
{
  OAILOG_EXTERNAL (loglevel, LOG_FDIAM, "[GTLS] %s", str);
}

//------------------------------------------------------------------------------
// callback for freeDiameter logs
static void oai_fd_logger(int loglevel, const char * format, va_list args)
{
#define FD_LOG_MAX_MESSAGE_LENGTH 8192
  char       buffer[FD_LOG_MAX_MESSAGE_LENGTH];
  int        rv = 0;

  rv = vsnprintf (buffer, 8192, format, args);
  if ((0 > rv) || ((FD_LOG_MAX_MESSAGE_LENGTH) < rv)) {
    return;
  }
  OAILOG_EXTERNAL (loglevel, LOG_FDIAM, "%s\n", buffer);
}

//------------------------------------------------------------------------------
int fdiam_init (const mme_config_t * mme_config_p)
{
  int                                     ret;
  char                                    host_name[100];
  size_t                                  host_name_len = 0;

  OAILOG_DEBUG (LOG_FDIAM, "Initializing Free Diameter core interface\n");

  /*
   * if (strcmp(fd_core_version(), free_wrapper_DIAMETER_MINIMUM_VERSION) ) {
   * FDIAM_ERROR("Freediameter version %s found, expecting %s\n", fd_core_version(),
   * free_wrapper_DIAMETER_MINIMUM_VERSION);
   * return RETURNerror;
   * } else {
   * FDIAM_DEBUG("Freediameter version %s\n", fd_core_version());
   * }
   */

  /*
   * Initializing freeDiameter logger
   */
  ret = fd_log_handler_register(oai_fd_logger);
  if (ret) {
    OAILOG_ERROR (LOG_FDIAM, "An error occurred during freeDiameter log handler registration: %d\n", ret);
    return ret;
  } else {
    OAILOG_DEBUG (LOG_FDIAM, "Initializing freeDiameter log handler done\n");
  }

  /*
   * Initializing freeDiameter core
   */
  OAILOG_DEBUG (LOG_FDIAM, "Initializing freeDiameter core...\n");
  ret = fd_core_initialize ();
  if (ret) {
    OAILOG_ERROR (LOG_FDIAM, "An error occurred during freeDiameter core library initialization: %d\n", ret);
    return ret;
  } else {
    OAILOG_DEBUG (LOG_FDIAM, "Initializing freeDiameter core done\n");
  }

  OAILOG_DEBUG (LOG_FDIAM, "Default ext path: %s\n", DEFAULT_EXTENSIONS_PATH);

  ret = fd_core_parseconf (bdata(mme_config_p->fdiam_config.conf_file));
  if (ret) {
    OAILOG_ERROR (LOG_FDIAM, "An error occurred during fd_core_parseconf file : %s.\n", bdata(mme_config_p->fdiam_config.conf_file));
    return ret;
  } else {
    OAILOG_DEBUG (LOG_FDIAM, "fd_core_parseconf done\n");
  }

  /*
   * Set gnutls debug level ?
   */
  if (gnutls_log_level) {
    gnutls_global_set_log_function ((gnutls_log_func) fd_gnutls_debug);
    gnutls_global_set_log_level (gnutls_log_level);
    OAILOG_DEBUG (LOG_FDIAM, "Enabled GNUTLS debug at level %d\n", gnutls_log_level);
  }

  /*
   * Starting freeDiameter core
   */
  ret = fd_core_start ();
  if (ret) {
    OAILOG_ERROR (LOG_FDIAM, "An error occurred during freeDiameter core library start\n");
    return ret;
  } else {
    OAILOG_DEBUG (LOG_FDIAM, "fd_core_start done\n");
  }

  ret = fd_core_waitstartcomplete ();
  if (ret) {
    OAILOG_ERROR (LOG_FDIAM, "An error occurred during freeDiameter core library start\n");
    return ret;
  } else {
    OAILOG_DEBUG (LOG_FDIAM, "fd_core_waitstartcomplete done\n");
  }

  if (fd_g_config->cnf_diamid ) {
    free (fd_g_config->cnf_diamid);
    fd_g_config->cnf_diamid_len = 0;
  }

  DevAssert (gethostname (host_name, 100) == 0);
  host_name_len = strlen (host_name);
  host_name[host_name_len] = '.';
  host_name[host_name_len + 1] = '\0';
  strcat (host_name, (const char *)mme_config.realm->data);
  fd_g_config->cnf_diamid = strdup (host_name);
  fd_g_config->cnf_diamid_len = strlen (fd_g_config->cnf_diamid);
  OAILOG_DEBUG (LOG_FDIAM, "Diameter identity of MME: %s with length: %zd\n",
                fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len);
  
  OAILOG_DEBUG (LOG_FDIAM, "Initializing Free Diameter interface: DONE\n");

  return RETURNok;
}

//------------------------------------------------------------------------------
void fdiam_terminate(void)
{
  int    rv = RETURNok;
  /* Initialize shutdown of the framework */
  rv = fd_core_shutdown();
  if (rv) {
    OAI_FPRINTF_ERR ("An error occurred during fd_core_shutdown().\n");
  }

  /* Wait for the shutdown to be complete -- this should always be called after fd_core_shutdown */
  rv = fd_core_wait_shutdown_complete();
  if (rv) {
    OAI_FPRINTF_ERR ("An error occurred during fd_core_wait_shutdown_complete().\n");
  }
}

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


#ifndef FDIAM_DEFS_H_
#define FDIAM_DEFS_H_

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "mme_config.h"
#include "queue.h"

/* Errors that fall within the Permanent Failures category shall be used to
 * inform the peer that the request has failed, and should not be attempted
 * again. The Result-Code AVP values defined in Diameter Base Protocol RFC 3588
 * shall be applied. When one of the result codes defined here is included in a
 * response, it shall be inside an Experimental-Result AVP and the Result-Code
 * AVP shall be absent.
 */

/* Result codes that fall within the transient failures category shall be used
 * to inform a peer that the request could not be satisfied at the time it was
 * received, but may be able to satisfy the request in the future. The
 * Result-Code AVP values defined in Diameter Base Protocol RFC 3588 shall be
 * applied. When one of the result codes defined here is included in a response,
 * it shall be inside an Experimental-Result AVP and the Result-Code AVP shall
 * be absent.
 */

#define DIAMETER_ERROR_IS_VENDOR(x)                    \
   ((x == DIAMETER_ERROR_USER_UNKNOWN)              || \
    (x == DIAMETER_ERROR_ROAMING_NOT_ALLOWED)       || \
    (x == DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION)  || \
    (x == DIAMETER_ERROR_RAT_NOT_ALLOWED)           || \
    (x == DIAMETER_ERROR_EQUIPMENT_UNKNOWN)         || \
    (x == DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE) || \
    (x == DIAMETER_ERROR_UNKOWN_SERVING_NODE))

typedef enum {
  DIAMETER_AUTHENTICATION_DATA_UNAVAILABLE = 4181,
  DIAMETER_ERROR_USER_UNKNOWN              = 5001,
  DIAMETER_ERROR_ROAMING_NOT_ALLOWED       = 5004,
  DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION  = 5420,
  DIAMETER_ERROR_RAT_NOT_ALLOWED           = 5421,
  DIAMETER_ERROR_EQUIPMENT_UNKNOWN         = 5422,
  DIAMETER_ERROR_UNKOWN_SERVING_NODE       = 5423,
} fdiam_experimental_result_t;

typedef enum {
  DIAMETER_SUCCESS = 2001,
} fdiam_base_result_t;

typedef struct {
#define FDIAM_RESULT_BASE         0x0
#define FDIAM_RESULT_EXPERIMENTAL 0x1
  unsigned present:1;

  union {
    /* Experimental result as defined in 3GPP TS 29.272 */
    fdiam_experimental_result_t experimental;
    /* Diameter basics results as defined in RFC 3588 */
    fdiam_base_result_t         base;
  } choice;
} fdiam_result_t;

#define AVP_CODE_VENDOR_ID                         (266)
#define AVP_CODE_EXPERIMENTAL_RESULT               (297)
#define AVP_CODE_EXPERIMENTAL_RESULT_CODE          (298)

int fdiam_init(const mme_config_t *mme_config);
void fdiam_terminate(void);

char * fdiam_experimental_retcode_2_string(uint32_t ret_code);
char * fdiam_retcode_2_string(uint32_t ret_code);
int fdiam_parse_experimental_result(struct avp *avp, fdiam_experimental_result_t *ptr);

#endif /* FDIAM_DEFS_H_ */

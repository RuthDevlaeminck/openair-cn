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

#include <stdio.h>
#include <stdint.h>

#include "mme_config.h"
#include "assertions.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "t6a_defs.h"
#include "fdiam_defs.h"
#include "t6a_messages.h"
#include "msc.h"
#include "log.h"
#include "t6a_messages_types.h"
#include "3gpp_24.008.h"

static inline int
t6a_parse_imsi (
  struct avp_hdr *hdr,
  Imsi_t         *imsi)
{
  int    ret;

  DevCheck (hdr->avp_value->os.len <= IMSI_BCD_DIGITS_MAX, hdr->avp_value->os.len, IMSI_BCD_DIGITS_MAX, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (imsi->digit, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  imsi->length = ret;
  return RETURNok;
}

static inline int
t6a_parse_msisdn (
  struct avp_hdr *hdr,
  Msisdn_t       *msisdn)
{
  int    ret;

  DevCheck (hdr->avp_value->os.len <= MSISDN_LENGTH, hdr->avp_value->os.len, MSISDN_LENGTH, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (msisdn->digit, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  msisdn->length = ret;
  return RETURNok;
}

static inline int
t6a_parse_lmsi (
  struct avp_hdr *hdr,
  Lmsi_t         *lmsi)
{
  int    ret;

  DevCheck (hdr->avp_value->os.len <= LMSI_DIGITS_MAX, hdr->avp_value->os.len, LMSI_DIGITS_MAX, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (lmsi->digit, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  lmsi->length = ret;
  return RETURNok;
}

static inline int
t6a_parse_externalId (
  struct avp_hdr *hdr,
  char           *externalId)
{
  int    ret;
  DevCheck (hdr->avp_value->os.len <= EXTERNAL_ID_LENGTH, hdr->avp_value->os.len, EXTERNAL_ID_LENGTH, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (externalId, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  return RETURNok;
}

static inline int
t6a_parse_originHost (
  struct avp_hdr *hdr,
  char           *originHost)
{
  int    ret;
  DevCheck (hdr->avp_value->os.len <= HOST_NAME_MAX, hdr->avp_value->os.len, HOST_NAME_MAX, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (originHost, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  return RETURNok;
}

static inline int
t6a_parse_originRealm (
  struct avp_hdr *hdr,
  char           *originRealm)
{
  int    ret;
  DevCheck (hdr->avp_value->os.len <= REALM_NAME_MAX, hdr->avp_value->os.len, REALM_NAME_MAX, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (originRealm, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  return RETURNok;
}

static inline int
t6a_parse_bearer_identifier (
  struct avp_hdr *hdr,
  bearer_id_t    *bearerId)
{
  int    ret;

  DevCheck (hdr->avp_value->os.len <= BEARER_ID_MAX_LENGTH, hdr->avp_value->os.len, BEARER_ID_MAX_LENGTH, 0);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = sprintf (bearerId->data, "%*s", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
  bearerId->length = ret;
  return RETURNok;
}

static inline int
t6a_parse_extendedPCO (
  struct avp_hdr                      *hdr,
  protocol_configuration_options_t    *pco)
{
  int    ret;

  DevCheck (hdr->avp_value->os.len <= PCO_MAX_LENGTH, hdr->avp_value->os.len, PCO_MAX_LENGTH, PCO_MIN_LENGTH);

  if (hdr->avp_value->os.len == 0)
    return RETURNok;

  ret = decode_protocol_configuration_options (pco, hdr->avp_value->os.data, (int)hdr->avp_value->os.len);
  return RETURNok;
}

static inline int
t6a_parse_drmp (
  struct avp_hdr *hdr,
  uint32_t       *drmp)
{
  *drmp = hdr->avp_value->u32;
  return RETURNok;
}

static inline int
t6a_parse_pdnConnChargingId (
  struct avp_hdr *hdr,
  uint32_t       *chargingId)
{
  *chargingId = hdr->avp_value->u32;
  return RETURNok;
}

int
t6a_parse_user_identifier (
  struct avp                *avp_data,
  t6a_user_identifier_t     *userid)
{
  struct avp                             *avp = NULL;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_browse (avp_data, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_USER_NAME:
      CHECK_FCT (t6a_parse_imsi (hdr, &userid->imsi));
      FLAGS_SET(userid->set_mask, USERID_IMSI_FLAG);
      break;
    case AVP_CODE_MSISDN:
      CHECK_FCT (t6a_parse_msisdn (hdr, &userid->msisdn));
      FLAGS_SET(userid->set_mask, USERID_MSISDN_FLAG);
      break;
    case AVP_CODE_EXTERNAL_IDENTIFIER:
      CHECK_FCT (t6a_parse_externalId (hdr, userid->external_id));
      FLAGS_SET(userid->set_mask, USERID_EXTERNID_FLAG);
      break;
    case AVP_CODE_LMSI:
      CHECK_FCT (t6a_parse_lmsi (hdr, &userid->lmsi));
      FLAGS_SET(userid->set_mask, USERID_LMSI_FLAG);
      break;
    default:
      return RETURNerror;
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  return RETURNok;
}

int
t6a_parse_ocSupportedFeatures (
  struct avp   *avp_data,
  uint64_t     *featureVector)
{
  struct avp                             *avp = NULL;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_browse (avp_data, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_OC_FEATURE_VECTOR:
      *featureVector = hdr->avp_value->u64;
      break;
    default:
      break;    // Unexpected AVP. Ignore
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  return RETURNok;
}

int
t6a_parse_oc_olr (
  struct avp        *avp_data,
  t6a_oc_olr_t      *ocOlr)
{
  struct avp                             *avp = NULL;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_browse (avp_data, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_OC_SEQUENCE_NUMBER:
      ocOlr->sequence_num = hdr->avp_value->u64;
      FLAGS_SET(ocOlr->set_mask, OCOLR_SEQNUM_FLAG);
      break;
    case AVP_CODE_OC_REPORT_TYPE:
      ocOlr->report_type = hdr->avp_value->u32;
      FLAGS_SET(ocOlr->set_mask, OCOLR_RPTTYPE_FLAG);
      break;
    case AVP_CODE_OC_REDUCTION_PERCENTAGE:
      ocOlr->reduction_pct = hdr->avp_value->u32;
      FLAGS_SET(ocOlr->set_mask, OCOLR_REDUCTPCT_FLAG);
      break;
    case AVP_CODE_OC_VALIDITY_DURATION:
      ocOlr->validity_duration = hdr->avp_value->u32;
      FLAGS_SET(ocOlr->set_mask, OCOLR_VALIDDUR_FLAG);
      break;
    default:
      break;   // Ignore AVP. Unexpected
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  return RETURNok;
}

int
t6a_parse_supportedFeature (
  struct avp                   *avp_data,
  t6a_supported_features_t     *feature)
{
  struct avp                             *avp = NULL;
  struct avp_hdr                         *hdr;

  CHECK_FCT (fd_msg_browse (avp_data, MSG_BRW_FIRST_CHILD, &avp, NULL));

  while (avp) {
    CHECK_FCT (fd_msg_avp_hdr (avp, &hdr));

    switch (hdr->avp_code) {
    case AVP_CODE_VENDOR_ID:
      feature->vendor_id = hdr->avp_value->u32;
      FLAGS_SET(feature->set_mask, SUPPFEAT_VENDOR_FLAG);
      break;
    case AVP_CODE_FEATURE_LIST_ID:
      feature->feature_list_id = hdr->avp_value->u32;
      FLAGS_SET(feature->set_mask, SUPPFEAT_FEATLISTID_FLAG);
      break;
    case AVP_CODE_FEATURE_LIST:
      feature->feature_list = hdr->avp_value->u32;
      FLAGS_SET(feature->set_mask, SUPPFEAT_FEATLIST_FLAG);
      break;
    default:
      break;   // Ignore AVP. Unexpected
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (avp, MSG_BRW_NEXT, &avp, NULL));
  }

  return RETURNok;
}

int
t6a_cma_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  struct msg                             *ans_p = NULL;
  struct msg                             *qry_p = NULL;
  struct avp                             *avp_p = NULL;
  struct avp_hdr                         *hdr_p = NULL;
  MessageDef                             *message_p = NULL;
  t6a_conn_mgmt_ans_t                    *t6a_conn_mgmt_ans_p = NULL;

  DevAssert (msg_pP);
  ans_p = *msg_pP;
  
  /*
   * Retrieve the original query associated with the answer
   */
  CHECK_FCT (fd_msg_answ_getq (ans_p, &qry_p));
  DevAssert (qry_p);
  message_p = itti_alloc_new_message (TASK_T6A, T6A_CONN_MGMT_ANS);
  t6a_conn_mgmt_ans_p = &message_p->ittiMsg.t6a_conn_mgmt_ans;
  
  // Get User Identifier from original query
  CHECK_FCT (fd_msg_search_avp (qry_p, t6a_fd_cnf.dataobj_t6a_user_identifier, &avp_p));
  if (avp_p) {
    FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_USERID_FLAG);
    CHECK_FCT (t6a_parse_user_identifier (avp_p, &t6a_conn_mgmt_ans_p->user_identifier));
  } else {
    DevMessage ("Query has been freed before we received the answer\n");
  }

  // Get Bearer Identifier from original query
  CHECK_FCT (fd_msg_search_avp (qry_p, t6a_fd_cnf.dataobj_t6a_bearer_identifier, &avp_p));
  if (avp_p) {
    CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
    FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_BEARERID_FLAG);
    CHECK_FCT (t6a_parse_bearer_identifier (hdr_p, &t6a_conn_mgmt_ans_p->bearer_id));
  }

  // Parse answer message
  CHECK_FCT (fd_msg_browse (ans_p, MSG_BRW_FIRST_CHILD, &avp_p, NULL));

  while (avp_p) {
    CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));

    switch (hdr_p->avp_code) {
    case AVP_CODE_ORIGIN_HOST:
      // Retrieve origin host from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_ORIGINHOST_FLAG);
      CHECK_FCT (t6a_parse_originHost (hdr_p, t6a_conn_mgmt_ans_p->origin_host));
      break;
    case AVP_CODE_ORIGIN_REALM:
      // Retrieve origin realm from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_ORIGINREALM_FLAG);
      CHECK_FCT (t6a_parse_originRealm (hdr_p, t6a_conn_mgmt_ans_p->origin_realm));
      break;
    case AVP_CODE_DRMP:
      // Retrieve drmp from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_DRMP_FLAG);
      CHECK_FCT (t6a_parse_drmp (hdr_p, &t6a_conn_mgmt_ans_p->drmp));
      break;
    case AVP_CODE_RESULT_CODE:
      // Retrieve the result-code from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_RESULTCODE_FLAG);
      t6a_conn_mgmt_ans_p->result.present = FDIAM_RESULT_BASE;
      t6a_conn_mgmt_ans_p->result.choice.base = hdr_p->avp_value->u32;
      MSC_LOG_TX_MESSAGE (MSC_T6A_MME, MSC_MMEAPP_MME, NULL, 0, "0 T6A_CONN_MGMT_ANS imsi %s %s",
                          t6a_conn_mgmt_ans_p->user_identifier.imsi, fdiam_retcode_2_string (hdr_p->avp_value->u32));
  
      if (hdr_p->avp_value->u32 != ER_DIAMETER_SUCCESS) {
        OAILOG_ERROR (LOG_T6A, "Got error %u:%s\n", hdr_p->avp_value->u32, fdiam_retcode_2_string (hdr_p->avp_value->u32));
        goto err;
      }
      break;
    case AVP_CODE_EXPERIMENTAL_RESULT:
        /*
         * The procedure has failed within the SCEF
         * * * * NOTE: contrary to result-code, the experimental-result is a grouped
         * * * * AVP and requires parsing its childs to get the code back.
         */
        FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_RESULTCODE_FLAG);
        t6a_conn_mgmt_ans_p->result.present = FDIAM_RESULT_EXPERIMENTAL;
        fdiam_parse_experimental_result (avp_p, &t6a_conn_mgmt_ans_p->result.choice.experimental);
        goto err;
        break;
    case AVP_CODE_OC_SUPPORTED_FEATURES:
      // Retrieve OC Supported Features from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_OCFEATURE_FLAG);
      CHECK_FCT (t6a_parse_ocSupportedFeatures (hdr_p, &t6a_conn_mgmt_ans_p->oc_feature_vector));
      break;
    case AVP_CODE_OC_OLR:
      // Retrieve OC OLR from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_OCOLR_FLAG);
      CHECK_FCT (t6a_parse_oc_olr (avp_p, &t6a_conn_mgmt_ans_p->oc_olr));
      break;
    case AVP_CODE_SUPPORTED_FEATURES:
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_SUPPFEATURE_FLAG);
      if (t6a_conn_mgmt_ans_p->num_supp_features < MAX_SUPP_FEATURES) {
        CHECK_FCT (t6a_parse_supportedFeature (avp_p,
                                               &t6a_conn_mgmt_ans_p->supported_features[t6a_conn_mgmt_ans_p->num_supp_features]));
        t6a_conn_mgmt_ans_p->num_supp_features += 1;
      }
      break;
    case AVP_CODE_PDN_CONNECTION_CHARGING_ID:
      // Retrieve charging id from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_PDNCONNCHARGINGID_FLAG);
      CHECK_FCT (t6a_parse_pdnConnChargingId (hdr_p, &t6a_conn_mgmt_ans_p->pdn_conn_charging_id));
      break;
    case AVP_CODE_EXTENDED_PCO:
      // Retrieve PCO from answer
      FLAGS_SET(t6a_conn_mgmt_ans_p->set_mask, CMA_XTENDPCO_FLAG);
      CHECK_FCT (t6a_parse_extendedPCO (hdr_p, &t6a_conn_mgmt_ans_p->extended_pco));
      break;
    case AVP_CODE_FAILED_AVP:
      // This is grouped AVP but not sure how to decode yet
      break;
    default:
      break;       // Ignore
    }

    /*
     * Go to next AVP in the grouped AVP
     */
    CHECK_FCT (fd_msg_browse (ans_p, MSG_BRW_NEXT, &avp_p, NULL));
  }

err:
  ans_p = NULL;
  itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
  OAILOG_DEBUG (LOG_T6A, "Sending T6A_CONN_MGMT_ANS to task MME_APP\n");
  return RETURNok;
}

int
t6a_ria_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  struct msg                             *ans_p = NULL;
  struct msg                             *qry_p = NULL;
  struct avp                             *avp_p = NULL;
  struct avp_hdr                         *hdr_p = NULL;
  MessageDef                             *message_p = NULL;
  t6a_report_info_ans_t                  *t6a_report_info_ans_p = NULL;

  DevAssert (msg_pP);
  ans_p = *msg_pP;
  /*
   * Retrieve the original query associated with the answer
   */
  CHECK_FCT (fd_msg_answ_getq (ans_p, &qry_p));
  DevAssert (qry_p);
  message_p = itti_alloc_new_message (TASK_T6A, T6A_REPORT_INFO_ANS);
  t6a_report_info_ans_p = &message_p->ittiMsg.t6a_report_info_ans;
  CHECK_FCT (fd_msg_search_avp (qry_p, t6a_fd_cnf.dataobj_t6a_user_name, &avp_p));

  if (avp_p) {
    CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
    memcpy (t6a_report_info_ans_p->imsi, hdr_p->avp_value->os.data, hdr_p->avp_value->os.len);
    t6a_report_info_ans_p->imsi[hdr_p->avp_value->os.len] = '\0';
    t6a_report_info_ans_p->imsi_length = hdr_p->avp_value->os.len;
    OAILOG_DEBUG (LOG_T6A, "Received t6a rir for imsi=%*s\n", (int)hdr_p->avp_value->os.len, hdr_p->avp_value->os.data);
  } else {
    DevMessage ("Query has been freed before we received the answer\n");
  }

  /*
   * Retrieve the result-code
   */
  CHECK_FCT (fd_msg_search_avp (ans_p, t6a_fd_cnf.dataobj_t6a_result_code, &avp_p));

  if (avp_p) {
    CHECK_FCT (fd_msg_avp_hdr (avp_p, &hdr_p));
    t6a_report_info_ans_p->result.present = FDIAM_RESULT_BASE;
    t6a_report_info_ans_p->result.choice.base = hdr_p->avp_value->u32;
    MSC_LOG_TX_MESSAGE (MSC_T6A_MME, MSC_MMEAPP_MME, NULL, 0, "0 T6A_REPORT_INFO_ANS imsi %s %s", t6a_report_info_ans_p->imsi, fdiam_retcode_2_string (hdr_p->avp_value->u32));

    if (hdr_p->avp_value->u32 != ER_DIAMETER_SUCCESS) {
      OAILOG_ERROR (LOG_T6A, "Got error %u:%s\n", hdr_p->avp_value->u32, fdiam_retcode_2_string (hdr_p->avp_value->u32));
      goto err;
    }
  } else {
    /*
     * The result-code is not present, may be it is an experimental result
     * * * * avp_p indicating a 3GPP specific failure.
     */
    CHECK_FCT (fd_msg_search_avp (ans_p, t6a_fd_cnf.dataobj_t6a_experimental_result, &avp_p));

    if (avp_p) {
      /*
       * The procedure has failed within the HSS.
       * * * * NOTE: contrary to result-code, the experimental-result is a grouped
       * * * * AVP and requires parsing its childs to get the code back.
       */
      t6a_report_info_ans_p->result.present = FDIAM_RESULT_EXPERIMENTAL;
      fdiam_parse_experimental_result (avp_p, &t6a_report_info_ans_p->result.choice.experimental);
      goto err;
    } else {
      /*
       * Neither result-code nor experimental-result is present ->
       * * * * totally incorrect behaviour here.
       */
      OAILOG_ERROR (LOG_T6A, "Experimental-Result and Result-Code are absent: " "This is not a correct behaviour\n");
      goto err;
    }
  }


err:
  ans_p = NULL;
  itti_send_msg_to_task (TASK_MME_APP, INSTANCE_DEFAULT, message_p);
  OAILOG_DEBUG (LOG_T6A, "Sending T6A_REPORT_INFO_ANS to task MME_APP\n");
  return RETURNok;
}

int
t6a_cir_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  return RETURNok;
}

int
t6a_oda_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  return RETURNok;
}

int
t6a_tdr_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  return RETURNok;
}

int
t6a_cmr_cb (struct msg       **msg_pP,
            struct avp        *paramavp_pP,
            struct session    *sess_pP,
            void              *opaque_pP,
            enum disp_action  *act_pP)
{
  return RETURNok;
}

int
t6a_generate_report_info_req(t6a_report_info_req_t *rir_p)
{
  struct msg       *msg;

  CHECK_FCT (t6a_msg_new_rir (&msg));

  if (FLAG_IS_SET(rir_p->set_mask,RIR_DRMP_FLAG)) {
    CHECK_FCT (t6a_msg_add_drmp (msg, rir_p->drmp));
  }
  
  if (FLAG_IS_SET(rir_p->set_mask,RIR_USERID_FLAG)) {
    CHECK_FCT (t6a_msg_add_userIdentifier (msg, &rir_p->user_identifier));
  }
  
  if (FLAG_IS_SET(rir_p->set_mask,RIR_OCFEATURE_FLAG)) {
    CHECK_FCT (t6a_msg_add_OCSuppFeatures (msg, rir_p->oc_feature_vector));
  }
  
  if (FLAG_IS_SET(rir_p->set_mask,RIR_SUPPFEATURE_FLAG)) {
    CHECK_FCT (t6a_msg_add_suppFeatures (msg, &rir_p->supported_features));
  }
  
  if (FLAG_IS_SET(rir_p->set_mask,RIR_MONEVTRPT_FLAG)) {
    for (int idx=0; idx < rir_p->num_monitor_events; idx++) {
      CHECK_FCT (t6a_msg_add_monEvtRpt (msg, &rir_p->monitor_event_report[idx]));
    }
  }
  
  CHECK_FCT (fd_msg_send (&msg, NULL, NULL));
  
  return RETURNok;
}

int
t6a_generate_conn_mgmt_req(t6a_conn_mgmt_req_t *cmr_p)
{
  struct msg       *msg;

  CHECK_FCT (t6a_msg_new_cmr (&msg));

  if (cmr_p->set_mask & CMR_DRMP_FLAG) {
    CHECK_FCT (t6a_msg_add_drmp (msg, cmr_p->drmp));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_USERID_FLAG)) {
    CHECK_FCT (t6a_msg_add_userIdentifier (msg, &cmr_p->user_identifier));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_BEARERID_FLAG)) {
    CHECK_FCT (t6a_msg_add_bearerIdentifier (msg, &cmr_p->bearer_id));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_OCFEATURE_FLAG)) {
    CHECK_FCT (t6a_msg_add_OCSuppFeatures (msg, cmr_p->oc_feature_vector));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_CMRFLAGS_FLAG)) {
    CHECK_FCT (t6a_msg_add_cmrFlags (msg, cmr_p->cmr_flags));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_MAXUEAVAILTIME_FLAG)) {
    CHECK_FCT (t6a_msg_add_maxUEAvailTime (msg, cmr_p->max_ue_avail_time));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_SUPPFEATURE_FLAG)) {
    for (int idx=0; idx < cmr_p->num_supp_features; idx++) {
       CHECK_FCT (t6a_msg_add_suppFeatures (msg, &cmr_p->supported_features[idx]));
    }
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_SRVCSELECTION_FLAG)) {
    CHECK_FCT (t6a_msg_add_srvcSelection (msg, cmr_p->service_selection));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_PLMNRATECTRL_FLAG)) {
    CHECK_FCT (t6a_msg_add_plmnRateCtrl (msg, &cmr_p->plmn_rate_ctrl));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_XTENDPCO_FLAG)) {
    CHECK_FCT (t6a_msg_add_extendedPCO (msg, &cmr_p->extended_pco));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_3GPPCHARGING_FLAG)) {
    CHECK_FCT (t6a_msg_add_3gppChargingChars (msg, cmr_p->charging_chars));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_RATTYPE_FLAG)) {
    CHECK_FCT (t6a_msg_add_ratType (msg, cmr_p->rat_type));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_TERMINFO_FLAG)) {
    CHECK_FCT (t6a_msg_add_termInfo (msg, &cmr_p->term_info));
  }
  
  if (FLAG_IS_SET(cmr_p->set_mask,CMR_VISITPLMN_FLAG)) {
    CHECK_FCT (t6a_msg_add_visitedPlmnId (msg, cmr_p->visited_plmn_id));
  }
  
  CHECK_FCT (fd_msg_send (&msg, NULL, NULL));
  
  return RETURNok;
}

int
t6a_msg_new_rir (struct msg    **msg_p)
{
  struct avp                             *avp_p = NULL;
  struct session                         *sess_p = NULL;
  union avp_value                         value;

  /*
   * Create the new report info request message
   */
  CHECK_FCT (fd_msg_new (t6a_fd_cnf.dataobj_t6a_rir, 0, msg_p));
  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "appt6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_session_id, 0, &avp_p));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_FIRST_CHILD, avp_p));
  }

  /*
   * AUTH Session State - No State maintained
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_auth_session_state, 0, &avp_p));
  value.i32 = 1;
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));

  /*
   * Add Origin_Host & Origin_Realm
   */
  CHECK_FCT (fd_msg_add_origin (*msg_p, 0));
  
  mme_config_read_lock (&mme_config);

  /*
   * Destination SCEF Host
   */
  {
    bstring     scefHost = bstrcpy(mme_config.fdiam_config.scef_host_name);

    bconchar(scefHost, '.');
    bconcat (scefHost, mme_config.realm);

    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_destination_host, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(scefHost);
    value.os.len = blength(scefHost);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));
    bdestroy(scefHost);
  }

  /*
   * Destination_Realm
   */
  {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_destination_realm, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(mme_config.realm);
    value.os.len = blength(mme_config.realm);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));
  }
  mme_config_unlock (&mme_config);

  return RETURNok;
}

int
t6a_msg_new_cmr (struct msg    **msg_p)
{
  struct avp                             *avp_p = NULL;
  struct session                         *sess_p = NULL;
  union avp_value                         value;

  /*
   * Create the new config mgmt request message
   */
  CHECK_FCT (fd_msg_new (t6a_fd_cnf.dataobj_t6a_cmr, 0, msg_p));
  /*
   * Create a new session
   */
  CHECK_FCT (fd_sess_new (&sess_p, fd_g_config->cnf_diamid, fd_g_config->cnf_diamid_len, (os0_t) "appt6a", 6));
  {
    os0_t                                   sid;
    size_t                                  sidlen;

    CHECK_FCT (fd_sess_getsid (sess_p, &sid, &sidlen));
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_session_id, 0, &avp_p));
    value.os.data = sid;
    value.os.len = sidlen;
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_FIRST_CHILD, avp_p));
  }

  /*
   * AUTH Session State - No State maintained
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_auth_session_state, 0, &avp_p));
  value.i32 = 1;
  CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
  CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));

  /*
   * Add Origin_Host & Origin_Realm
   */
  CHECK_FCT (fd_msg_add_origin (*msg_p, 0));
  mme_config_read_lock (&mme_config);

  /*
   * Destination SCEF Host
   */
  {
    bstring     scefHost = bstrcpy(mme_config.fdiam_config.scef_host_name);

    bconchar(scefHost, '.');
    bconcat (scefHost, mme_config.realm);

    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_destination_host, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(scefHost);
    value.os.len = blength(scefHost);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));
    bdestroy(scefHost);
  }

  /*
   * Destination_Realm
   */
  {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_destination_realm, 0, &avp_p));
    value.os.data = (unsigned char *)bdata(mme_config.realm);
    value.os.len = blength(mme_config.realm);
    CHECK_FCT (fd_msg_avp_setvalue (avp_p, &value));
    CHECK_FCT (fd_msg_avp_add (*msg_p, MSG_BRW_LAST_CHILD, avp_p));
  }
  mme_config_unlock (&mme_config);

  return RETURNok;
}

int
t6a_msg_add_drmp(struct msg *msg_p, 
                 uint32_t    drmp)
{
  struct avp                             *drmp_avp = NULL;
  union avp_value                         value;

  /*
   * DRMP - Routing msg priority
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_drmp, 0, &drmp_avp));
  value.i32 = drmp;
  CHECK_FCT (fd_msg_avp_setvalue (drmp_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, drmp_avp));

  return RETURNok;
}

int
t6a_msg_add_ratType(struct msg   *msg_p, 
                    rat_type_t    ratType)
{
  struct avp                             *rat_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_rat_type, 0, &rat_avp));
  value.i32 = ratType;
  CHECK_FCT (fd_msg_avp_setvalue (rat_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, rat_avp));

  return RETURNok;
}

int
t6a_msg_add_cmrFlags(struct msg *msg_p, 
                     uint32_t    cmrFlags)
{
  struct avp                             *cmr_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_cmr_flags, 0, &cmr_avp));
  value.u32 = cmrFlags;
  CHECK_FCT (fd_msg_avp_setvalue (cmr_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, cmr_avp));

  return RETURNok;
}

int
t6a_msg_add_maxUEAvailTime(struct msg *msg_p, 
                           uint32_t    maxUEAvailTime)
{
  struct avp                             *availTime_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_maximum_ue_availability_time, 0, &availTime_avp));
  uint8_t  timeOctets[4];
  time_t  currTime = time(NULL);
  time_t  epochTime = currTime + 2208988800UL;
  time_t  maxUETime = epochTime + maxUEAvailTime;    // NTP UTC 
  UTC_T_TO_TBCD(maxUETime,timeOctets);
  value.os.data = timeOctets;
  value.os.len = 4;
  CHECK_FCT (fd_msg_avp_setvalue (availTime_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, availTime_avp));

  return RETURNok;
}

int
t6a_msg_add_userIdentifier(struct msg             *msg_p,
                           t6a_user_identifier_t  *user_ident)
{
  struct avp                             *userid_avp = NULL;
  struct avp                             *child_avp  = NULL;
  union avp_value                         value;

  /*
   * Adding User-Identifier AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_user_identifier, 0, &userid_avp));
    
  // Add User-Name (IMSI) (UTF8String)
  if (user_ident->set_mask & USERID_IMSI_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_user_name, 0, &child_avp));
    value.os.data = (unsigned char *)user_ident->imsi.digit;
    value.os.len = user_ident->imsi.length;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (userid_avp, MSG_BRW_LAST_CHILD, child_avp));
  }    

  // Add MSISDN (OctetString)
  if (user_ident->set_mask & USERID_MSISDN_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_msisdn, 0, &child_avp));
    value.os.data = user_ident->msisdn.digit;
    value.os.len = user_ident->msisdn.length;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (userid_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add External Identifier (UTF8String)
  if (user_ident->set_mask & USERID_EXTERNID_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_external_identifier, 0, &child_avp));
    value.os.data = (unsigned char*)user_ident->external_id;
    value.os.len = strlen(user_ident->external_id);
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (userid_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add LMSI
  if (user_ident->set_mask & USERID_LMSI_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_lmsi, 0, &child_avp));
    value.os.data = user_ident->lmsi.digit;
    value.os.len = user_ident->lmsi.length;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (userid_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, userid_avp));
  
  return RETURNok;
}

int
t6a_msg_add_bearerIdentifier(struct msg             *msg_p,
                             bearer_id_t            *bearerId)
{
  struct avp                             *bearerid_avp = NULL;
  union avp_value                         value;

  /*
   * Adding Bearer-Identifier AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_bearer_identifier, 0, &bearerid_avp));
  value.os.data = bearerId->data;
  value.os.len = bearerId->length;
  CHECK_FCT (fd_msg_avp_setvalue (bearerid_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, bearerid_avp));
  
  return RETURNok;
}

int
t6a_msg_add_srvcSelection(struct msg   *msg_p,
                          char         *srvcSelection)
{
  struct avp                             *srvcSelect_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_service_selection, 0, &srvcSelect_avp));
  value.os.data = (unsigned char*)srvcSelection;
  value.os.len = strlen(srvcSelection);
  CHECK_FCT (fd_msg_avp_setvalue (srvcSelect_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, srvcSelect_avp));
  
  return RETURNok;
}

int
t6a_msg_add_3gppChargingChars(struct msg   *msg_p,
                              char         *chargingChars)
{
  struct avp                             *charging_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_3gpp_charging_characteristics, 0, &charging_avp));
  value.os.data = (unsigned char*)chargingChars;
  value.os.len = strlen(chargingChars);
  CHECK_FCT (fd_msg_avp_setvalue (charging_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, charging_avp));
  
  return RETURNok;
}

int
t6a_msg_add_OCSuppFeatures(struct msg   *msg_p, 
                           uint64_t      oc_feature_vector)
{
  struct avp                             *avp_p = NULL;
  struct avp                             *vector_avp = NULL;
  union avp_value                         value;

  /*
   * Adding OC-Supported-Features AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_oc_supported_features, 0, &avp_p));
    
  // Add OC-Feature-Vector (Unsigned64)
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_oc_feature_vector, 0, &vector_avp));
  value.u64 = oc_feature_vector;
  CHECK_FCT (fd_msg_avp_setvalue (vector_avp, &value));
  CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, vector_avp));
        
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  return RETURNok;
}

int
t6a_msg_add_suppFeatures(struct msg                 *msg_p,
                         t6a_supported_features_t   *supp_features)
{
  struct avp                             *avp_p = NULL;
  struct avp                             *child_avp = NULL;
  union avp_value                         value;

  /*
   * Adding Supported-Features AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_supported_features, 0, &avp_p));
    
  // Add Supported-Feature VendorId (Unsigned32)
  if (supp_features->set_mask & SUPPFEAT_VENDOR_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_vendor_id, 0, &child_avp));
    value.u32 = supp_features->vendor_id;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Add Supported-Feature FeatureListId (Unsigned32)
  if (supp_features->set_mask & SUPPFEAT_FEATLISTID_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_feature_list_id, 0, &child_avp));
    value.u32 = supp_features->feature_list_id;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Add Supported-Feature FeatureList (Unsigned32)
  if (supp_features->set_mask & SUPPFEAT_FEATLIST_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_feature_list, 0, &child_avp));
    value.u32 = supp_features->feature_list;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Supported Features AVP        
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  return RETURNok;
}

int
t6a_msg_add_plmnRateCtrl(struct msg                     *msg_p,
                         t6a_serving_plmn_rate_ctrl_t   *plmnRateCtrl)
{
  struct avp                             *avp_p = NULL;
  struct avp                             *child_avp = NULL;
  union avp_value                         value;

  /*
   * Adding Group AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_serving_plmn_rate_control, 0, &avp_p));
    
  // Add Uplink Rate Limit (Unsigned32)
  if (plmnRateCtrl->set_mask & PLMNRATECTRL_UPLINKLIMIT_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_uplink_rate_limit, 0, &child_avp));
    value.u32 = plmnRateCtrl->uplink_rate_limit;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Add Downlink Rate Limit (Unsigned32)
  if (plmnRateCtrl->set_mask & PLMNRATECTRL_DOWNLINKLIMIT_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_downlink_rate_limit, 0, &child_avp));
    value.u32 = plmnRateCtrl->downlink_rate_limit;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Serving PLMN Rate Control AVP        
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  return RETURNok;
}

int
t6a_msg_add_termInfo(struct msg                 *msg_p,
                     t6a_terminal_info_t        *termInfo)
{
  struct avp                             *avp_p = NULL;
  struct avp                             *child_avp = NULL;
  union avp_value                         value;

  /*
   * Adding Group AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_terminal_information, 0, &avp_p));
    
  // Add IMEI (UTF8String)
  if (termInfo->set_mask & TERMINFO_IMEI_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_imei, 0, &child_avp));
    char imeiStr[16];
    IMEI_TO_STRING(&termInfo->imei, imeiStr, 16);
    value.os.data = (unsigned char*)imeiStr;
    value.os.len = strlen(imeiStr);
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Add MEID (OctetString)
  if (termInfo->set_mask & TERMINFO_MEID_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_3gpp2_meid, 0, &child_avp));
    uint8_t   octetStr[7];
    MEID_T_TO_TBCD(termInfo->meid, octetStr);
    value.os.data = octetStr;
    value.os.len = 7;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }
    
  // Add Software Version (UTF8String)
  if (termInfo->set_mask & TERMINFO_SWVERSION_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_software_version, 0, &child_avp));
    value.os.data = (unsigned char*)termInfo->sw_version;
    value.os.len = 2;   // Sofware version is 2 characters
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, child_avp));
  }

  
  // Terminal Info AVP        
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, avp_p));

  return RETURNok;
}

int
t6a_msg_add_extendedPCO(struct msg                         *msg_p,
                        protocol_configuration_options_t   *pco)
{
  struct avp                             *child_avp = NULL;
  union avp_value                         value;
  uint8_t                                 pcoOctets[PCO_MAX_LENGTH];
  uint8_t                                 octetsLen = 0;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_extended_pco, 0, &child_avp));

  // Call encode from 3gpp_24.008
  octetsLen = encode_protocol_configuration_options(pco, pcoOctets, PCO_MAX_LENGTH);

  value.os.data = pcoOctets;
  value.os.len = octetsLen;
  CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, child_avp));
  
  return RETURNok;
}

int
t6a_msg_add_visitedPlmnId(struct msg          *msg_p,
                          plmn_t              visitedPlmnId)
{
  struct avp                             *child_avp = NULL;
  union avp_value                         value;

  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_visited_plmn_id, 0, &child_avp));
  uint8_t  plmn[3];
  PLMN_T_TO_TBCD (visitedPlmnId,
                  plmn,
                  mme_config_find_mnc_length (visitedPlmnId.mcc_digit1, 
                                              visitedPlmnId.mcc_digit2, 
                                              visitedPlmnId.mcc_digit3, 
                                              visitedPlmnId.mnc_digit1, 
                                              visitedPlmnId.mnc_digit2, 
                                              visitedPlmnId.mnc_digit3));
  value.os.data = plmn;
  value.os.len = 3;
  CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, child_avp));
  
  return RETURNok;
}

int
t6a_msg_add_monEvtRpt(struct msg                     *msg_p,
                      t6a_monitor_event_report_t     *mon_evt_rpt)
{
  struct avp              *monevtrpt_avp = NULL;
  struct avp              *child_avp     = NULL;
  union avp_value          value;

  /*
   * Create Monitoring-Event-Report AVP
   */
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_monitoring_event_report, 0, &monevtrpt_avp));
    
  // Add SCEF ReferenceId (Unsigned32)
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_scef_reference_id, 0, &child_avp));
  value.u32 = mon_evt_rpt->scef_reference_id;
  CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
  CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
        
  // Add SCEF Id (DiameterIdentity)
  if (mon_evt_rpt->set_mask & MONEVTRPT_SCEFID_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_scef_id, 0, &child_avp));
    value.os.data = (unsigned char*)mon_evt_rpt->scef_id;
    value.os.len  = strlen(mon_evt_rpt->scef_id);
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add Visited PLMN Id (Octet String)
  if (mon_evt_rpt->set_mask & MONEVTRPT_VISITPLMN_FLAG) {
    uint8_t       plmn[3];
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_visited_plmn_id, 0, &child_avp));
    PLMN_T_TO_TBCD (mon_evt_rpt->visited_plmn_id,
                    plmn,
                    mme_config_find_mnc_length (mon_evt_rpt->visited_plmn_id.mcc_digit1, 
                                                mon_evt_rpt->visited_plmn_id.mcc_digit2, 
                                                mon_evt_rpt->visited_plmn_id.mcc_digit3, 
                                                mon_evt_rpt->visited_plmn_id.mnc_digit1, 
                                                mon_evt_rpt->visited_plmn_id.mnc_digit2, 
                                                mon_evt_rpt->visited_plmn_id.mnc_digit3));
    value.os.data = plmn;
    value.os.len = 3;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add Roaming Information (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_ROAMINFO_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_roaming_information, 0, &child_avp));
    value.u32 = mon_evt_rpt->roaming_information;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }    

  // Add IMEI Change (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_IMEICHANGE_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_imei_change, 0, &child_avp));
    value.u32 = mon_evt_rpt->imei_change;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add Reachability Information (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_REACHINFO_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_reachability_information, 0, &child_avp));
    value.u32 = mon_evt_rpt->reachability_information;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }
  
  // Add Loss of Connectivity Information (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_LOSSOFCONN_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_loss_of_connectivity_reason, 0, &child_avp));
    value.u32 = mon_evt_rpt->loss_of_connectivity_reason;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }    

  // Add Monitoring Type Information (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_MONTYPE_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_monitoring_type, 0, &child_avp));
    value.u32 = mon_evt_rpt->monitoring_type;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Add Event Handling Information (Unsigned32)
  if (mon_evt_rpt->set_mask & MONEVTRPT_EVENTHNDL_FLAG) {
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_event_handling, 0, &child_avp));
    value.u32 = mon_evt_rpt->event_handling;
    CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, child_avp));
  }

  // Create EPS Location Information (Grouped)
  if (mon_evt_rpt->set_mask & MONEVTRPT_EPSINFO_FLAG) {
    CHECK_FCT (t6a_avp_add_epsLocateInfo (monevtrpt_avp, &mon_evt_rpt->eps_location_info));
  }

  // Create Communication Failure Information (Grouped)
  if (mon_evt_rpt->set_mask & MONEVTRPT_COMMFAIL_FLAG) {
    struct avp              *commfailinfo_avp = NULL;
    union avp_value                         value;

    // Create Communication Failure Information (Grouped)
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_communication_failure_information, 0, &commfailinfo_avp));
  
    // Add Cause Type (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_CAUSETYPE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_cause_type, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.cause_type;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add S1AP Cause (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_S1APCAUSE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_s1ap_cause, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.s1ap_cause;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add RANAP Cause (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_RANAPCAUSE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_ranap_cause, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.ranap_cause;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add BSSAP Cause (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_BSSAPCAUSE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_bssap_cause, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.bssap_cause;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add GMM Cause (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_GMMCAUSE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_gmm_cause, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.gmm_cause;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add SM Cause (Unsigned32)
    if (mon_evt_rpt->comm_fail_info.set_mask & COMMFAIL_SMCAUSE_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_sm_cause, 0, &child_avp));
      value.u32 = mon_evt_rpt->comm_fail_info.sm_cause;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (commfailinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, commfailinfo_avp));
  }

  if (mon_evt_rpt->set_mask & MONEVTRPT_SRVCRPT_FLAG) {
    for (int idx=0; idx < mon_evt_rpt->num_srvc_reports; idx++) {
      struct avp                            *srvc_rpt_avp = NULL;
      struct avp                            *srvc_result_avp = NULL;
      struct avp                            *node_type_avp = NULL;

      // Create Service Report 
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_service_report, 0, &srvc_rpt_avp));

      if ((mon_evt_rpt->srvc_report_info[idx].set_mask & SRVCRPT_VENDOR_FLAG) ||
          (mon_evt_rpt->srvc_report_info[idx].set_mask & SRVCRPT_RESULTCODE_FLAG)) {  
        // Create Service Result (Grouped)
        CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_service_result, 0, &srvc_result_avp));

        {
          struct avp                            *vendor_avp = NULL;
          struct avp                            *result_code_avp = NULL;

          // Add Vendor Id (Unsigned32)
          if (mon_evt_rpt->srvc_report_info[idx].set_mask & SRVCRPT_VENDOR_FLAG) {
            CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_vendor_id, 0, &vendor_avp));
            value.u32 = mon_evt_rpt->srvc_report_info[idx].vendor_id;
            CHECK_FCT (fd_msg_avp_setvalue (vendor_avp, &value));
            CHECK_FCT (fd_msg_avp_add (srvc_result_avp, MSG_BRW_LAST_CHILD, vendor_avp));
          }
    
          // Add Service Result Code (Unsigned32)
          if (mon_evt_rpt->srvc_report_info[idx].set_mask & SRVCRPT_RESULTCODE_FLAG) {
            CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_service_result_code, 0, &result_code_avp));
            value.u32 = mon_evt_rpt->srvc_report_info[idx].service_result_code;
            CHECK_FCT (fd_msg_avp_setvalue (result_code_avp, &value));
            CHECK_FCT (fd_msg_avp_add (srvc_result_avp, MSG_BRW_LAST_CHILD, result_code_avp));
          }

          // Adding Service Result AVP to Service Report AVP
          CHECK_FCT (fd_msg_avp_add (srvc_rpt_avp, MSG_BRW_LAST_CHILD, srvc_result_avp));
        }
      }

      // Add Node Type (Unsigned32)
      if (mon_evt_rpt->srvc_report_info[idx].set_mask & SRVCRPT_NODETYPE_FLAG) {
        CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_node_type, 0, &node_type_avp));
        value.u32 = mon_evt_rpt->srvc_report_info[idx].node_type;
        CHECK_FCT (fd_msg_avp_setvalue (node_type_avp, &value));
        CHECK_FCT (fd_msg_avp_add (srvc_rpt_avp, MSG_BRW_LAST_CHILD, node_type_avp));
      }
        
      CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, srvc_rpt_avp));
    }
  }


  if (mon_evt_rpt->set_mask & MONEVTRPT_NUMUERPT_FLAG) {
    for (int idx=0; idx < mon_evt_rpt->num_ue_count_reports; idx++) {
      struct avp                            *ue_rpt_avp = NULL;

      // Create Number of UE Per Location Report AVP (Grouped)
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_number_of_ue_per_location_report, 0, &ue_rpt_avp));

      // Create EPS Location Info 
      if (mon_evt_rpt->set_mask & NUMUERPT_EPSINFO_FLAG) {
        CHECK_FCT (t6a_avp_add_epsLocateInfo (ue_rpt_avp, &mon_evt_rpt->ue_count_reports[idx].eps_location_info));
      }

      // Create UE Count
      if (mon_evt_rpt->set_mask & NUMUERPT_UECOUNT_FLAG) {
        struct avp              *uecount_avp = NULL;
        CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_ue_count, 0, &uecount_avp));
        value.u32 = mon_evt_rpt->ue_count_reports[idx].ue_count;
        CHECK_FCT (fd_msg_avp_setvalue (uecount_avp, &value));
        CHECK_FCT (fd_msg_avp_add (ue_rpt_avp, MSG_BRW_LAST_CHILD, uecount_avp));
      }

      CHECK_FCT (fd_msg_avp_add (monevtrpt_avp, MSG_BRW_LAST_CHILD, ue_rpt_avp));
    }
  }
  
  // Supported Features AVP        
  CHECK_FCT (fd_msg_avp_add (msg_p, MSG_BRW_LAST_CHILD, monevtrpt_avp));

  return RETURNok;
}

int
t6a_avp_add_epsLocateInfo(struct avp                *avp_p,
                          t6a_eps_location_info_t   *eps_location_info)			    
{
  struct avp    *epsinfo_avp = NULL;
  struct avp    *child_avp = NULL;
  union avp_value          value;
  
  CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_eps_location_information, 0, &epsinfo_avp));

  if (eps_location_info->set_mask & EPSINFO_MME_FLAG) {
    struct avp              *mmeinfo_avp = NULL;

    // Create MME-Location Information (Grouped)
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_mme_location_information, 0, &mmeinfo_avp));

    // Add E UTRAN Cell Global Identity (Octet String)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_ECGI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_e_utran_cell_global_identity, 0, &child_avp));

      uint8_t       ecgi[7];
      PLMN_T_TO_TBCD (eps_location_info->mme_location_info.ecgi.plmn,
                      ecgi,
                      mme_config_find_mnc_length (eps_location_info->mme_location_info.ecgi.plmn.mcc_digit1, 
                                                  eps_location_info->mme_location_info.ecgi.plmn.mcc_digit2, 
                                                  eps_location_info->mme_location_info.ecgi.plmn.mcc_digit3, 
                                                  eps_location_info->mme_location_info.ecgi.plmn.mnc_digit1, 
                                                  eps_location_info->mme_location_info.ecgi.plmn.mnc_digit2, 
                                                  eps_location_info->mme_location_info.ecgi.plmn.mnc_digit3));
      uint8_t* cellPtr = (uint8_t*)&ecgi[3];
      ECGI_T_TO_TBCD(eps_location_info->mme_location_info.ecgi.cell_identity,
                     cellPtr); 
      value.os.data = ecgi;
      value.os.len = 7;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Tracking Area Identity (Octet String)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_TAI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_tracking_area_identity, 0, &child_avp));
      uint8_t       tai[5];
      PLMN_T_TO_TBCD (eps_location_info->mme_location_info.tai.plmn,
                      tai,
                      mme_config_find_mnc_length (eps_location_info->mme_location_info.tai.plmn.mcc_digit1, 
                                                  eps_location_info->mme_location_info.tai.plmn.mcc_digit2, 
                                                  eps_location_info->mme_location_info.tai.plmn.mcc_digit3, 
                                                  eps_location_info->mme_location_info.tai.plmn.mnc_digit1, 
                                                  eps_location_info->mme_location_info.tai.plmn.mnc_digit2, 
                                                  eps_location_info->mme_location_info.tai.plmn.mnc_digit3));
      tai[3] = eps_location_info->mme_location_info.tai.tac >> 8;
      tai[4] = eps_location_info->mme_location_info.tai.tac & 0xff;
      value.os.data = tai;
      value.os.len = 5;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }
    
    // Add Geographical Information (Octet String)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_GEOGRAPHIC_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_geographical_information, 0, &child_avp));
      value.os.data = (uint8_t*)&eps_location_info->mme_location_info.geographical_info;
      value.os.len  = 8;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }
 
    // Add Geodetic Information (OctetString)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_GEODETIC_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_geodetic_information, 0, &child_avp));
      value.os.data = (uint8_t*)&eps_location_info->mme_location_info.geodetic_info;
      value.os.len  = 10;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }
        
    // Add Current Location Retrieved (Enumerated)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_CURRLOCATION_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_current_location_retrieved, 0, &child_avp));
      value.i32 = eps_location_info->mme_location_info.current_location_retrieved;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Age of Location Information (Unsigned32)
    if (eps_location_info->mme_location_info.set_mask & MMEINFO_AGEOFLOCATION_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_age_of_location_information, 0, &child_avp));
      value.u32 = eps_location_info->mme_location_info.age_of_location_information;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (mmeinfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    CHECK_FCT (fd_msg_avp_add (epsinfo_avp, MSG_BRW_LAST_CHILD, mmeinfo_avp));
  }

  if (eps_location_info->set_mask & EPSINFO_SGSN_FLAG) {
    struct avp              *sgsninfo_avp = NULL;

    // Create SGSN-Location Information (Grouped)
    CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_sgsn_location_information, 0, &sgsninfo_avp));

    // Add Cell Global Identity (Octet String)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_CGI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_cell_global_identity, 0, &child_avp));
      uint8_t       cgi[7];
      PLMN_T_TO_TBCD (eps_location_info->sgsn_location_info.cgi.plmn,
                      cgi,
                      mme_config_find_mnc_length (eps_location_info->sgsn_location_info.cgi.plmn.mcc_digit1, 
                                                  eps_location_info->sgsn_location_info.cgi.plmn.mcc_digit2, 
                                                  eps_location_info->sgsn_location_info.cgi.plmn.mcc_digit3, 
                                                  eps_location_info->sgsn_location_info.cgi.plmn.mnc_digit1, 
                                                  eps_location_info->sgsn_location_info.cgi.plmn.mnc_digit2, 
                                                  eps_location_info->sgsn_location_info.cgi.plmn.mnc_digit3));
      cgi[3] = eps_location_info->sgsn_location_info.cgi.lac >> 8;
      cgi[4] = eps_location_info->sgsn_location_info.cgi.lac & 0xff;
      cgi[5] = eps_location_info->sgsn_location_info.cgi.ci >> 8;
      cgi[6] = eps_location_info->sgsn_location_info.cgi.ci & 0xff;
      value.os.data = cgi;
      value.os.len  = 7;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Location Area Identity (Octet String)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_LAI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_location_area_identity, 0, &child_avp));
      uint8_t       lai[7];
      PLMN_T_TO_TBCD (eps_location_info->sgsn_location_info.lai.plmn,
                      lai,
                      mme_config_find_mnc_length (eps_location_info->sgsn_location_info.lai.plmn.mcc_digit1, 
                                                  eps_location_info->sgsn_location_info.lai.plmn.mcc_digit2, 
                                                  eps_location_info->sgsn_location_info.lai.plmn.mcc_digit3, 
                                                  eps_location_info->sgsn_location_info.lai.plmn.mnc_digit1, 
                                                  eps_location_info->sgsn_location_info.lai.plmn.mnc_digit2, 
                                                  eps_location_info->sgsn_location_info.lai.plmn.mnc_digit3));
      lai[3] = eps_location_info->sgsn_location_info.lai.lac >> 8;
      lai[4] = eps_location_info->sgsn_location_info.lai.lac & 0xff;
      value.os.data = lai;
      value.os.len  = 5;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Service Area Identity (Octet String)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_SAI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_service_area_identity, 0, &child_avp));
      uint8_t       sai[7];
      PLMN_T_TO_TBCD (eps_location_info->sgsn_location_info.sai.plmn,
                      sai,
                      mme_config_find_mnc_length (eps_location_info->sgsn_location_info.sai.plmn.mcc_digit1, 
                                                  eps_location_info->sgsn_location_info.sai.plmn.mcc_digit2, 
                                                  eps_location_info->sgsn_location_info.sai.plmn.mcc_digit3, 
                                                  eps_location_info->sgsn_location_info.sai.plmn.mnc_digit1, 
                                                  eps_location_info->sgsn_location_info.sai.plmn.mnc_digit2, 
                                                  eps_location_info->sgsn_location_info.sai.plmn.mnc_digit3));
      sai[3] = eps_location_info->sgsn_location_info.sai.lac >> 8;
      sai[4] = eps_location_info->sgsn_location_info.sai.lac & 0xff;
      sai[5] = eps_location_info->sgsn_location_info.sai.sac >> 8;
      sai[6] = eps_location_info->sgsn_location_info.sai.sac & 0xff;
      value.os.data = sai;
      value.os.len  = 7;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Routing Area Identity (Octet String)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_RAI_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_routing_area_identity, 0, &child_avp));
      uint8_t       rai[6];
      PLMN_T_TO_TBCD (eps_location_info->sgsn_location_info.rai.plmn,
                      rai,
                      mme_config_find_mnc_length (eps_location_info->sgsn_location_info.rai.plmn.mcc_digit1, 
                                                  eps_location_info->sgsn_location_info.rai.plmn.mcc_digit2, 
                                                  eps_location_info->sgsn_location_info.rai.plmn.mcc_digit3, 
                                                  eps_location_info->sgsn_location_info.rai.plmn.mnc_digit1, 
                                                  eps_location_info->sgsn_location_info.rai.plmn.mnc_digit2, 
                                                  eps_location_info->sgsn_location_info.rai.plmn.mnc_digit3));
      rai[3] = eps_location_info->sgsn_location_info.rai.lac >> 8;
      rai[4] = eps_location_info->sgsn_location_info.rai.lac & 0xff;
      rai[5] = eps_location_info->sgsn_location_info.rai.rac;
      value.os.data = rai;
      value.os.len = 6;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Geographical Information (Octet String)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_GEOGRAPHIC_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_geographical_information, 0, &child_avp));
      value.os.data = (uint8_t*)&eps_location_info->sgsn_location_info.geographical_info;
      value.os.len  = 8;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }
 
    // Add Geodetic Information (OctetString)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_GEODETIC_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_geodetic_information, 0, &child_avp));
      value.os.data = (uint8_t*)&eps_location_info->sgsn_location_info.geodetic_info;
      value.os.len  = 10;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }
        
    // Add Current Location Retrieved (Enumerated)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_CURRLOCATION_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_current_location_retrieved, 0, &child_avp));
      value.i32 = eps_location_info->sgsn_location_info.current_location_retrieved;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    // Add Age of Location Information (Unsigned32)
    if (eps_location_info->sgsn_location_info.set_mask & SGSNINFO_AGEOFLOCATION_FLAG) {
      CHECK_FCT (fd_msg_avp_new (t6a_fd_cnf.dataobj_t6a_age_of_location_information, 0, &child_avp));
      value.u32 = eps_location_info->sgsn_location_info.age_of_location_information;
      CHECK_FCT (fd_msg_avp_setvalue (child_avp, &value));
      CHECK_FCT (fd_msg_avp_add (sgsninfo_avp, MSG_BRW_LAST_CHILD, child_avp));
    }

    CHECK_FCT (fd_msg_avp_add (epsinfo_avp, MSG_BRW_LAST_CHILD, sgsninfo_avp));
  }

  CHECK_FCT (fd_msg_avp_add (avp_p, MSG_BRW_LAST_CHILD, epsinfo_avp));
    
  return RETURNok;
}

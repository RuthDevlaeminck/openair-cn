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

#ifndef T6A_MESSAGES_H_
#define T6A_MESSAGES_H_

#include "3gpp_23.003.h"

int t6a_generate_report_info_req(t6a_report_info_req_t *rir_p);
//int t6a_generate_config_info_ans(t6a_config_info_ans_t *cia_p);
//int t6a_generate_mo_data_req(t6a_mo_data_req_t *odr_p);
//int t6a_generate_mt_data_ans(t6a_mt_data_ans_t *tda_p);
int t6a_generate_conn_mgmt_req(t6a_conn_mgmt_req_t *cmr_p);
//int t6a_generate_conn_mgmt_ans(t6a_conn_mgmt_ans_t *cma_p);

int t6a_msg_new_rir (struct msg    **msg_p);
int t6a_msg_new_cmr (struct msg    **msg_p);

int t6a_msg_add_drmp(struct msg *msg_p, 
                     uint32_t    drmp);

int t6a_msg_add_ratType(struct msg   *msg_p, 
                        rat_type_t    ratType);

int t6a_msg_add_cmrFlags(struct msg *msg_p, 
                         uint32_t    cmrFlags);

int t6a_msg_add_maxUEAvailTime(struct msg *msg_p, 
                               uint32_t    maxUEAvailTime);

int t6a_msg_add_userIdentifier(struct msg *msg_p,
			       t6a_user_identifier_t  *user_ident);

int  t6a_msg_add_bearerIdentifier(struct msg             *msg_p,
                                  bearer_id_t            *bearerId);

int t6a_msg_add_srvcSelection(struct msg   *msg_p,
                              char         *srvcSelection);

int t6a_msg_add_3gppChargingChars(struct msg   *msg_p,
                                  char         *chargingChars);

int t6a_msg_add_OCSuppFeatures(struct msg   *msg_p, 
                               uint64_t      oc_feature_vector);

int t6a_msg_add_suppFeatures(struct msg                 *msg_p,
			     t6a_supported_features_t   *supp_features);

int t6a_msg_add_plmnRateCtrl(struct msg                     *msg_p,
                             t6a_serving_plmn_rate_ctrl_t   *plmnRateCtrl);

int t6a_msg_add_termInfo(struct msg                 *msg_p,
                         t6a_terminal_info_t        *termInfo);

int t6a_msg_add_extendedPCO(struct msg                         *msg_p,
                            protocol_configuration_options_t   *pco);

int t6a_msg_add_visitedPlmnId(struct msg          *msg_p,
                              plmn_t               visitedPlmnId);

// Monitor Event Report
int t6a_msg_add_monEvtRpt(struct msg                     *msg_p,
			  t6a_monitor_event_report_t     *mon_evt_rpt);

// EPS Location Info
int t6a_avp_add_epsLocateInfo(struct avp                *avp_p,
			      t6a_eps_location_info_t   *eps_location_info);

int t6a_ria_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int t6a_cir_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int t6a_oda_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int t6a_tdr_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int t6a_cmr_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);
int t6a_cma_cb(struct msg **msg, struct avp *paramavp,
               struct session *sess, void *opaque,
               enum disp_action *act);

// TODO: Will be removed
//int t6a_parse_subscription_data(struct avp *avp_subscription_data,
//                                subscription_data_t *subscription_data);

#endif /* T6A_MESSAGES_H_ */

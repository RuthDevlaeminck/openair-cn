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


#ifndef T6A_DEFS_H_
#define T6A_DEFS_H_

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "fdiam_defs.h"
#include "mme_config.h"
#include "queue.h"

#define VENDOR_3GPP (10415)
#define APP_T6A     (16777346)

typedef struct {
  struct dict_object *dataobj_t6a_vendor;     /* t6a vendor object */
  struct dict_object *dataobj_t6a_app;        /* t6a application object */

  /* Commands */
  struct dict_object *dataobj_t6a_rir; /* t6a reporting information request */
  struct dict_object *dataobj_t6a_ria; /* t6a reporting information answer */
  struct dict_object *dataobj_t6a_cir; /* t6a configuration information request */
  struct dict_object *dataobj_t6a_cia; /* t6a configuration information answer */
  struct dict_object *dataobj_t6a_cmr; /* t6a connection mgmt request */
  struct dict_object *dataobj_t6a_cma; /* t6a connection mgmt answer */
  struct dict_object *dataobj_t6a_odr; /* t6a MO Data request */
  struct dict_object *dataobj_t6a_oda; /* t6a MO Data answer */
  struct dict_object *dataobj_t6a_tdr; /* t6a MT Data request */
  struct dict_object *dataobj_t6a_tda; /* t6a MT Data answer */

  /* Some standard basic AVPs */
  struct dict_object *dataobj_t6a_session_id;
  struct dict_object *dataobj_t6a_drmp;
  struct dict_object *dataobj_t6a_user_identifier;
  struct dict_object *dataobj_t6a_bearer_identifier;
  struct dict_object *dataobj_t6a_origin_host;
  struct dict_object *dataobj_t6a_origin_realm;
  struct dict_object *dataobj_t6a_destination_host;
  struct dict_object *dataobj_t6a_destination_realm;
  struct dict_object *dataobj_t6a_auth_session_state;
  struct dict_object *dataobj_t6a_result_code;
  struct dict_object *dataobj_t6a_experimental_result;
  struct dict_object *dataobj_t6a_supported_features;
  struct dict_object *dataobj_t6a_oc_supported_features;     /* Overload Conveyance */

  /* T6A specific AVPs */
  struct dict_object *dataobj_t6a_monitoring_event_configuration;  /* Config Info Request */
  struct dict_object *dataobj_t6a_monitoring_event_config_status;  /* Config Info Answer */
  struct dict_object *dataobj_t6a_monitoring_event_report;         /* Config Info Answer and Report Info Request */
  struct dict_object *dataobj_t6a_oc_olr;
  struct dict_object *dataobj_t6a_oc_sequence_number;
  struct dict_object *dataobj_t6a_oc_validity_duration;
  struct dict_object *dataobj_t6a_oc_report_type;
  struct dict_object *dataobj_t6a_oc_reduction_percentage;
  struct dict_object *dataobj_t6a_failed_avp;
  struct dict_object *dataobj_t6a_non_ip_data;
  struct dict_object *dataobj_t6a_rrc_cause_counter;               /* MO Data Request */
  struct dict_object *dataobj_t6a_scef_wait_time;                  /* MT Data Request */
  struct dict_object *dataobj_t6a_maximum_retransmission_time;     /* MT Data Request */
  struct dict_object *dataobj_t6a_requested_retransmission_time;   /* MT Data Answer */
  struct dict_object *dataobj_t6a_cmr_flags;                       /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_maximum_ue_availability_time;    /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_connection_action;               /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_service_selection;               /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_serving_plmn_rate_control;       /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_uplink_rate_limit;               /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_downlink_rate_limit;             /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_extended_pco;                    /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_3gpp_charging_characteristics;   /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_rat_type;                        /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_terminal_information;            /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_imei;                            /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_3gpp2_meid;                      /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_software_version;                /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_visited_plmn_id;                 /* Conn Mgmt Request */
  struct dict_object *dataobj_t6a_pdn_connection_charging_id;      /* Conn Mgmt Answer */
  struct dict_object *dataobj_t6a_msisdn;                          /* Reporting Info */
  struct dict_object *dataobj_t6a_user_name;                       /* Reporting Info - IMSI */      
  struct dict_object *dataobj_t6a_external_identifier;             /* Reporting Info */
  struct dict_object *dataobj_t6a_lmsi;                            /* Reporting Info */      
  struct dict_object *dataobj_t6a_oc_feature_vector;               /* Reporting Info */      
  struct dict_object *dataobj_t6a_vendor_id;      
  struct dict_object *dataobj_t6a_feature_list_id;      
  struct dict_object *dataobj_t6a_feature_list;      
  struct dict_object *dataobj_t6a_scef_reference_id;      
  struct dict_object *dataobj_t6a_scef_id;      
  struct dict_object *dataobj_t6a_roaming_information;      
  struct dict_object *dataobj_t6a_imei_change;      
  struct dict_object *dataobj_t6a_reachability_information;  
  struct dict_object *dataobj_t6a_loss_of_connectivity_reason;  
  struct dict_object *dataobj_t6a_eps_location_information;      
  struct dict_object *dataobj_t6a_mme_location_information;      
  struct dict_object *dataobj_t6a_sgsn_location_information;      
  struct dict_object *dataobj_t6a_e_utran_cell_global_identity;      
  struct dict_object *dataobj_t6a_tracking_area_identity;      
  struct dict_object *dataobj_t6a_geographical_information;      
  struct dict_object *dataobj_t6a_geodetic_information;      
  struct dict_object *dataobj_t6a_current_location_retrieved;      
  struct dict_object *dataobj_t6a_age_of_location_information;      
  struct dict_object *dataobj_t6a_cell_global_identity;      
  struct dict_object *dataobj_t6a_location_area_identity;      
  struct dict_object *dataobj_t6a_service_area_identity;      
  struct dict_object *dataobj_t6a_routing_area_identity;      
  struct dict_object *dataobj_t6a_monitoring_type;      
  struct dict_object *dataobj_t6a_event_handling;      
  struct dict_object *dataobj_t6a_service_report;      
  struct dict_object *dataobj_t6a_service_result;      
  struct dict_object *dataobj_t6a_service_result_code;      
  struct dict_object *dataobj_t6a_node_type;      
  struct dict_object *dataobj_t6a_communication_failure_information;      
  struct dict_object *dataobj_t6a_cause_type;      
  struct dict_object *dataobj_t6a_s1ap_cause;      
  struct dict_object *dataobj_t6a_ranap_cause;      
  struct dict_object *dataobj_t6a_bssap_cause;      
  struct dict_object *dataobj_t6a_gmm_cause;      
  struct dict_object *dataobj_t6a_sm_cause;      
  struct dict_object *dataobj_t6a_number_of_ue_per_location_report;      
  struct dict_object *dataobj_t6a_ue_count;      
  //struct dict_object *dataobj_t6a_;      

  /* Handlers */
  struct disp_hdl *ria_hdl;   /* Reporting Information Answer Handle */
  struct disp_hdl *cir_hdl;   /* Config Info Request Handle */
  struct disp_hdl *cmr_hdl;   /* Conn Mgmt Request Handle */
  struct disp_hdl *cma_hdl;   /* Conn Mgmt Answer Handle */
  struct disp_hdl *oda_hdl;   /* MO Data Answer Handle */
  struct disp_hdl *tdr_hdl;   /* MT Data Request Handle */
} t6a_fd_cnf_t;

extern t6a_fd_cnf_t t6a_fd_cnf;

/* IANA defined IP address type */
#define IANA_IPV4   (0x1)
#define IANA_IPV6   (0x2)

#define AVP_CODE_USER_NAME                         (1)
#define AVP_CODE_3GPP_CHARGING_CHARACTERISTICS     (13)
#define AVP_CODE_SESSION_ID                        (263)
#define AVP_CODE_ORIGIN_HOST                       (264)
#define AVP_CODE_VENDOR_ID                         (266)
#define AVP_CODE_RESULT_CODE                       (268)
#define AVP_CODE_AUTH_SESSION_STATE                (277)
#define AVP_CODE_DESTINATION_REALM                 (283)
#define AVP_CODE_DESTINATION_HOST                  (293)
#define AVP_CODE_ORIGIN_REALM                      (296)
#define AVP_CODE_EXPERIMENTAL_RESULT               (297)
#define AVP_CODE_EXPERIMENTAL_RESULT_CODE          (298)
#define AVP_CODE_DRMP                              (301)
#define AVP_CODE_SERVICE_SELECTION                 (493)
#define AVP_CODE_OC_SUPPORTED_FEATURES             (621)
#define AVP_CODE_OC_FEATURE_VECTOR                 (622)
#define AVP_CODE_OC_OLR                            (623)
#define AVP_CODE_OC_SEQUENCE_NUMBER                (624)
#define AVP_CODE_OC_VALIDITY_DURATION              (625)
#define AVP_CODE_OC_REPORT_TYPE                    (626)
#define AVP_CODE_OC_REDUCTION_PERCENTAGE           (627)
#define AVP_CODE_SUPPORTED_FEATURES                (628)
#define AVP_CODE_FEATURE_LIST_ID                   (629)
#define AVP_CODE_FEATURE_LIST                      (630)
#define AVP_CODE_MSISDN                            (701)
#define AVP_CODE_FAILED_AVP                        (729)
#define AVP_CODE_BEARER_IDENTIFIER                 (1020)
#define AVP_CODE_RAT_TYPE                          (1032)
#define AVP_CODE_TERMINAL_INFORMATION              (1401)
#define AVP_CODE_IMEI                              (1402)
#define AVP_CODE_SOFTWARE_VERSION                  (1403)
#define AVP_CODE_VISITED_PLMN_ID                   (1407)
#define AVP_CODE_3GPP2_MEID                        (1471)
#define AVP_CODE_EPS_LOCATION_INFORMATION          (1496)
#define AVP_CODE_MME_LOCATION_INFORMATION          (1600)
#define AVP_CODE_SGSN_LOCATION_INFORMATION         (1601)
#define AVP_CODE_E_UTRAN_CELL_GLOBAL_IDENTITY      (1602)
#define AVP_CODE_TRACKING_AREA_IDENTITY            (1603)
#define AVP_CODE_CELL_GLOBAL_IDENTITY              (1604)
#define AVP_CODE_ROUTING_AREA_IDENTITY             (1605)
#define AVP_CODE_LOCATION_AREA_IDENTITY            (1606)
#define AVP_CODE_SERVICE_AREA_IDENTITY             (1607)
#define AVP_CODE_GEOGRAPHICAL_INFORMATION          (1608)
#define AVP_CODE_GEODETIC_INFORMATION              (1609)
#define AVP_CODE_CURRENT_LOCATION_RECEIVED         (1610)
#define AVP_CODE_AGE_OF_LOCATION_INFORMATION       (1611)
#define AVP_CODE_PDN_CONNECTION_CHARGING_ID        (2050)
#define AVP_CODE_LMSI                              (2400)
#define AVP_CODE_USER_IDENTIFIER                   (3102)
#define AVP_CODE_EXTERNAL_IDENTIFIER               (3111)
#define AVP_CODE_MONITORING_EVENT_CONFIGURATION    (3122)
#define AVP_CODE_MONITORING_EVENT_REPORT           (3123)
#define AVP_CODE_SCEF_REFERENCE_ID                 (3124)
#define AVP_CODE_SCEF_ID                           (3125)
#define AVP_CODE_MONITORING_TYPE                   (3127)
#define AVP_CODE_ROAMING_INFORMATION               (3139)
#define AVP_CODE_REACHABILITY_INFORMATION          (3140)
#define AVP_CODE_IMEI_CHANGE                       (3141)
#define AVP_CODE_MONITORING_EVENT_CONFIG_STATUS    (3142)
#define AVP_CODE_SERVICE_RESULT                    (3146)
#define AVP_CODE_SERVICE_RESULT_CODE               (3147)
#define AVP_CODE_EVENT_HANDLING                    (3149)
#define AVP_CODE_SERVICE_REPORT                    (3152)
#define AVP_CODE_NODE_TYPE                         (3153)
#define AVP_CODE_MAXIMUM_UE_AVAILABILITY_TIME      (3329)
#define AVP_CODE_MAXIMUM_RETRANSMISSION_TIME       (3330)
#define AVP_CODE_REQUESTED_RETRANSMISSION_TIME     (3331)
#define AVP_CODE_CAUSE_TYPE                        (4301)
#define AVP_CODE_S1AP_CAUSE                        (4302)
#define AVP_CODE_RANAP_CAUSE                       (4303)
#define AVP_CODE_GMM_CAUSE                         (4304)
#define AVP_CODE_SM_CAUSE                          (4305)
#define AVP_CODE_NUMBER_OF_UE_PER_LOCATION_REPORT  (4307)
#define AVP_CODE_UE_COUNT                          (4308)
#define AVP_CODE_BSSAP_CAUSE                       (4309)
#define AVP_CODE_SERVING_PLMN_RATE_CONTROL         (4310)
#define AVP_CODE_UPLINK_RATE_LIMIT                 (4311)
#define AVP_CODE_DOWNLINK_RATE_LIMIT               (4312)
#define AVP_CODE_EXTENDED_PCO                      (4313)
#define AVP_CODE_CONNECTION_ACTION                 (4314)
#define AVP_CODE_NON_IP_DATA                       (4315)
#define AVP_CODE_SCEF_WAIT_TIME                    (4316)
#define AVP_CODE_CMR_FLAGS                         (4317)
#define AVP_CODE_RRC_CAUSE_COUNTER                 (4318)

// #define AVP_CODE_

#define AVP_CODE_BANDWIDTH_UL                      (516)
#define AVP_CODE_BANDWIDTH_DL                      (515)
#define AVP_CODE_SERVED_PARTY_IP_ADDRESS           (848)
#define AVP_CODE_QCI                               (1028)
#define AVP_CODE_ALLOCATION_RETENTION_PRIORITY     (1034)
#define AVP_CODE_PRIORITY_LEVEL                    (1046)
#define AVP_CODE_PRE_EMPTION_CAPABILITY            (1047)
#define AVP_CODE_PRE_EMPTION_VULNERABILITY         (1048)
#define AVP_CODE_SUBSCRIPTION_DATA                 (1400)
#define AVP_CODE_AUTHENTICATION_INFO               (1413)
#define AVP_CODE_E_UTRAN_VECTOR                    (1414)
#define AVP_CODE_NETWORK_ACCESS_MODE               (1417)
#define AVP_CODE_CONTEXT_IDENTIFIER                (1423)
#define AVP_CODE_SUBSCRIBER_STATUS                 (1424)
#define AVP_CODE_ACCESS_RESTRICTION_DATA           (1426)
#define AVP_CODE_ALL_APN_CONFIG_INC_IND            (1428)
#define AVP_CODE_APN_CONFIGURATION_PROFILE         (1429)
#define AVP_CODE_APN_CONFIGURATION                 (1430)
#define AVP_CODE_EPS_SUBSCRIBED_QOS_PROFILE        (1431)
#define AVP_CODE_AMBR                              (1435)
#define AVP_CODE_RAND                              (1447)
#define AVP_CODE_XRES                              (1448)
#define AVP_CODE_AUTN                              (1449)
#define AVP_CODE_KASME                             (1450)
#define AVP_CODE_PDN_TYPE                          (1456)
#define AVP_CODE_SUBSCRIBED_PERIODIC_RAU_TAU_TIMER (1619)

int t6a_init(const mme_config_t *mme_config);

int t6a_fd_new_peer(void);

void t6a_peer_connected_cb(struct peer_info *info, void *arg);

int t6a_fd_init_dict_objs(void);

#endif /* T6A_DEFS_H_ */

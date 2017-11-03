/*
 * Copyright (c) 2015, EURECOM (www.eurecom.fr)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */
#ifndef FILE_T6A_MESSAGES_TYPES_SEEN
#define FILE_T6A_MESSAGES_TYPES_SEEN

#include "common_types.h"
#include "3gpp_23.003.h"
#include "3gpp_24.008.h"

#define T6A_REPORT_INFO_REQ(mSGpTR)  (mSGpTR)->ittiMsg.t6a_report_info_req
#define T6A_REPORT_INFO_ANS(mSGpTR)  (mSGpTR)->ittiMsg.t6a_report_info_ans
#define T6A_CONFIG_INFO_REQ(mSGpTR)  (mSGpTR)->ittiMsg.t6a_config_info_req
#define T6A_CONFIG_INFO_ANS(mSGpTR)  (mSGpTR)->ittiMsg.t6a_config_info_ans
#define T6A_CONN_MGMT_REQ(mSGpTR)    (mSGpTR)->ittiMsg.t6a_conn_mgmt_req
#define T6A_CONN_MGMT_ANS(mSGpTR)    (mSGpTR)->ittiMsg.t6a_conn_mgmt_ans
#define T6A_MO_DATA_REQ(mSGpTR)      (mSGpTR)->ittiMsg.t6a_mo_data_req
#define T6A_MO_DATA_ANS(mSGpTR)      (mSGpTR)->ittiMsg.t6a_mo_data_ans
#define T6A_MT_DATA_REQ(mSGpTR)      (mSGpTR)->ittiMsg.t6a_mt_data_req
#define T6A_MT_DATA_ANS(mSGpTR)      (mSGpTR)->ittiMsg.t6a_mt_data_ans

#define AUTS_LENGTH 14
#define RESYNC_PARAM_LENGTH AUTS_LENGTH + RAND_LENGTH_OCTETS

// User Identifier
#define USERID_IMSI_FLAG             (1U)
#define USERID_MSISDN_FLAG           (1U << 1)
#define USERID_LMSI_FLAG             (1U << 2)
#define USERID_EXTERNID_FLAG         (1U << 3)

typedef struct t6a_user_identifier_s {
  uint32_t   set_mask;
  Imsi_t     imsi;                                // Optional. username
  Msisdn_t   msisdn;                              // Optional
  Lmsi_t     lmsi;                                // Optional
  char       external_id[EXTERNAL_ID_LENGTH+1];   // Optional
} t6a_user_identifier_t;

// Serving PLMN Rate Control
#define PLMNRATECTRL_UPLINKLIMIT_FLAG            (1U)
#define PLMNRATECTRL_DOWNLINKLIMIT_FLAG          (1U << 1)

typedef struct t6a_serving_plmn_rate_ctrl_s {
  uint32_t            set_mask;
  uint32_t            uplink_rate_limit;          // Optional. Max NAS Data PDUs per deci hour
  uint32_t            downlink_rate_limit;        // Optional. Max NAS Data PDUs per deci hour
} t6a_serving_plmn_rate_ctrl_t;

#define TERMINFO_IMEI_FLAG               (1U)
#define TERMINFO_MEID_FLAG               (1U << 1)
#define TERMINFO_SWVERSION_FLAG          (1U << 2)

typedef struct t6a_terminal_info_s {
  uint32_t          set_mask;
  imei_t            imei;           // Optional.  3gpp_23.003.h
  meid_t            meid;           // Optional
  char              sw_version[3];  // Optional  (2 digit SW version of IMEI) - Allow one space for Null char
} t6a_terminal_info_t;

// Supported Features
#define SUPPFEAT_VENDOR_FLAG            (1U)
#define SUPPFEAT_FEATLISTID_FLAG        (1U << 1)
#define SUPPFEAT_FEATLIST_FLAG          (1U << 2)

#define MAX_SUPP_FEATURES     5

typedef struct t6a_supported_features_s {
  uint32_t            set_mask;
  uint32_t            vendor_id;                  // Mandatory. Fixed value: 10415
  uint32_t            feature_list_id;            // Mandatory. Fixed value: 1
  uint32_t            feature_list;               // Mandatory
} t6a_supported_features_t;

// MME Location Info
#define MMEINFO_ECGI_FLAG            (1U)
#define MMEINFO_TAI_FLAG             (1U << 1)
#define MMEINFO_GEOGRAPHIC_FLAG      (1U << 2)
#define MMEINFO_GEODETIC_FLAG        (1U << 3)
#define MMEINFO_CURRLOCATION_FLAG    (1U << 4)
#define MMEINFO_AGEOFLOCATION_FLAG   (1U << 5)

typedef struct t6a_mme_location_info_s {
  uint32_t             set_mask;
  ecgi_t               ecgi;                          // Optional  E Utran Cell Global Identity   3gpp_23.003.h
  tai_t                tai;                           // Optional  Tracking Area Identity         3gpp_23.003.h
  geographical_info_t  geographical_info;             // Optional
  geodetic_info_t      geodetic_info;                 // Optional
  uint32_t             current_location_retrieved;    // Optional
  uint32_t             age_of_location_information;   // Optional
} t6a_mme_location_info_t;

// SGSN Location Info
#define SGSNINFO_CGI_FLAG             (1U)
#define SGSNINFO_LAI_FLAG             (1U << 1)
#define SGSNINFO_SAI_FLAG             (1U << 2)
#define SGSNINFO_RAI_FLAG             (1U << 3)
#define SGSNINFO_GEOGRAPHIC_FLAG      (1U << 4)
#define SGSNINFO_GEODETIC_FLAG        (1U << 5)
#define SGSNINFO_CURRLOCATION_FLAG    (1U << 6)
#define SGSNINFO_AGEOFLOCATION_FLAG   (1U << 7)

typedef struct t6a_sgsn_location_info_s {
  uint32_t             set_mask;
  cgi_t                cgi;                           // Optional   Cell Global Identity    3gpp_23.003.h
  lai_t                lai;                           // Optional   Location Area Identity  3gpp_23.003.h
  sai_t                sai;                           // Optional   Service Area Identity   3gpp_23.003.h
  rai_t                rai;                           // Optional   Routing Area Identity   3gpp_23.003.h
  geographical_info_t  geographical_info;             // Optional
  geodetic_info_t      geodetic_info;                 // Optional
  uint32_t             current_location_retrieved;    // Optional
  uint32_t             age_of_location_information;   // Optional
} t6a_sgsn_location_info_t;

// EPS Location Info
#define EPSINFO_MME_FLAG            (1U)
#define EPSINFO_SGSN_FLAG           (1U << 1)

typedef struct t6a_eps_location_info_s {
  uint32_t                    set_mask;
  t6a_mme_location_info_t     mme_location_info;     // Optional
  t6a_sgsn_location_info_t    sgsn_location_info;    // Optional
} t6a_eps_location_info_t;

#define CAUSETYPE_RADIO_NETWORK_LAYER    (0)
#define CAUSETYPE_TRANSPORT_LAYER        (1)
#define CAUSETYPE_NAS                    (2)
#define CAUSETYPE_PROTOCOL               (3)
#define CAUSETYPE_MISCELLANEOUS          (4)

// Communication Failure Info
#define COMMFAIL_CAUSETYPE_FLAG     (1U)
#define COMMFAIL_S1APCAUSE_FLAG     (1U << 1)
#define COMMFAIL_RANAPCAUSE_FLAG    (1U << 2)
#define COMMFAIL_BSSAPCAUSE_FLAG    (1U << 3)
#define COMMFAIL_GMMCAUSE_FLAG      (1U << 4)
#define COMMFAIL_SMCAUSE_FLAG       (1U << 5)

typedef struct t6a_comm_fail_info_s {
  uint32_t     set_mask;
  uint32_t     cause_type;         // Optional. Use the CAUSETYPE defines above
  uint32_t     s1ap_cause;         // Optional
  uint32_t     ranap_cause;        // Optional
  uint32_t     bssap_cause;        // Optional
  uint32_t     gmm_cause;          // Optional
  uint32_t     sm_cause;           // Optional
} t6a_comm_fail_info_t;

// Service Report
#define SRVCRPT_VENDOR_FLAG        (1U)
#define SRVCRPT_RESULTCODE_FLAG    (1U << 1)
#define SRVCRPT_NODETYPE_FLAG      (1U << 2)

typedef struct t6a_srvc_report_info_s {
  uint32_t   set_mask;
  uint32_t   vendor_id;              // Optional
  uint32_t   service_result_code;    // Optional
  uint32_t   node_type;              // Optional
} t6a_srvc_report_info_t;

// Number UE Per Location Report
#define NUMUERPT_EPSINFO_FLAG        (1U)
#define NUMUERPT_UECOUNT_FLAG        (1U << 1)

typedef struct t6a_num_ue_count_rpt_s {
  t6a_eps_location_info_t    eps_location_info;         // Mandatory
  uint32_t                   ue_count;                  // Mandatory
} t6a_num_ue_count_rpt_t;

#define LOSSOFCONN_UE_DETACHED_MME (0)
#define LOSSOFCONN_UE_DETACHED_SGSN (1)
#define LOSSOFCONN_MAX_DETECTION_TIME_EXPIRED_MME (2)
#define LOSSOFCONN_MAX_DETECTION_TIME_EXPIRED_SGSN (3)
#define LOSSOFCONN_UE_PURGED_MME (4)
#define LOSSOFCONN_UE_PURGED_SGSN (5)

#define MONITORTYPE_LOSS_OF_CONNECTIVITY (0)
#define MONITORTYPE_UE_REACHABILITY (1)
#define MONITORTYPE_LOCATION_REPORTING (2)
#define MONITORTYPE_CHANGE_OF_IMSI_IMEI_ASSOCIATION (3)
#define MONITORTYPE_ROAMING_STATUS (4)
#define MONITORTYPE_COMMUNICATION_FAILURE (5)
#define MONITORTYPE_AVAILABILITY_AFTER_DDN_FAILURE (6)
#define MONITORTYPE_NUMBER_OF_UES_PRESENT_IN_A_GEOGRAPHICAL_AREA (7)

// Monitor Event Report
#define MONEVTRPT_SCEFREF_FLAG             (1U)
#define MONEVTRPT_SCEFID_FLAG              (1U << 1)
#define MONEVTRPT_VISITPLMN_FLAG           (1U << 2)
#define MONEVTRPT_ROAMINFO_FLAG            (1U << 3)
#define MONEVTRPT_IMEICHANGE_FLAG          (1U << 4)
#define MONEVTRPT_REACHINFO_FLAG           (1U << 5)
#define MONEVTRPT_LOSSOFCONN_FLAG          (1U << 6)
#define MONEVTRPT_MONTYPE_FLAG             (1U << 7)
#define MONEVTRPT_EVENTHNDL_FLAG           (1U << 8)
#define MONEVTRPT_EPSINFO_FLAG             (1U << 9)
#define MONEVTRPT_COMMFAIL_FLAG            (1U << 10)
#define MONEVTRPT_SRVCRPT_FLAG             (1U << 11)
#define MONEVTRPT_NUMUERPT_FLAG            (1U << 12)

#define MAX_SRVC_REPORTS                 5
#define MAX_NUM_UE_REPORTS               5

typedef struct t6a_monitor_event_report_s {
  uint32_t                   set_mask;
  uint32_t                   scef_reference_id;                   // Mandatory
  char                       scef_id[HOST_NAME_MAX];              // Optional
  plmn_t                     visited_plmn_id;                     // Optional
  uint32_t                   roaming_information;                 // Optional
  uint32_t                   imei_change;                         // Optional
  uint32_t                   reachability_information;            // Optional
  uint32_t                   loss_of_connectivity_reason;         // Optional. Use the LOSSOFCONN defines above
  uint32_t                   monitoring_type;                     // Optional. Use the MONITORTYPE defines above. 3GPP TS 29.336
  uint32_t                   event_handling;                      // Optional
  t6a_eps_location_info_t    eps_location_info;                   // Optional
  t6a_comm_fail_info_t       comm_fail_info;                      // Optional
  uint32_t                   num_srvc_reports;                    // Mandatory
  t6a_srvc_report_info_t     srvc_report_info[MAX_SRVC_REPORTS];  // Optional
  uint32_t                   num_ue_count_reports;                // Mandatory
  t6a_num_ue_count_rpt_t     ue_count_reports[MAX_NUM_UE_REPORTS];    // Optional
} t6a_monitor_event_report_t;

// Overload Condition - Overload Report
#define OCOLR_SEQNUM_FLAG              (1U)
#define OCOLR_RPTTYPE_FLAG             (1U << 1)
#define OCOLR_REDUCTPCT_FLAG           (1U << 2)
#define OCOLR_VALIDDUR_FLAG            (1U << 3)

typedef struct t6a_oc_olr_s {
  uint32_t                    set_mask;
  uint64_t                    sequence_num;            // Mandatory
  uint32_t                    report_type;             // Mandatory    RFC 7683
  uint32_t                    reduction_pct;           // Optional.    0..100
  uint32_t                    validity_duration;       // Optional
} t6a_oc_olr_t;

// Report Info Request
#define RIR_DRMP_FLAG             (1U)
#define RIR_USERID_FLAG           (1U << 1)
#define RIR_OCFEATURE_FLAG        (1U << 2)
#define RIR_SUPPFEATURE_FLAG      (1U << 3)
#define RIR_MONEVTRPT_FLAG        (1U << 4)

#define MAX_MON_EVT_REPORTS       5

typedef struct t6a_report_info_req_s {
  uint32_t                    set_mask;
  uint32_t                    drmp;                 // Optional. Routing msg priority.  Min value=0.  Max value=15
  t6a_user_identifier_t       user_identifier;      // Optional
  uint64_t                    oc_feature_vector;    // Optional. Diameter Overload Indication Conveyance
  t6a_supported_features_t    supported_features;   // Optional
  uint32_t                    num_monitor_events;   // Mandatory. Specify number of contained monitor events
  t6a_monitor_event_report_t  monitor_event_report[MAX_MON_EVT_REPORTS]; // Optional
} t6a_report_info_req_t;

typedef struct t6a_report_info_ans_s {
  fdiam_result_t        result;              // Result of the update location request procedure
  subscription_data_t subscription_data;   // subscriber status,
  // Maximum Requested Bandwidth Uplink, downlink
  // access restriction data
  // msisdn
  apn_config_profile_t  apn_config_profile;// APN configuration profile

  network_access_mode_t access_mode;
  rau_tau_timer_t       rau_tau_timer;
  char    imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t imsi_length;
} t6a_report_info_ans_t;

// Connection Management Request
#define CMR_DRMP_FLAG                  (1U)
#define CMR_USERID_FLAG                (1U << 1)
#define CMR_BEARERID_FLAG              (1U << 2)
#define CMR_OCFEATURE_FLAG             (1U << 3)
#define CMR_CMRFLAGS_FLAG              (1U << 4)
#define CMR_MAXUEAVAILTIME_FLAG        (1U << 5)
#define CMR_SUPPFEATURE_FLAG           (1U << 6)
#define CMR_CONNACTION_FLAG            (1U << 7)
#define CMR_SRVCSELECTION_FLAG         (1U << 8)
#define CMR_PLMNRATECTRL_FLAG          (1U << 9)
#define CMR_XTENDPCO_FLAG              (1U << 10)
#define CMR_3GPPCHARGING_FLAG          (1U << 11)
#define CMR_RATTYPE_FLAG               (1U << 12)
#define CMR_TERMINFO_FLAG              (1U << 13)
#define CMR_VISITPLMN_FLAG             (1U << 14)

typedef struct t6a_conn_mgmt_req_s {
  uint32_t                          set_mask;
  uint32_t                          drmp;                 // Optional. Routing msg priority.  Min value=0.  Max value=15
  t6a_user_identifier_t             user_identifier;      // Mandatory
  bearer_id_t                       bearer_id;            // Mandatory
  uint64_t                          oc_feature_vector;    // Optional. Diameter Overload Indication Conveyance
  uint32_t                          cmr_flags;            // Optional
  uint32_t                          max_ue_avail_time;    // Optional. Specify in seconds into future
  uint32_t                          num_supp_features;    // Optional
  t6a_supported_features_t          supported_features[MAX_SUPP_FEATURES];   // Optional
  uint32_t                          connection_action;    // Optional.   Min value=0   Max value=2
  char                              service_selection[SRVC_SELECT_MAX];   // Optional
  t6a_serving_plmn_rate_ctrl_t      plmn_rate_ctrl;       // Optional
  protocol_configuration_options_t  extended_pco;         // Optional - 3gpp 24.008 (see section 10.5.6.3)
  char                              charging_chars[CHARGING_CHAR_MAX];    // Optional - 3GPP Charging Characteristics
  rat_type_t                        rat_type;             // Optional
  t6a_terminal_info_t               term_info;            // Optional
  plmn_t                            visited_plmn_id;      // Optional
} t6a_conn_mgmt_req_t;

// Connection Management Answer
#define CMA_DRMP_FLAG                  (1U)
#define CMA_USERID_FLAG                (1U << 1)
#define CMA_BEARERID_FLAG              (1U << 2)
#define CMA_RESULTCODE_FLAG            (1U << 3)
#define CMA_ORIGINHOST_FLAG            (1U << 4)
#define CMA_ORIGINREALM_FLAG           (1U << 5)
#define CMA_OCFEATURE_FLAG             (1U << 6)
#define CMA_OCOLR_FLAG                 (1U << 7)
#define CMA_SUPPFEATURE_FLAG           (1U << 8)
#define CMA_PDNCONNCHARGINGID_FLAG     (1U << 9)
#define CMA_XTENDPCO_FLAG              (1U << 10)

typedef struct t6a_conn_mgmt_ans_s {
  uint32_t                          set_mask;
  uint32_t                          drmp;                 // Optional. Routing msg priority.  Min value=0.  Max value=15
  t6a_user_identifier_t             user_identifier;      // Mandatory
  bearer_id_t                       bearer_id;            // Mandatory
  fdiam_result_t                    result;               // Optional.
  char                              origin_host[HOST_NAME_MAX];   // Mandatory
  char                              origin_realm[REALM_NAME_MAX]; // Mandatory
  uint64_t                          oc_feature_vector;    // Optional. Diameter Overload Indication Conveyance
  t6a_oc_olr_t                      oc_olr;               // Optional - Overload Report
  uint32_t                          num_supp_features;    // Optional
  t6a_supported_features_t          supported_features[MAX_SUPP_FEATURES];   // Optional
  uint32_t                          pdn_conn_charging_id; // Optional
  protocol_configuration_options_t  extended_pco;         // Optional - 3gpp 24.008 (see section 10.5.6.3)
} t6a_conn_mgmt_ans_t;

typedef struct t6a_config_info_req_s {
  char    imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t imsi_length;
  plmn_t  visited_plmn;
  /* Number of vectors to retrieve from HSS, should be equal to one */
  uint8_t nb_of_vectors;

  /* Bit to indicate that USIM has requested a re-synchronization of SQN */
  unsigned re_synchronization:1;
  /* AUTS to provide to AUC.
   * Only present and interpreted if re_synchronization == 1.
   */
  uint8_t resync_param[RAND_LENGTH_OCTETS + AUTS_LENGTH];
} t6a_config_info_req_t;

typedef struct t6a_config_info_ans_s {
  char       imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t    imsi_length;

  /* Result of the authentication information procedure */
  fdiam_result_t result;
  /* Authentication info containing the vector(s) */
  authentication_info_t auth_info;
} t6a_config_info_ans_t;

typedef struct t6a_mo_data_req_s {
  char    imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t imsi_length;
  plmn_t  visited_plmn;
  /* Number of vectors to retrieve from HSS, should be equal to one */
  uint8_t nb_of_vectors;

  /* Bit to indicate that USIM has requested a re-synchronization of SQN */
  unsigned re_synchronization:1;
  /* AUTS to provide to AUC.
   * Only present and interpreted if re_synchronization == 1.
   */
  uint8_t resync_param[RAND_LENGTH_OCTETS + AUTS_LENGTH];
} t6a_mo_data_req_t;

typedef struct t6a_mo_data_ans_s {
  char       imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t    imsi_length;

  /* Result of the authentication information procedure */
  fdiam_result_t result;
  /* Authentication info containing the vector(s) */
  authentication_info_t auth_info;
} t6a_mo_data_ans_t;

typedef struct t6a_mt_data_req_s {
  char    imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t imsi_length;
  plmn_t  visited_plmn;
  /* Number of vectors to retrieve from HSS, should be equal to one */
  uint8_t nb_of_vectors;

  /* Bit to indicate that USIM has requested a re-synchronization of SQN */
  unsigned re_synchronization:1;
  /* AUTS to provide to AUC.
   * Only present and interpreted if re_synchronization == 1.
   */
  uint8_t resync_param[RAND_LENGTH_OCTETS + AUTS_LENGTH];
} t6a_mt_data_req_t;

typedef struct t6a_mt_data_ans_s {
  char       imsi[IMSI_BCD_DIGITS_MAX + 1];
  uint8_t    imsi_length;

  /* Result of the authentication information procedure */
  fdiam_result_t result;
  /* Authentication info containing the vector(s) */
  authentication_info_t auth_info;
} t6a_mt_data_ans_t;

#endif /* FILE_T6A_MESSAGES_TYPES_SEEN */

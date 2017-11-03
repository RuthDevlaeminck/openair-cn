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

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "intertask_interface.h"
#include "t6a_defs.h"
#include "t6a_messages.h"
#include "assertions.h"
#include "log.h"

#define CHECK_FD_FCT(fCT)  DevAssert(fCT == 0);

/*! \file t6a_dict.c
   \brief Initialize t6a dictionnary and setup callbacks for procedures
   \author Sebastien ROUX <sebastien.roux@eurecom.fr>
   \date 2013
   \version 0.1
*/

int
t6a_fd_init_dict_objs (
  void)
{
  struct disp_when                        when;
  vendor_id_t                             vendor_3gpp = VENDOR_3GPP;
  application_id_t                        app_t6a = APP_T6A;

  /*
   * Pre-loading vendor object
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_VENDOR, VENDOR_BY_ID, (void *)&vendor_3gpp, &t6a_fd_cnf.dataobj_t6a_vendor, ENOENT));
  /*
   * Pre-loading application object
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_APPLICATION, APPLICATION_BY_ID, (void *)&app_t6a, &t6a_fd_cnf.dataobj_t6a_app, ENOENT));
  /*
   * Pre-loading commands objects
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Reporting-Information-Request", &t6a_fd_cnf.dataobj_t6a_rir, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Reporting-Information-Answer", &t6a_fd_cnf.dataobj_t6a_ria, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Configuration-Information-Request", &t6a_fd_cnf.dataobj_t6a_cir, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Configuration-Information-Answer", &t6a_fd_cnf.dataobj_t6a_cia, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Connection-Management-Request", &t6a_fd_cnf.dataobj_t6a_cmr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "Connection-Management-Answer", &t6a_fd_cnf.dataobj_t6a_cma, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "MO-Data-Request", &t6a_fd_cnf.dataobj_t6a_odr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "MO-Data-Answer", &t6a_fd_cnf.dataobj_t6a_oda, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "MT-Data-Request", &t6a_fd_cnf.dataobj_t6a_tdr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_COMMAND, CMD_BY_NAME, "MT-Data-Answer", &t6a_fd_cnf.dataobj_t6a_tda, ENOENT));

  /*
   * Pre-loading base avps
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Origin-Host", &t6a_fd_cnf.dataobj_t6a_origin_host, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Origin-Realm", &t6a_fd_cnf.dataobj_t6a_origin_realm, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Host", &t6a_fd_cnf.dataobj_t6a_destination_host, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Realm", &t6a_fd_cnf.dataobj_t6a_destination_realm, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "User-Identifier", &t6a_fd_cnf.dataobj_t6a_user_identifier, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Session-Id", &t6a_fd_cnf.dataobj_t6a_session_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Bearer-Identifier", &t6a_fd_cnf.dataobj_t6a_bearer_identifier, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "DRMP", &t6a_fd_cnf.dataobj_t6a_drmp, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Auth-Session-State", &t6a_fd_cnf.dataobj_t6a_auth_session_state, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Result-Code", &t6a_fd_cnf.dataobj_t6a_result_code, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Experimental-Result", &t6a_fd_cnf.dataobj_t6a_experimental_result, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Supported-Features", &t6a_fd_cnf.dataobj_t6a_supported_features, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "OC-Supported-Features", &t6a_fd_cnf.dataobj_t6a_oc_supported_features, ENOENT));

  /*
   * Pre-loading T6A specifics AVPs
   */
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Monitoring-Event-Configuration", &t6a_fd_cnf.dataobj_t6a_monitoring_event_configuration, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Monitoring-Event-Config-Status", &t6a_fd_cnf.dataobj_t6a_monitoring_event_config_status, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Monitoring-Event-Report", &t6a_fd_cnf.dataobj_t6a_monitoring_event_report, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-OLR", &t6a_fd_cnf.dataobj_t6a_oc_olr, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-Sequence-Number", &t6a_fd_cnf.dataobj_t6a_oc_sequence_number, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-Report-Type", &t6a_fd_cnf.dataobj_t6a_oc_report_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-Reduction-Percentage", &t6a_fd_cnf.dataobj_t6a_oc_reduction_percentage, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-Validity-Duration", &t6a_fd_cnf.dataobj_t6a_oc_validity_duration, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Failed-AVP", &t6a_fd_cnf.dataobj_t6a_failed_avp, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Non-IP-Data", &t6a_fd_cnf.dataobj_t6a_non_ip_data, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "RRC-Cause-Counter", &t6a_fd_cnf.dataobj_t6a_rrc_cause_counter, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "SCEF-Wait-Time", &t6a_fd_cnf.dataobj_t6a_scef_wait_time, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Maximum-Retransmission-Time", &t6a_fd_cnf.dataobj_t6a_maximum_retransmission_time, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Requested-Retransmission-Time", &t6a_fd_cnf.dataobj_t6a_requested_retransmission_time, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "CMR-Flags", &t6a_fd_cnf.dataobj_t6a_cmr_flags, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Maximum-UE-Availability-Time", &t6a_fd_cnf.dataobj_t6a_maximum_ue_availability_time, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Connection-Action", &t6a_fd_cnf.dataobj_t6a_connection_action, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Selection", &t6a_fd_cnf.dataobj_t6a_service_selection, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Serving-PLMN-Rate-Control", &t6a_fd_cnf.dataobj_t6a_serving_plmn_rate_control, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Uplink-Rate-Limit", &t6a_fd_cnf.dataobj_t6a_uplink_rate_limit, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Downlink-Rate-Limit", &t6a_fd_cnf.dataobj_t6a_downlink_rate_limit, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Extended-PCO", &t6a_fd_cnf.dataobj_t6a_extended_pco, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "3GPP-Charging-Characteristics", &t6a_fd_cnf.dataobj_t6a_3gpp_charging_characteristics, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "RAT-Type", &t6a_fd_cnf.dataobj_t6a_rat_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Terminal-Information", &t6a_fd_cnf.dataobj_t6a_terminal_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "IMEI", &t6a_fd_cnf.dataobj_t6a_imei, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "3GPP2_MEID", &t6a_fd_cnf.dataobj_t6a_3gpp2_meid, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Software-Version", &t6a_fd_cnf.dataobj_t6a_software_version, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Visited-PLMN-Id", &t6a_fd_cnf.dataobj_t6a_visited_plmn_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "PDN-Connection-Charging-ID", &t6a_fd_cnf.dataobj_t6a_pdn_connection_charging_id, ENOENT));


  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "MSISDN", &t6a_fd_cnf.dataobj_t6a_msisdn, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "User-Name", &t6a_fd_cnf.dataobj_t6a_user_name, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "External-Identifier", &t6a_fd_cnf.dataobj_t6a_external_identifier, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "LMSI", &t6a_fd_cnf.dataobj_t6a_lmsi, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "OC-Feature-Vector", &t6a_fd_cnf.dataobj_t6a_oc_feature_vector, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Vendor-Id", &t6a_fd_cnf.dataobj_t6a_vendor_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Feature-List-Id", &t6a_fd_cnf.dataobj_t6a_feature_list_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Feature-List", &t6a_fd_cnf.dataobj_t6a_feature_list, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "SCEF-Reference-Id", &t6a_fd_cnf.dataobj_t6a_scef_reference_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "SCEF-Id", &t6a_fd_cnf.dataobj_t6a_scef_id, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Roaming-Information", &t6a_fd_cnf.dataobj_t6a_roaming_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "IMEI-Change", &t6a_fd_cnf.dataobj_t6a_imei_change, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Reachability-Information", &t6a_fd_cnf.dataobj_t6a_reachability_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Loss-Of-Connectivity-Reason", &t6a_fd_cnf.dataobj_t6a_loss_of_connectivity_reason, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "EPS-Location-Information", &t6a_fd_cnf.dataobj_t6a_eps_location_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "MME-Location-Information", &t6a_fd_cnf.dataobj_t6a_mme_location_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "SGSN-Location-Information", &t6a_fd_cnf.dataobj_t6a_sgsn_location_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "E-UTRAN-Cell-Global-Identity", &t6a_fd_cnf.dataobj_t6a_e_utran_cell_global_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Tracking-Area-Identity", &t6a_fd_cnf.dataobj_t6a_tracking_area_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Geographical-Information", &t6a_fd_cnf.dataobj_t6a_geographical_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Geodetic-Information", &t6a_fd_cnf.dataobj_t6a_geodetic_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Current-Location-Retrieved", &t6a_fd_cnf.dataobj_t6a_current_location_retrieved, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Age-Of-Location-Information", &t6a_fd_cnf.dataobj_t6a_age_of_location_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Cell-Global-Identity", &t6a_fd_cnf.dataobj_t6a_cell_global_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Location-Area-Identity", &t6a_fd_cnf.dataobj_t6a_location_area_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Area-Identity", &t6a_fd_cnf.dataobj_t6a_service_area_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Routing-Area-Identity", &t6a_fd_cnf.dataobj_t6a_routing_area_identity, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Monitoring-Type", &t6a_fd_cnf.dataobj_t6a_monitoring_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Event-Handling", &t6a_fd_cnf.dataobj_t6a_event_handling, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Report", &t6a_fd_cnf.dataobj_t6a_service_report, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Result", &t6a_fd_cnf.dataobj_t6a_service_result, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Service-Result-Code", &t6a_fd_cnf.dataobj_t6a_service_result_code, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Node-Type", &t6a_fd_cnf.dataobj_t6a_node_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Communication-Failure-Information", &t6a_fd_cnf.dataobj_t6a_communication_failure_information, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Cause-Type", &t6a_fd_cnf.dataobj_t6a_cause_type, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "S1AP-Cause", &t6a_fd_cnf.dataobj_t6a_s1ap_cause, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "RANAP-Cause", &t6a_fd_cnf.dataobj_t6a_ranap_cause, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "BSSAP-Cause", &t6a_fd_cnf.dataobj_t6a_bssap_cause, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "GMM-Cause", &t6a_fd_cnf.dataobj_t6a_gmm_cause, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "SM-Cause", &t6a_fd_cnf.dataobj_t6a_sm_cause, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "Number-Of-UE-Per-Location-Report", &t6a_fd_cnf.dataobj_t6a_number_of_ue_per_location_report, ENOENT));
  CHECK_FD_FCT (fd_dict_search (fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME_ALL_VENDORS, "UE-Count", &t6a_fd_cnf.dataobj_t6a_ue_count, ENOENT));

  /*
   * Register callbacks
   */
  memset (&when, 0, sizeof (when));

  /*
   * Register the callback for Reporting Information Answer T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_ria;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_ria_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.ria_hdl));
  DevAssert (t6a_fd_cnf.ria_hdl);

  /*
   * Register the callback for Configuration Information Request T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_cir;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_cir_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.cir_hdl));
  DevAssert (t6a_fd_cnf.cir_hdl);

  /*
   * Register the callback for Connection Management Request T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_cmr;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_cmr_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.cmr_hdl));
  DevAssert (t6a_fd_cnf.cmr_hdl);

  /*
   * Register the callback for Connection Management Answer T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_cma;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_cma_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.cma_hdl));
  DevAssert (t6a_fd_cnf.cma_hdl);

  /*
   * Register the callback for MO Data Answer T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_oda;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_oda_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.oda_hdl));
  DevAssert (t6a_fd_cnf.oda_hdl);

  /*
   * Register the callback for MT Data Request T6A Application
   */
  when.command = t6a_fd_cnf.dataobj_t6a_tdr;
  when.app = t6a_fd_cnf.dataobj_t6a_app;
  CHECK_FD_FCT (fd_disp_register (t6a_tdr_cb, DISP_HOW_CC, &when, NULL, &t6a_fd_cnf.tdr_hdl));
  DevAssert (t6a_fd_cnf.tdr_hdl);

  /*
   * Advertise the support for the test application in the peer
   */
  CHECK_FD_FCT (fd_disp_app_support (t6a_fd_cnf.dataobj_t6a_app, t6a_fd_cnf.dataobj_t6a_vendor, 1, 0));
  return RETURNok;
}

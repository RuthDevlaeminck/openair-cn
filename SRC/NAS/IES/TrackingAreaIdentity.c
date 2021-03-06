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
#include <stdlib.h>
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "TrackingAreaIdentity.h"

int
decode_tracking_area_identity (
  TrackingAreaIdentity * trackingareaidentity,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  int                                     decoded = 0;

  if (iei > 0) {
    CHECK_IEI_DECODER (iei, *buffer);
    decoded++;
  }

  trackingareaidentity->mccdigit2 = (*(buffer + decoded) >> 4) & 0xf;
  trackingareaidentity->mccdigit1 = *(buffer + decoded) & 0xf;
  decoded++;
  trackingareaidentity->mncdigit3 = (*(buffer + decoded) >> 4) & 0xf;
  trackingareaidentity->mccdigit3 = *(buffer + decoded) & 0xf;
  decoded++;
  trackingareaidentity->mncdigit2 = (*(buffer + decoded) >> 4) & 0xf;
  trackingareaidentity->mncdigit1 = *(buffer + decoded) & 0xf;
  decoded++;
  //IES_DECODE_U16(trackingareaidentity->tac, *(buffer + decoded));
  IES_DECODE_U16 (buffer, decoded, trackingareaidentity->tac);
#if NAS_DEBUG
  dump_tracking_area_identity_xml (trackingareaidentity, iei);
#endif
  return decoded;
}

int
encode_tracking_area_identity (
  TrackingAreaIdentity * trackingareaidentity,
  uint8_t iei,
  uint8_t * buffer,
  uint32_t len)
{
  uint32_t                                encoded = 0;

  /*
   * Checking IEI and pointer
   */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER (buffer, TRACKING_AREA_IDENTITY_MINIMUM_LENGTH, len);
#if NAS_DEBUG
  dump_tracking_area_identity_xml (trackingareaidentity, iei);
#endif

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  *(buffer + encoded) = 0x00 | ((trackingareaidentity->mccdigit2 & 0xf) << 4) | (trackingareaidentity->mccdigit1 & 0xf);
  encoded++;
  *(buffer + encoded) = 0x00 | ((trackingareaidentity->mncdigit3 & 0xf) << 4) | (trackingareaidentity->mccdigit3 & 0xf);
  encoded++;
  *(buffer + encoded) = 0x00 | ((trackingareaidentity->mncdigit2 & 0xf) << 4) | (trackingareaidentity->mncdigit1 & 0xf);
  encoded++;
  IES_ENCODE_U16 (buffer, encoded, trackingareaidentity->tac);
  return encoded;
}

void
dump_tracking_area_identity_xml (
  TrackingAreaIdentity * trackingareaidentity,
  uint8_t iei)
{
  OAILOG_DEBUG (LOG_NAS, "<Tracking Area Identity>\n");

  if (iei > 0)
    /*
     * Don't display IEI if = 0
     */
    OAILOG_DEBUG (LOG_NAS, "    <IEI>0x%X</IEI>\n", iei);

  OAILOG_DEBUG (LOG_NAS, "    <MCC digit 2>%u</MCC digit 2>\n", trackingareaidentity->mccdigit2);
  OAILOG_DEBUG (LOG_NAS, "    <MCC digit 1>%u</MCC digit 1>\n", trackingareaidentity->mccdigit1);
  OAILOG_DEBUG (LOG_NAS, "    <MNC digit 3>%u</MNC digit 3>\n", trackingareaidentity->mncdigit3);
  OAILOG_DEBUG (LOG_NAS, "    <MCC digit 3>%u</MCC digit 3>\n", trackingareaidentity->mccdigit3);
  OAILOG_DEBUG (LOG_NAS, "    <MNC digit 2>%u</MNC digit 2>\n", trackingareaidentity->mncdigit2);
  OAILOG_DEBUG (LOG_NAS, "    <MNC digit 1>%u</MNC digit 1>\n", trackingareaidentity->mncdigit1);
  OAILOG_DEBUG (LOG_NAS, "    <TAC>0x%.4x</TAC>\n", trackingareaidentity->tac);
  OAILOG_DEBUG (LOG_NAS, "</Tracking Area Identity>\n");
}

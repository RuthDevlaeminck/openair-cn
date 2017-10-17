#!/bin/bash

OPENAIRCN_DIR=$(dirname $(dirname $(readlink -f $0)))
SERVICE_TEMPLATE=$OPENAIRCN_DIR/ETC/oai.service
SERVICE_DIR=/etc/systemd/system


function install_service_file()
{
  SERVICE=$1
  SERVICE_FILE=$SERVICE_DIR/$SERVICE.service

  cp $SERVICE_TEMPLATE $SERVICE_FILE
  sed -i -e "s:%OPENAIRCN_DIR%:$OPENAIRCN_DIR:g" $SERVICE_FILE 
  sed -i -e "s:%SERVICE%:$SERVICE:g" $SERVICE_FILE 

  systemctl enable $SERVICE
}

install_service_file "hss"
install_service_file "spgw"
install_service_file "mme"

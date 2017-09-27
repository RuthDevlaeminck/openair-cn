#!/bin/bash

THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/../BUILD/TOOLS/build_helper

function help()
{
  echo_error " "
  echo_error "Usage: prereq.sh <mysql root password> <hss db password>..."
}

if [ "$#" -ne 2 ]; then
  help
  exit
fi

# and since we don't want to have to enter the gui for phpmyadmin
echo "phpmyadmin phpmyadmin/dbconfig-install boolean true" | debconf-set-selections
echo "phpmyadmin phpmyadmin/app-password-confirm password $2" | debconf-set-selections
#echo "phpmyadmin phpmyadmin/mysql/admin-pass password $1" | debconf-set-selections
echo "phpmyadmin phpmyadmin/mysql/app-pass password $2" | debconf-set-selections
echo "phpmyadmin phpmyadmin/reconfigure-webserver multiselect apache2" | debconf-set-selections

export  DEBIAN_FRONTEND=noninteractive
check_install_software 

mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '$1'"

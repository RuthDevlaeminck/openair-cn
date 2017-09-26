#!/bin/bash

THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/../BUILD/TOOLS/build_helper

export  DEBIAN_FRONTEND=noninteractive
check_install_software 



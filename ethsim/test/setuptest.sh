#!/bin/bash

if [ $# != 2 ]; then
    echo "Usage: ./setuptest.sh <virt-ifname> <username>"
    echo "This script should be run by root"
    exit
fi

IFNAME=$1
USERID=`id -u $2`

# check if interface exists and create it if not
ifconfig $IFNAME &> /dev/null
if [ $? != 0 ]; then
    echo "Creating interface..."
    LD_LIBRARY_PATH=../../tap ../../tap/createtap $IFNAME $USERID
fi

TUNNEL_SERVER=192.168.1.1

# configure it with ip address
ifconfig $IFNAME $TUNNEL_SERVER/24

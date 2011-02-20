#!/bin/bash

if [ $# != 1 ]; then
    echo "Usage: ./runtest.sh <virt-ifname>"
    exit
fi

# if you change the parameters, update testcomm.py and app.c accordingly
IFNAME=$1
SIM_USB_PORT=9999
SIM_ETH_PORT=8888
SIM_SERVER_ETH_PORT=6666
TUNNEL_PORT=7777
TUNNEL_SERVER=192.168.1.1
TUNNEL_CLIENT=192.168.1.2

export PYTHONPATH=../../tap
export LD_LIBRARY_PATH=../../tap

# Run the appplication and the needed services in background
../ethapp $SIM_ETH_PORT $SIM_USB_PORT &> /dev/null &
python stackfeeder.py $SIM_ETH_PORT $IFNAME &
python virtclient.py $TUNNEL_SERVER $TUNNEL_CLIENT $TUNNEL_PORT $SIM_SERVER_ETH_PORT &

# run the testcase
python testcomm.py

# Now it's important to terminate all processes in such way that no open sockets
# will be left, otherwise this test can't be ran again during few minutes...

# When testcomm.py finishes, it closes its connections
# which in turn causes virtclient.py to terminate.
# So just wait a bit for it
sleep 1

# Now we'll kill a stackfeeder.py which is client of ./ethapp
# This will close its sockets and cause ./ethapp to close its own sockets.
kill %2
sleep 1

# Now it's safe to terminate ./ethapp
kill %1

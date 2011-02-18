import socket
import sys
import os
import select
import opentap

MAX_PACKET_SIZE = 2000

def main(argv):
    if len(argv) != 5:
        print >> sys.stderr, "Usage: ./vpnserv <my-addr> <client-addr> <port> <tapdev>"
        sys.exit(1)
    
    my_addr = argv[1]
    client_addr = argv[2]
    vpnport = int(argv[3])
    tapdevname = argv[4]
    
    tapdev = opentap.opentap(tapdevname)
    
    vpnsock = socket.socket(type = socket.SOCK_DGRAM)
    vpnsock.bind((my_addr, vpnport))
    
    while True:
        ready_fd = select.select([tapdev, vpnsock], [], [])[0]
        
        if tapdev in ready_fd:
            pkt = os.read(tapdev, MAX_PACKET_SIZE)
            vpnsock.sendto(pkt, (client_addr, vpnport))
        
        if vpnsock in ready_fd:
            pkt, remoteaddr = vpnsock.recvfrom(MAX_PACKET_SIZE)
            os.write(tapdev, pkt)
    

if __name__ == "__main__":
    main(sys.argv)

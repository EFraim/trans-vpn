import socket
import sys
import os
import select
import packetchannel

MAX_PACKET_SIZE = 2000

def main(argv):
    if len(argv) != 5:
        print >> sys.stderr, "Usage: ./stackfeeder <virt-serv> <virt-client> <virt-udp-port> <chan-port>"
        sys.exit(1)
    
    virt_server = argv[1]
    virt_client = argv[2]
    virt_port = int(argv[3])
    chan_port = int(argv[4])
    
    chan = packetchannel.accept('localhost', chan_port)
    chan_fd = chan.fd()
    data_sock = socket.socket(type = socket.SOCK_DGRAM)
    data_sock.bind((virt_server, virt_port))
    
    try:
        while True:
            ready_fd = select.select([chan_fd, data_sock], [], [])[0]
            
            if chan_fd in ready_fd:
                pkt = chan.recv()
                data_sock.sendto(pkt, (virt_client, virt_port))
            
            if data_sock in ready_fd:
                pkt, addr = data_sock.recvfrom(MAX_PACKET_SIZE)
                chan.send(pkt)
    except packetchannel.ChannelClosedException, e:
        pass

if __name__ == "__main__":
    main(sys.argv)

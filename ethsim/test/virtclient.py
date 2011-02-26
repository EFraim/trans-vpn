import socket
import sys
import os
import select
import packetchannel

MAX_PACKET_SIZE = 2000

def main(argv):
    if len(argv) != 6:
        print >> sys.stderr, "Usage: ./stackfeeder <virt-serv> <virt-client> <virt-tcp-port> <chan-port-my> <chan-port-other>"
        sys.exit(1)
    
    virt_server = argv[1]
    virt_client = argv[2]
    virt_port = int(argv[3])
    my_chan_port = int(argv[4])
    other_chan_port = int(argv[5])
    
    chan = packetchannel.UDPChannel('localhost', my_chan_port, 'localhost', other_chan_port)
    chan_fd = chan.fd()

    data_chan = packetchannel.accept(virt_server, virt_port, 2)
    data_fd = data_chan.fd()
    
    print "vpn connection accepted"
    
    try:
        while True:
            ready_fd = select.select([chan_fd, data_fd], [], [])[0]
            
            if chan_fd in ready_fd:
                pkt = chan.recv()
                data_chan.send(pkt)
            
            if data_fd in ready_fd:
                pkt = data_chan.recv()
                chan.send(pkt)
                
    except packetchannel.ChannelClosedException, e:
        pass
    except KeyboardInterrupt:
        pass
    
    data_chan.close()
    chan.close()
    
if __name__ == "__main__":
    main(sys.argv)

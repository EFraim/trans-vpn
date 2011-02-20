import sys
import os
import select
import packetchannel
import opentap

MAX_PACKET_SIZE = 2000

def main(argv):
    if len(argv) != 3:
        print >> sys.stderr, "Usage: ./stackfeeder <phys-port> <virt-dev>"
        sys.exit(1)
    
    port = int(argv[1])
    devname =  argv[2]
    
    phys = packetchannel.connect("localhost", int(port))
    phys_fd = phys.fd()
    dev = opentap.opentap(devname)
    
    try:
        while True:
            ready_fds = select.select([phys_fd, dev], [], [])[0]
            
            if dev in ready_fds:
                pkt = os.read(dev, MAX_PACKET_SIZE)
                phys.send(pkt)
            
            if phys_fd in ready_fds:
                pkt = phys.recv()
                os.write(dev, pkt)
    except packetchannel.ChannelClosedException, e:
        pass


if __name__ == "__main__":
    main(sys.argv)

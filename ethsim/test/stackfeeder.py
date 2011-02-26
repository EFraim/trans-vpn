import sys
import os
import select
import packetchannel
import opentap

MAX_PACKET_SIZE = 2000

def main(argv):
    if len(argv) != 4:
        print >> sys.stderr, "Usage: ./stackfeeder <phys-port-my> <phys-port-other> <virt-dev>"
        sys.exit(1)
    
    myport = int(argv[1])
    otherport = int(argv[2])
    devname =  argv[3]
    
    phys = packetchannel.UDPChannel("localhost", int(myport), "localhost", int(otherport))
    dev = opentap.opentap(devname)
    
    try:
        while True:
            ready_fds = select.select([phys, dev], [], [])[0]
            
            if dev in ready_fds:
                pkt = os.read(dev, MAX_PACKET_SIZE)
                phys.send(pkt)
            
            if phys in ready_fds:
                pkt = phys.recv()
                os.write(dev, pkt)

    except KeyboardInterrupt:
        phys.close()


if __name__ == "__main__":
    main(sys.argv)

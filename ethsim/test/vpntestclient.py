import sys
import packetchannel
import select

def main(argv):
    if len(argv) != 5:
        print >> sys.stderr, "Usage: ./vpntestclient.py <serv-addr> <serv-port> <chan-port-my> <chan-port-other>"
        sys.exit(1)
        
    serv_addr = argv[1]
    serv_port = int(argv[2])
    myport = int(argv[3])
    otherport = int(argv[4])
    
    serv_chan = packetchannel.connect(serv_addr, serv_port, 2)
    data_chan = packetchannel.UDPChannel("localhost", myport, "localhost", otherport)
    
    try:
        while True:
            ready_fds = select.select([serv_chan, data_chan], [], [])[0]
            
            if serv_chan in ready_fds:
                pkt = serv_chan.recv()
                data_chan.send(pkt)
            
            if data_chan in ready_fds:
                pkt = data_chan.recv()
                serv_chan.send(pkt)
    
    except:
        serv_chan.close()
        data_chan.close()

if __name__ == "__main__":
    main(sys.argv)

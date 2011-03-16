import sys
import packetchannel
import select

def read_secure_id(filename):
    f = open(filename, 'r')
    data = f.read().splitlines()
    return tuple(data)

def main(argv):
    if len(argv) != 6:
        print >> sys.stderr, "Usage: ./vpntestclient.py <serv-addr> <serv-port> <chan-port-my> <chan-port-other> <secure-id-filename>"
        sys.exit(1)
        
    serv_addr = argv[1]
    serv_port = int(argv[2])
    myport = int(argv[3])
    otherport = int(argv[4])
    sec_filename = argv[5]
    
    #serv_chan = packetchannel.TCPChannel()
    serv_chan = packetchannel.SecureChannel()

    id, pub_mod, pub_key, priv_key = read_secure_id(sec_filename)
    secure_id = packetchannel.SecureId(id, pub_mod, pub_key, priv_key)
    
    serv_chan.connect(serv_addr, serv_port, secure_id)
    data_chan = packetchannel.UDPChannel("localhost", myport, "localhost", otherport)
    
    try:
        while True:
            ready_chans = packetchannel.select([serv_chan, data_chan])
            
            if serv_chan in ready_chans:
                pkt = serv_chan.recv()
                data_chan.send(pkt)
            
            if data_chan in ready_chans:
                pkt = data_chan.recv()
                serv_chan.send(pkt)
    
    except:
        serv_chan.close()
        data_chan.close()

if __name__ == "__main__":
    main(sys.argv)

import socket
import sys
import os
import select
import packetchannel

MAX_PACKET_SIZE = 2000

def read_secure_db(filename):
    f = open(filename, 'r')
    data = f.read().splitlines()
    return [tuple(data[i:i+4]) for i in range(0, len(data), 4)]

class RoutingUnit:
    def __init__(self):
        self._clients = set()
    
    def attach_client(self, c):
        self._clients.add(c)
        
    def detach_client(self, c):
        self._clients.remove(c)
    
    def route_packet(self, src_client, src_addr, dst_addr):
        """
        Returns list of destination sockets
        """
        raise NotImplementedError("route_packet")

class HubUnit(RoutingUnit):
    def route_packet(self, src_client, src_addr, dst_addr):
        return list(set.difference(self._clients, [src_client]))


def main(argv):
    if len(argv) != 4:
        print >> sys.stderr, "Usage: ./vpnserver.py <serv-addr> <serv-port> <sec-db-filename>"
        sys.exit(1)

    serv_addr = argv[1]
    serv_port = int(argv[2])
    secure_db_filename = argv[3]
    
    secure_db_data = read_secure_db(secure_db_filename)
    secure_db = {}
    for secure_id in secure_db_data:
        secure_db[int(secure_id[0])] = packetchannel.SecureId(*secure_id)
    
    server = packetchannel.SecureChannelServer(serv_addr, serv_port, secure_db)
    #server = packetchannel.TCPChannelServer(serv_addr, serv_port)
    
    clients = set()
    
    routing_unit = HubUnit()
    
    try:
        while True:
            ready_comms = packetchannel.select(list(clients) + [server])
            
            for c in ready_comms:
                if c == server:
                    print "new vpn connection accepted!"
                    client = server.accept()
                    clients.add(client)
                    routing_unit.attach_client(client)
                else:
                    try:
                        pkt = c.recv()
                        dst = routing_unit.route_packet(c, 0, 0)
                        for d in dst:
                            d.send(pkt)
                    except packetchannel.ChannelClosedException, e:
                        print "vpn connection closed!"
                        routing_unit.detach_client(c)
                        clients.remove(c)
                        c.close()
                
    except KeyboardInterrupt:
        pass
    
    server.close()
    for c in clients:
        c.close()
    
if __name__ == "__main__":
    main(sys.argv)

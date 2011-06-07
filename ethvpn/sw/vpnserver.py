import socket
import sys
import os
import select
import packetchannel
import rsaid

MAX_PACKET_SIZE = 2000

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
    if len(argv) != 5:
        print >> sys.stderr, "Usage: ./vpnserver.py <serv-addr> <serv-port> <server-id-filename> <clients-id-filename>"
        sys.exit(1)

    serv_addr = argv[1]
    serv_port = int(argv[2])
    server_id_filename = argv[3]
    client_id_filenames = argv[4]
    
    server_id = rsaid.from_file(server_id_filename)
    client_ids = set(rsaid.from_file(client_id_filenames))
        
    server = packetchannel.SecureChannelServer(serv_addr, serv_port, server_id, client_ids)
    
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

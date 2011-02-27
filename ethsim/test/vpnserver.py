import socket
import sys
import os
import select
import packetchannel

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
    if len(argv) != 3:
        print >> sys.stderr, "Usage: ./vpnserver.py <serv-addr> <serv-port>"
        sys.exit(1)

    serv_addr = argv[1]
    serv_port = int(argv[2])

    server = socket.socket()
    server.bind((serv_addr, serv_port))
    server.listen(1)
    
    clients = set()
    
    routing_unit = HubUnit()
    
    try:
        while True:
            ready_socks = select.select(list(clients) + [server], [], [])[0]
            
            for s in ready_socks:
                if s == server:
                    print "new vpn connection accepted!"
                    client = packetchannel.TCPChannel(s.accept()[0], 2)
                    clients.add(client)
                    routing_unit.attach_client(client)
                else:
                    try:
                        pkt = s.recv()
                        dst = routing_unit.route_packet(s, 0, 0)
                        for d in dst:
                            d.send(pkt)
                    except packetchannel.ChannelClosedException, e:
                        print "vpn connection closed!"
                        routing_unit.detach_client(s)
                        clients.remove(s)
                        s.close()
                
    except KeyboardInterrupt:
        pass
    
    server.close()
    for c in clients:
        c.close()
    
if __name__ == "__main__":
    main(sys.argv)

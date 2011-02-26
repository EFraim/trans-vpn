import socket
import struct
import select

class ChannelClosedException(Exception):
    pass

class TCPChannel:
    def __init__(self, sock, sizelen = 4):
        """
        sock 	 - socket object
        sizetype - length of the frame size field
        """
        self._socket = sock
        self._poll = select.poll()
        self._poll.register(self._socket)
        self._recvlen = None
        self._recvframe = ""
        self._sizelen = sizelen
        self._sizetype = {2: 'h', 4: 'i'}[sizelen]
    
    def send(self, packet):
        self._socket.send(struct.pack(self._sizetype, len(packet)) + packet)
    
    def recv(self):
        if self._poll.poll()[0][1] & select.POLLIN:
            if self._recvlen == None:
                data = self._socket.recv(self._sizelen)
                if len(data) == 0:
                    raise ChannelClosedException()
                self._recvlen = struct.unpack(self._sizetype, data)[0]
                self._recvframe = ""
                payload_ready = self._poll.poll()
            else:
                payload_ready = True
            
            if payload_ready:
                bytes_remaining = self._recvlen - len(self._recvframe)

                data = self._socket.recv(bytes_remaining)
                if len(data) == 0:
                    raise ChannelClosedException()
                
                self._recvframe += data
            
            if len(self._recvframe) == self._recvlen:
                self._recvlen = None
                return self._recvframe
                
        return None

    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
        
    def fileno(self):
        return self._socket.fileno()

class UDPChannel:
    def __init__(self, myaddress, myport, otheraddress, otherport):
        self._socket = socket.socket(type = socket.SOCK_DGRAM)
        self._socket.bind((myaddress, myport))
        self._poll = select.poll()
        self._poll.register(self._socket)
        self._other = (otheraddress, otherport)

    def send(self, packet):
        self._socket.sendto(packet, (self._other))
    
    def recv(self):
        if self._poll.poll()[0][1] & select.POLLIN:
            return self._socket.recvfrom(2000)[0]
        
        return None
        
    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
        
    def fileno(self):
        return self._socket.fileno()

def connect(server, port, *args):
    s = socket.socket()
    s.connect((server, port))
    return TCPChannel(s, *args)

def accept(address, port, *args):
    s = socket.socket()
    s.bind((address, port))
    s.listen(1)
    conn = s.accept()[0]
    s.close()
    return TCPChannel(conn, *args)


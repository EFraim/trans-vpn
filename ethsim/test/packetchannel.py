import socket
import struct
import select

class ChannelClosedException(Exception):
    pass

class PacketChannel:
    def __init__(self, sock):
        self._socket = sock
        self._poll = select.poll()
        self._poll.register(self._socket)
        self._recvlen = None
        self._recvframe = ""
    
    def send(self, packet):
        self._socket.send(struct.pack("i", len(packet)) + packet)
    
    def recv(self):
        if self._poll.poll()[0][1] & select.POLLIN:
            if self._recvlen == None:
                data = self._socket.recv(4)
                if len(data) == 0:
                    self.close()
                    raise ChannelClosedException()
                self._recvlen = struct.unpack("i", data)[0]
                self._recvframe = ""
                payload_ready = self._poll.poll()
            else:
                payload_ready = True
            
            if payload_ready:
                bytes_remaining = self._recvlen - len(self._recvframe)

                data = self._socket.recv(bytes_remaining)
                if len(data) == 0:
                    self.close()
                    raise ChannelClosedException()
                
                self._recvframe += data
            
            if len(self._recvframe) == self._recvlen:
                self._recvlen = None
                return self._recvframe
                
        return None

    def close(self):
        self._socket.close()
        
    def fd(self):
        return self._socket.fileno()


def connect(server, port):
    s = socket.socket()
    s.connect((server, port))
    return PacketChannel(s)

def accept(address, port):
    s = socket.socket()
    s.bind((address, port))
    s.listen(1)
    conn = s.accept()[0]
    s.close()
    return PacketChannel(conn)

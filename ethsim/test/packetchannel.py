import socket
import struct
import select as cselect
import operator
import polarssl

def poll_read(p):
    l = p.poll(0)
    return (len(l) > 0) and (l[0][1] & cselect.POLLIN)

class ChannelClosedException(Exception):
    pass

class Task:
    """
    """
    
    def fileno(self):
        """
        Returns file descriptor that is used to wait for next processing
        """
        pass
    
    def process(self):
        """
        Performs some processing
        """
        pass

class Communication:
    def close(self):
        pass
    
    def tasks(self):
        pass
    
    def poll(self):
        pass
    

class Channel(Communication):
    
    def send(self, packet):
        pass
    
    def recv(self):
        pass

class Server(Communication):
    def accept(self):
        pass


class UDPChannel(Channel, Task):
    def __init__(self, myaddress, myport, otheraddress, otherport):
        self._socket = socket.socket(type = socket.SOCK_DGRAM)
        self._socket.bind((myaddress, myport))
        self._poll = cselect.poll()
        self._poll.register(self._socket)
        self._other = (otheraddress, otherport)

    def send(self, packet):
        self._socket.sendto(packet, (self._other))
    
    def recv(self):
        if UDPChannel.poll(self):
            return self._socket.recvfrom(2000)[0]
        
        return None
        
    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
        
    def fileno(self):
        return self._socket.fileno()
        
    def process(self):
        pass
    
    def poll(self):
        return poll_read(self._poll)
        
    def tasks(self):
        return [self]


class TCPChannel(Channel, Task):
    
    PACKET_EMPTY = 0
    PACKET_LEN_RCV = 1
    PACKET_FULL = 2

    def from_socket(self, sock):
        self._socket = sock
        self._poll = cselect.poll()
        self._poll.register(self._socket)
        self._packetstate = TCPChannel.PACKET_EMPTY
        self._packetlen = None
        self._packet = ""
        self._isclosed = False
        
    def connect(self, address, port):
        sock = socket.socket()
        sock.connect((address, port))
        self.from_socket(sock)

    def send(self, packet):
        self._socket.send(struct.pack("h", len(packet)) + packet)
    
    def poll(self):
        return self._packetstate == TCPChannel.PACKET_FULL or self._isclosed
    
    def _poll_socket(self):
        return poll_read(self._poll)

    def recv(self):
        TCPChannel.process(self)
        if self._packetstate == TCPChannel.PACKET_FULL:
            self._packetstate = TCPChannel.PACKET_EMPTY
            return self._packet
        elif self._isclosed:
            raise ChannelClosedException()
        
        return None

    def process(self):
        try:
            if self._poll_socket() and self._packetstate != TCPChannel.PACKET_FULL:
                if self._packetstate == TCPChannel.PACKET_EMPTY:
                    data = self._socket.recv(2)
                    if len(data) == 0:
                        self._isclosed = True
                        return
                    self._packetlen = struct.unpack("h", data)[0]
                    self._packet = ""
                    payload_ready = self._poll_socket()
                else: # self._packetstate == TCPChannel.PACKET_LEN_RCV:
                    payload_ready = True
                
                if payload_ready:
                    bytes_remaining = self._packetlen - len(self._packet)

                    data = self._socket.recv(bytes_remaining)
                    if len(data) == 0:
                        self._isclosed = True
                        return
                    
                    self._packet += data
                
                if len(self._packet) == self._packetlen:
                    self._packetstate = TCPChannel.PACKET_FULL
        except socket.error, e:
            self._isclosed = True

    def fileno(self):
        return self._socket.fileno()

    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
        
    def tasks(self):
        return [self]


class TCPChannelServer(Server, Task):
    def __init__(self, address, port):
        self._socket = socket.socket()
        self._socket.bind((address, port))
        self._socket.listen(1)
        self._poll = cselect.poll()
        self._poll.register(self._socket)

        
    def accept(self, ChannelClass = TCPChannel):
        if TCPChannelServer.poll(self):
            client = ChannelClass()
            client.from_socket(self._socket.accept()[0])
            return client
        return None
    
    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
    
    def poll(self):
        return poll_read(self._poll)
    
    def tasks(self):
        return [self]
    
    def fileno(self):
        return self._socket.fileno()
        
    def process(self):
        pass
    

def select(comms):

    ready_comms = []
    while len(ready_comms) == 0:
        tasks = reduce(operator.add, [comm.tasks() for comm in comms])
        ready_tasks = cselect.select(tasks, [], [])[0]
        for task in ready_tasks:
            task.process()
        ready_comms = filter(lambda comm: comm.poll(), comms)

    return ready_comms

def pad_string(string, length, char = '\x00'):
    return string + char * (length - len(string))

class SecureChannel(TCPChannel):
    
    def connect(self, address, port, secure_id):
        TCPChannel.connect(self, address, port)
        
        #TCPChannel.send(self, "hello")
        rsa = polarssl.RSA()
        rsa.set_public_modulus(secure_id.public_modulus)
        rsa.set_private_key(secure_id.private_key)
        cert = pad_string(secure_id.public_modulus, 256) + \
               pad_string(secure_id.public_key, 256) + \
               rsa.encrypt_private(struct.pack("i", secure_id.id))
               
        #open("/tmp/1", "w").write(cert)

        TCPChannel.send(self, cert)
        
        select([self])
        res = TCPChannel.recv(self)
        if res[0] != "y":
            raise RuntimeError("Failed to connect: %s" % res)

class AuthSecureChannel(TCPChannel):
    STATE_UNKNOWN      = 0
    STATE_IDENTIFIED   = 1
    STATE_UNIDENTIFIED = 2
    
    def __init__(self):
        self._state = AuthSecureChannel.STATE_UNKNOWN
    
    def process(self):
        TCPChannel.process(self)
        if self._state == AuthSecureChannel.STATE_UNKNOWN:
            if TCPChannel.poll(self) and not self._isclosed:
                cert = TCPChannel.recv(self)
                
                #if cert == "hello":
                    #self._state = AuthSecureChannel.STATE_IDENTIFIED
                    #self.send("y")
                    #return
                
                pub_mod = cert[:256].strip("\x00")
                pub_key = cert[256:512].strip("\x00")
                enc_id  = cert[512:640]
                rsa = polarssl.RSA()
                rsa.set_public_modulus(pub_mod)
                rsa.set_public_key(pub_key)
                id = struct.unpack("i", rsa.encrypt_public(enc_id)[-4:])[0]
                
                if id > 0 and self._secure_db.has_key(id):
                    sec_id = self._secure_db[id]
                    if sec_id.public_modulus == pub_mod and sec_id.public_key == pub_key:
                        self._state = AuthSecureChannel.STATE_IDENTIFIED
                        self.send("y")
                        return
                
                self._state = AuthSecureChannel.STATE_UNIDENTIFIED
                TCPChannel.send(self, "n")
                self.close()
                self._isclosed = True
                
    def poll(self):
        return self._state == AuthSecureChannel.STATE_IDENTIFIED and TCPChannel.poll(self)
        
    def state(self):
        return self._state
        
    
class SecureChannelServer(TCPChannelServer):
    
    def __init__(self, address, port, secure_db):
        """
        secure_db - dict( id -> SecureId )
        """
        TCPChannelServer.__init__(self, address, port)
        self._secure_db = secure_db
        print secure_db
        self._auth_clients = set()
        self._ready_clients = set()
    
    def process(self):
        TCPChannelServer.process(self)
        if TCPChannelServer.poll(self):
            client = TCPChannelServer.accept(self, AuthSecureChannel)
            client._secure_db = self._secure_db
            self._auth_clients.add(client)
    
    def poll(self):
        new_clients = set()
        for c in self._auth_clients:
            if c.state() == AuthSecureChannel.STATE_IDENTIFIED:
                new_clients.add(c)
                self._ready_clients.add(c)
            elif c.state() == AuthSecureChannel.STATE_UNIDENTIFIED:
                new_clients.add(c)
                
        self._auth_clients.difference_update(new_clients)
        
        return len(self._ready_clients) > 0
    
    def accept(self):
        if len(self._ready_clients) > 0:
            return self._ready_clients.pop()
        
        return None
    
    def tasks(self):
        return list(self._auth_clients) + [self]
        

class SecureId:
    def __init__(self, id, public_modulus, public_key, private_key = None):
        self.id = int(id)
        self.public_modulus = public_modulus
        self.public_key = public_key
        self.private_key = private_key


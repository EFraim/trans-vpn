import socket
import struct
import select as cselect
import operator
import polarssl
import random
import rsaid

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
    
    PACKET_NEW     = 0
    PACKET_PARTIAL = 1

    def from_socket(self, sock):
        self._socket = sock
        self._poll = cselect.poll()
        self._poll.register(self._socket)
        self._packetstate = self.PACKET_NEW
        self._packetlen = None
        self._packet = ""
        self._full_packets = []
        self._isclosed = False
        
    def send(self, packet):
        self._socket.send(struct.pack("h", len(packet)) + packet)
    
    def poll(self):
        return len(self._full_packets) > 0 or self._isclosed
    
    def _poll_socket(self):
        return poll_read(self._poll)

    def recv(self):
        TCPChannel.process(self)
        if len(self._full_packets) > 0:
            return self._full_packets.pop(0)
        elif self._isclosed:
            raise ChannelClosedException()
        
        return None

    def process(self):
        try:
            all_data = ""
            while self._poll_socket():
                data = self._socket.recv(10000)
                if len(data) == 0:
                    self._isclosed = True
                    break
                else:
                    all_data += data
            
            while len(all_data) > 0:
                if self._packetstate == self.PACKET_NEW:
                    self._packetlen = struct.unpack("H", all_data[:2])[0]
                    self._packet = ""
                    
                    self._packetstate = self.PACKET_PARTIAL
                    all_data = all_data[2:]
                else:
                    recv_pkt_len = min(self._packetlen - len(self._packet), len(all_data))
                    self._packet += all_data[:recv_pkt_len]
                    
                    if len(self._packet) == self._packetlen:
                        self._packetstate = self.PACKET_NEW
                        self._full_packets.append(self._packet)
                    
                    all_data = all_data[recv_pkt_len:]
                    
        except socket.error, e:
            self._isclosed = True

    def fileno(self):
        try:
            return self._socket.fileno()
        except:
            import pdb
            pdb.set_trace()

    def close(self):
        self._poll.unregister(self._socket)
        self._socket.close()
        
    def tasks(self):
        return [self]

class TCPChannelClient:
    
    @staticmethod
    def connect(address, port, ChannelType = TCPChannel):
        channel = ChannelType()
        sock = socket.socket()
        sock.connect((address, port))
        channel.from_socket(sock)
        return channel


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
    
def generate_random_string(length):
    return "".join([chr(random.randrange(0, 256)) for i in xrange(length)])
    

RSA_BLOCK_SIZE   = 128
RSA_KEY_SIZE     = 128
AES_BLOCK_SIZE   = 16
AES_KEY_SIZE     = 16
CHALLENGE_SIZE   = 16

class SecureChannel(TCPChannel):
    
    @staticmethod
    def authentication_error(channel, msg, server_mode = False):
        TCPChannel.send(channel, "n" + msg);
        if not server_mode:
            TCPChannel.close(channel)
            raise RuntimeError(msg)
    
    @staticmethod
    def check_for_errors(channel, response, server_mode = False):
        if response[0] == 'n':
            if not server_mode:
                TCPChannel.close(channel)
                raise RuntimeError("Authentication failed: " + response[1:])
    

    def generate_random_key(self):
        return "".join([chr(random.randrange(0, 256)) for i in xrange(AES_KEY_SIZE)])

    def set_encryption_key(self, key):
        self._encrypt_ctx = polarssl.AES()
        self._decrypt_ctx = polarssl.AES()
        self._encrypt_ctx.set_enc_key(key)
        self._decrypt_ctx.set_dec_key(key)

    def send(self, packet):
        final_len = (len(packet) + AES_BLOCK_SIZE - 1) // AES_BLOCK_SIZE * AES_BLOCK_SIZE
        padlen = final_len - len(packet)
        padded_packet = packet + "\x00" * padlen
        enc_packet = self._encrypt_ctx.encrypt(padded_packet)
        TCPChannel.send(self, enc_packet + struct.pack("B", padlen))

    def recv(self):
        enc_packet = TCPChannel.recv(self)
        if enc_packet != None:
            padlen = struct.unpack("B", enc_packet[-1])[0]
            dec_packet = self._decrypt_ctx.decrypt(enc_packet[:-1])
            return dec_packet[:len(dec_packet) - padlen]
    

class SecureChannelClient:
    
    @staticmethod
    def connect(address, port, secure_id, server_secure_id, ChannelType = SecureChannel):
        
        channel = TCPChannelClient.connect(address, port, ChannelType)

        my_challenge = generate_random_string(CHALLENGE_SIZE)
        request = pad_string(secure_id.public_modulus, RSA_KEY_SIZE) + \
                  my_challenge
        
        TCPChannel.send(channel, request)
        
        select([channel])
        data = TCPChannel.recv(channel)
        SecureChannel.check_for_errors(channel, data)
        data = data[1:]
        
        if len(data) != 2 * RSA_BLOCK_SIZE:
            SecureChannel.authentication_error(channel, "Invalid response length from server")

        client_rsa = polarssl.RSA()
        client_rsa.set_public_modulus_binary(secure_id.public_modulus)
        client_rsa.set_private_key_binary(secure_id.private_key)
        
        server_signed_data = client_rsa.decrypt(data, client_rsa.PRIVATE)
        
        server_rsa = polarssl.RSA()
        server_rsa.set_public_modulus_binary(server_secure_id.public_modulus)
        server_rsa.set_public_key_binary(server_secure_id.public_key)
        
        server_data = server_rsa.decrypt(server_signed_data, server_rsa.PUBLIC)
        
        symmetric_key = server_data[-AES_KEY_SIZE:]
        challenge = server_data[:CHALLENGE_SIZE]
        
        if challenge != my_challenge:
            SecureChannel.authentication_error(channel, "Signed challenge doesn't match generated challenge")

        TCPChannel.send(channel, "y")
        
        channel.set_encryption_key(symmetric_key)
        
        return channel
    

class AuthSecureChannel(SecureChannel):
    
    STATE_INITIAL     = 0
    STATE_KEY_SENT    = 1
    STATE_ESTABLISHED = 2
    
    def __init__(self):
        self._state = self.STATE_INITIAL
    
    def process(self):
        TCPChannel.process(self)
        if self._state != self.STATE_ESTABLISHED:
            if TCPChannel.poll(self) and not self._isclosed:
                data = TCPChannel.recv(self)
                
                if self._state == self.STATE_INITIAL:
                    
                    if len(data) != RSA_KEY_SIZE + CHALLENGE_SIZE:
                        SecureChannel.authentication_error(self, "Invalid request length", True) # TODO
                    
                    client_id = rsaid.RSAId(data[:RSA_KEY_SIZE])
                    
                    if not client_id in self._client_ids:
                        SecureChannel.authentication_error(self, "Unknown client", True) # TODO
                        return
                    
                    challenge = data[-CHALLENGE_SIZE:]
                    
                    symmetric_key = generate_random_string(AES_KEY_SIZE)
                    self.set_encryption_key(symmetric_key)
                    
                    signed_data = self._server_rsa.encrypt(challenge + symmetric_key, \
                        self._server_rsa.PRIVATE)
                    
                    client_rsa = polarssl.RSA()
                    client_rsa.set_public_modulus_binary(client_id.public_modulus)
                    client_rsa.set_public_key_binary(client_id.public_key)
                    
                    encrypted_data = client_rsa.encrypt(signed_data, client_rsa.PUBLIC)
                    
                    TCPChannel.send(self, "y" + encrypted_data)
                    
                    self._state = self.STATE_KEY_SENT
                    
                elif self._state == self.STATE_KEY_SENT:
                    SecureChannel.check_for_errors(self, data, True)
                    
                    self._state = self.STATE_ESTABLISHED
                
    def poll(self):
        return self._state == self.STATE_ESTABLISHED and TCPChannel.poll(self)
        
    def state(self):
        return self._state
        
    
class SecureChannelServer(TCPChannelServer):
    
    def __init__(self, address, port, server_id, client_ids):
        """
        secure_db - dict( id -> SecureId )
        """
        TCPChannelServer.__init__(self, address, port)
        self._server_rsa = polarssl.RSA()
        self._server_rsa.set_public_modulus_binary(server_id.public_modulus)
        self._server_rsa.set_private_key_binary(server_id.private_key)
        
        self._client_ids = client_ids
        
        self._auth_clients = set()
        self._ready_clients = set()
    
    def process(self):
        TCPChannelServer.process(self)
        if TCPChannelServer.poll(self):
            channel = TCPChannelServer.accept(self, AuthSecureChannel)
            channel._client_ids = self._client_ids
            channel._server_rsa = self._server_rsa
            
            self._auth_clients.add(channel)
    
    def poll(self):
        affected_clients = set()
        for c in self._auth_clients:
            if c.state() == c.STATE_ESTABLISHED:
                affected_clients.add(c)
                self._ready_clients.add(c)
            elif c._isclosed:
                c.close()
                affected_clients.add(c)
                
        self._auth_clients.difference_update(affected_clients)
        
        return len(self._ready_clients) > 0
    
    def accept(self):
        if len(self._ready_clients) > 0:
            return self._ready_clients.pop()
        
        return None
    
    def tasks(self):
        return list(self._auth_clients) + [self]

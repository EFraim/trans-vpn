import threading
import zlib
import time
import random

class Streamer:
    """
    Runs a thread that send and receives packets over a PacketChannel.
    """
    
    def __init__(self, channel, nsend, nrecv, timeout, interval = 0.3, maxsize = 1000):
        """
        channel  - the chanel to use
        nsend    - number of packets to send
        nrecv    - number of packets to expect to recieve
        timeout  - in case 'nrecv' packets are not recieved within a given timeout,
                   thread terminates
        interval - maximal interval between consequent sent packets
        maxsize  - maximal size of generated packets
        """
        self._channel = channel
        self._nsend = nsend
        self._nrecv = nrecv
        self._timeout = timeout
        self._interval = interval
        self._maxsize = maxsize
    
    def start(self):
        """
        starts running the streamer in the new thread.
        """
        self._sentpkts = []
        self._recvpkts = []
        self._thread = threading.Thread(target = self._run_streamer)
        self._thread.start()
    
    def join(self):
        """
        returns (sendpkts, recvpkts)
        """
        self._thread.join()
        
        return self._sentpkts, self._recvpkts
    
    def _randombuf(self, length):
        buf = ""
        for i in xrange(length):
            buf += chr(random.randrange(32, 127))
        return buf
        
    def _run_streamer(self):
        
        nrecv = 0
        nsent = 0
        starttime = time.time()
        curtime = starttime
        nexttime = curtime + 0.5 # initial waiting interval
        while curtime - starttime < self._timeout:
            recvpkt = self._channel.recv()
            if recvpkt != None:
                self._recvpkts.append(recvpkt)
                nrecv += 1
            
            if (curtime >= nexttime) and (nsent < self._nsend):
                pktlen = random.randrange(1, self._maxsize + 1)
                print "sending pkt with len = ", pktlen
                pkt = self._randombuf(pktlen)
                self._sentpkts.append(pkt)
                self._channel.send(pkt)
                nsent += 1
                nexttime = curtime + random.uniform(0, self._interval)
            
            if nrecv == self._nrecv and nsent == self._nsend:
                break
            
            time.sleep(0.01)
            curtime = time.time()
            

#!/usr/bin/python

import sys
import streamer
import packetchannel
import unittest

class TestComm(unittest.TestCase):

    def assertSubset(self, subset, fullset):
        diff = set.difference(set(subset), set(fullset))
        self.assertEquals(diff, set())

    def setUp(self):
        self.HOST_PORT = 9999 # int(sys.argv[1])
        self.SERV_PORT = 6666 # int(sys.argv[2])
        self.host = streamer.Streamer(packetchannel.connect('localhost', self.HOST_PORT), 10, 10, 5)
        self.serv = streamer.Streamer(packetchannel.connect('localhost', self.SERV_PORT), 10, 10, 5)
        
    def testCommunication(self):
        self.host.start()
        self.serv.start()

        host_out, host_in = self.host.join()
        serv_out, serv_in = self.serv.join()
        
        self.assertTrue(len(host_in) > 0)
        self.assertTrue(len(serv_in) > 0)
        self.assertSubset(serv_in, host_out)
        self.assertSubset(host_in, serv_out)

unittest.main()

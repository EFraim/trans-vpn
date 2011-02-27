#!/usr/bin/python

import sys
import streamer
import packetchannel
import unittest

class TestComm(unittest.TestCase):

    def setUp(self):
        self.USB_PORT_MY     = 8887
        self.USB_PORT_OTHER  = 8888
        self.TEST_PORT_MY    = 6665
        self.TEST_PORT_OTHER = 6666
        
        self.usb_chan = packetchannel.UDPChannel('localhost', self.USB_PORT_MY,
                                                 'localhost', self.USB_PORT_OTHER)
        self.tst_chan = packetchannel.UDPChannel('localhost', self.TEST_PORT_MY,
                                                 'localhost', self.TEST_PORT_OTHER)
                                                
        self.usb_strm = streamer.Streamer(self.usb_chan, 10, 10, 5, 0.3, 1460)
        self.tst_strm = streamer.Streamer(self.tst_chan, 10, 10, 5, 0.3, 1460)
        
    def testCommunication(self):
        self.usb_strm.start()
        self.tst_strm.start()

        usb_out, usb_in = self.usb_strm.join()
        tst_out, tst_in = self.tst_strm.join()
        
        self.assertEquals(tst_out, usb_in)
        self.assertEquals(usb_out, tst_in)

unittest.main()

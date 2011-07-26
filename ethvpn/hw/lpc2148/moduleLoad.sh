#!/bin/sh
rmmod ftdi_sio
rmmod usbserial
modprobe usbserial vendor=0x16c0 product=0x03eb
modprobe ftdi_sio


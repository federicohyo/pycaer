"""Checking Network.

Author: Yuhuang Hu
Email : duguyue100@gmail.com
"""
from __future__ import print_function

import pycaer.network as network

# get network class

net = network.AEDAT3NetworkHeader()

#  net = network.AEDAT3NetworkHeader(10)

# checking function

print ("-"*50)
print ("[MESSAGE] NETWORK CHECK")
print ("-"*50)
print ("[MESSAGE] Get Magic Number:", net.getMagicNumber())
print ("[MESSAGE] Check Magic Number:", net.checkMagicNumber())
print ("-"*50)

print ("[MESSAGE] Get Sequence Number:", net.getSequenceNumber())
net.incrementSequenceNumber()
print ("[MESSAGE] Get Sequence Number again:", net.getSequenceNumber())
print ("-"*50)

print ("[MESSAGE] Get Version Number:", net.getVersionNumber())
print ("[MESSAGE] Check Version Number:", net.checkVersionNumber())
print ("-"*50)

print ("[MESSAGE] Get Format Number:", net.getFormatNumber())
net.setFormatNumber(12)
print ("[MESSAGE] Get Format Number again:", net.getFormatNumber())
print ("-"*50)

print ("[MESSAGE] Get Source ID:", net.getSourceID())
net.setSourceID(12345)
print ("[MESSAGE] Get Source ID again:", net.getSourceID())
print ("-"*50)

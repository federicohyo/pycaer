"""DYNAPSE Example, libcaer interface

Author: Federico Corradi
Email : federico.corradi@gmail.com
"""
import pycaer.devices as devices

DYNAPSE_DEV = 3
dynapse =  devices.dynapse(1,DYNAPSE_DEV,0,"")

#send default configuration to dynapse
dynapse.sendDefaultConfig()

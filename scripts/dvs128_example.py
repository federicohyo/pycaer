"""DVS 128 Example.

Author: Yuhuang Hu
Email : duguyue100@gmail.com
"""
from __future__ import print_function

import pycaer.devices as devices

# Open a DVS 128
dvs128_handle = devices.dvs128(1, 0, 0, "")

# Get device info
dvs128_info = dvs128_handle.infoGet()

print (dvs128_info.deviceID)
print (dvs128_info.deviceString)
print (dvs128_info.deviceIsMaster)
print (dvs128_info.dvsSizeX)
print (dvs128_info.dvsSizeY)
print (dvs128_info.logicVersion)

# send default configuration
dvs128_handle.sendDefaultConfig()

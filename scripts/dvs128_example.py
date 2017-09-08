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

print ("%s - ID: %s, Master: %d, DVS X: %d, DVS Y: %d, Logic: %d" %
       (dvs128_info.deviceString,
        dvs128_info.deviceSerialNumber,
        dvs128_info.deviceIsMaster,
        dvs128_info.dvsSizeX,
        dvs128_info.dvsSizeY,
        dvs128_info.logicVersion))

# send default configuration
dvs128_handle.sendDefaultConfig()

# tweak some bias
dvs128_handle.configSet(1, 11, 695)
dvs128_handle.configSet(1, 10, 867)

pr_bias = dvs128_handle.configGet(1, 11)
foll_bias = dvs128_handle.configGet(1, 10)

print (pr_bias, foll_bias)

dvs128_handle.dataStart(None, None, None, None, None)

dvs128_handle.dataStop()

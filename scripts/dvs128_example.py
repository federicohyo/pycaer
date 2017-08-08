"""DVS 128 Example.

Author: Yuhuang Hu
Email : duguyue100@gmail.com
"""

import pycaer.devices as devices

# Open a DVS 128
dvs128_handle = devices.dvs128(1, 0, 0, "")

# Get device info
dvs128_info = dvs128_handle.infoGet()

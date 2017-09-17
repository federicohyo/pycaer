"""DVS 128 Example.

Author: Yuhuang Hu
Email : duguyue100@gmail.com
"""
from __future__ import print_function

import pycaer.devices as devices

import numpy as np
import cv2

dvs128 = devices.DVS128Exporter()

dvs128.exporter_start()

idx = 1
while True:
    events = dvs128.export_data()

    print ("sample: ", idx)
    idx += 1
    img = np.zeros((128, 128), dtype=np.float)
    for event in events:
        if event.pol and event < 3:
            img[event.x, event.y] += 1.
        elif event.pol is False and event > -3:
            img[event.x, event.y] -= 1.
    img = (img+3.)/6.
    cv2.imshow("image", img)
    cv2.waitKey(10)

    del events

dvs128.exporter_stop()

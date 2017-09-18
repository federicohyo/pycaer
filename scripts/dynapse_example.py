"""DYNAPSE Example.

Author: Yuhuang Hu, Federico Corradi
Email : duguyue100@gmail.com, federico.corradi@gmail.com
"""
from __future__ import print_function

import pycaer.devices as devices

import numpy as np
import cv2

dynapse = devices.DYNAPSEExporter()

print("Initializing dynapse...")
dynapse.exporter_start()
print("Init done")
    
idx = 1
while True:
    # start exportind data to python
    events = dynapse.export_data()

    print ("sample: ", idx)
    idx += 1


    del events

dynapse.exporter_stop()

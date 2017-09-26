# -----------------------------------------------------------------------------
# Author : Federico Corradi -  
# Distributed under the BSD License.
# -----------------------------------------------------------------------------
from __future__ import print_function
import numpy as np
from glumpy import app
from glumpy.graphics.collections import PointCollection
import socket
import struct
import time
import threading, Queue


import pycaer.devices as devices

dynapse = devices.DYNAPSEExporter()

print("Initializing dynapse...")
dynapse.exporter_start()
print("Init done")

# PARAMETERS 
xdim = 64
ydim = 64
sizeW = 1024
timeMul = 10e-6
#end parameters

window = app.Window(sizeW,sizeW, color=(0,0,0,1))
points = PointCollection("agg", color="local", size="local")

Z = [[],[], [], []]
q = Queue.Queue()
q.put(Z)

def read_events(q):
    """ A simple function that read events from dynapse board"""
    while getattr(t, "do_run", True):
        events = dynapse.export_data()
        cores = []
        chips = []
        neus = []
        ts = []
        done = False
        for thise in range(len(events)):
            single = events[thise]
            cores.append(int(single.coreid))
            chips.append(int(single.chipid))
            neus.append(int(single.neuid))
            ts.append(int(single.ts))
            done = True
        if done:
            lock.acquire()
            q.put([[cores], [chips], [neus], [ts]])
            lock.release()
        del events
        time.sleep(0.03)
    
# start thread that reads from usb, it communicates to the main via the Queue q
t = threading.Thread(target=read_events,args=(q,))
lock = threading.Lock()
t.start()    
dtt = 0; 
   
@window.event    
def on_close():
    global dynapse
    setattr(t, "do_run", False)
    #t.do_run = False
    t.join()
    dynapse.exporter_stop()
    print("closed thread ")
    del dynapse
        
@window.event
def on_draw(dt):
    global dtt 
    window.clear()
    points.draw()
    if(q.empty() != True):
        lock.acquire()
        tt = q.get()
        coreid = [item for sublist in tt[0] for item in sublist]
        chipid = [item for sublist in tt[1] for item in sublist]
        neuronid = [item for sublist in tt[2] for item in sublist]
        timestamp = [item for sublist in tt[3] for item in sublist]
        timestamp = np.diff(timestamp)
        timestamp = np.insert(timestamp,0,0.0001)
        if(len(chipid) > 1):
            for i in range(len(chipid)):

                dtt += float(timestamp[i])*timeMul
                
                if(dtt >= 1.0):
                    dtt = -1.0
                    del points[...]
                    q.queue.clear()
                y_c = 0
                if( chipid[i] == 0):
                    y_c = (neuronid[i])+(coreid[i]*256)+((chipid[i])*1024)
                    y_c = float(y_c)/(1024*2.0)
                elif(chipid[i] == 2 ):
                    y_c = (neuronid[i])+(coreid[i]*256)+((chipid[i])*1024)
                    y_c = (float(y_c)/(1024*4.0))*2-((sizeW*0.5)/sizeW)          
                elif(chipid[i] == 1 ):
                    y_c = (neuronid[i])+(coreid[i]*256)+((chipid[i])*1024)
                    y_c = -(float(y_c)/(1024*2.0))
                elif(chipid[i] == 3 ):
                    y_c = (neuronid[i])+(coreid[i]*256)+((chipid[i])*1024)
                    y_c = -(float(y_c)/(1024*2.0))+((sizeW*0.5)/sizeW)*3          
                if(coreid[i] == 0):
                    col = (1,0,1,1)
                elif(coreid[i] == 1):
                    col = (1,0,0,1)
                elif(coreid[i] == 2):
                    col = (0,1,1,1)
                elif(coreid[i] == 3):
                    col = (0,0,1,1)
                y_c = round(y_c, 6)

                points.append([dtt,y_c,0], color = col, size  = 3)
                          
        lock.release()
	    
dtt = -1.0	    
window.attach(points["transform"])
window.attach(points["viewport"])
app.run(framerate=60)




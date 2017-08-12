"""Logging test.

Author: Yuhuang Hu
Email : duguyue100@gmail.com
"""
from __future__ import print_function

import pycaer.log as log

print ("-"*50)
print ("[MESSAGE] LOGLEVEL CHECK")
print ("-"*50)
log_level = log.logLevel

print ("[MESSAGE] EMERGENCY: %d" % (log_level.EMERGENCY))
print ("[MESSAGE] ALERT    : %d" % (log_level.ALERT))
print ("[MESSAGE] CRITICAL : %d" % (log_level.CRITICAL))
print ("[MESSAGE] ERROR    : %d" % (log_level.ERROR))
print ("[MESSAGE] WARNING  : %d" % (log_level.WARNING))
print ("[MESSAGE] NOTICE   : %d" % (log_level.NOTICE))
print ("[MESSAGE] INFO     : %d" % (log_level.INFO))
print ("[MESSAGE] DEBUG    : %d" % (log_level.DEBUG))
print ("-"*50)
print ("[MESSAGE] Get Log Level      :", log.logLevelGet())
log.logLevelSet(log_level.DEBUG)
print ("[MESSAGE] Get Log Level again:", log.logLevelGet())
print ("-"*50)
print ("[MESSAGE] Get First File Descriptors :", log.fileDescriptorsGetFirst())
print ("[MESSAGE] Get Second File Descriptors:",
       log.fileDescriptorsGetSecond())
log.fileDescriptorsSet(1, 1)
print ("[MESSAGE] Get First File Descriptors again :",
       log.fileDescriptorsGetFirst())
print ("[MESSAGE] Get Second File Descriptors again:",
       log.fileDescriptorsGetSecond())
log.fileDescriptorsSet(3, 2)
print ("[MESSAGE] Get First File Descriptors again :",
       log.fileDescriptorsGetFirst())
print ("[MESSAGE] Get Second File Descriptors again:",
       log.fileDescriptorsGetSecond())
print ("-"*50)
log.log(log_level.INFO, "Example log", "This is an example log {}", 1)
log.logVA(log_level.INFO, "Example log",
          "This is an example log {} {}",
          "test1", "test2")
log.logVAFull(2, -1, log_level.ERROR, log_level.ALERT, "Example log",
              "This is an example log {} {}",
              "test3", "test4")
print ("-"*50)

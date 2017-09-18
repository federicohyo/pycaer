# This is a Python template Makefile, do modification as you want
#
# Project: 
# Author:
# Email :

HOST = 127.0.0.1
PYTHONPATH="$(shell printenv PYTHONPATH):$(PWD)/../build"

clean:
	find . -name '*.pyc' -exec rm --force {} +
	find . -name '*.pyo' -exec rm --force {} +
	find . -name '*~' -exec rm --force  {} +

run:

dvs128-example:
	PYTHONPATH=$(PYTHONPATH) python dvs128_example.py

dynapse-example:
	PYTHONPATH=$(PYTHONPATH) python dynapse_example.py

network-test:
	PYTHONPATH=$(PYTHONPATH) python network_test.py

log-test:
	PYTHONPATH=$(PYTHONPATH) python log_test.py

test:
	PYTHONPATH=$(PYTHONPATH) python 

cleanall:

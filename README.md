# PyCAER

Python `libcaer` bindings with Pybind11

## Prerequisites

+ [Anaconda](https://anaconda.org/)
+ CMake
+ Git
+ `libusb`
+ `libserialport` (optional)
+ OpenCV (support from Anaconda)

## Checked Functions

+ [x] Logging
+ [x] Network
+ [ ] Events
+ [ ] Devices
   + [x] DVS128
   + [ ] DAVIS
   + [x] DYNAP
   + [ ] EDVS

## Clone the repository

```
git clone --recursive https://github.com/duguyue100/pycaer.git
```

## Compilation

To compile the library, use following command

```
$ ./compile make
```

## Clean Compilation

```
$ ./compile clean
```

## How to use

The program is compiled as a single shared library `pycaer.so`,
one can directly import it in Python

```python
import pycaer
```

## Contacts

Yuhuang Hu  
Email: duguyue100@gmail.com

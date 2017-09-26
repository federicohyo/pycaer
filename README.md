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
   + [x] DYNAP-SE
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

## Problems/Extensions?

If you have any problems, please submit an issue or pull request!

## How to use

The program is compiled as a single shared library `pycaer.so`,
one can directly import it in Python

```python
import pycaer
```

## Running Examples

We have some test examples you can work on in `scripts` folder!

+ For DVS128, just run

    ```
    make dvs128-example
    ```

+ For DYNAP-SE, just run

    ```
    make dynapse-example
    ```
    
    the script example 'dynapse_visualizer.py' exploits glumpy to visualize spikes. Glumpy is a scientific library for fast visualization, it is interfaced with numpy and OpenGL. 
    ![Alt text](/docs/images/RasterDynapse.png?raw=true "RasterPlot Dynap-se")


## Contacts

Yuhuang Hu  
Email: duguyue100@gmail.com

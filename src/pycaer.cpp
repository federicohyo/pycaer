/*
 * pycaer.cpp
 *
 *  Created on: 29 Jul 2017
 *      Author: Yuhuang Hu
 *      Email : duguyue100@gmail.com
 */

// PyBind11
#include <pybind11/pybind11.h>

namespace py = pybind11;

void pylog_module(py::module &);
void pynetwork_module(py::module &);
void pyevents_module(py::module &);
void pydevices_module(py::module &);

PYBIND11_MODULE(pycaer, libpycaer) {
    libpycaer.doc() = "The master module of libcaer";

    // ------ Log ------
    pylog_module(libpycaer);

    // ------ Network ------
    pynetwork_module(libpycaer);

    // ------ Events ------
    pyevents_module(libpycaer);

    // ------ Devices ------
    pydevices_module(libpycaer);
}


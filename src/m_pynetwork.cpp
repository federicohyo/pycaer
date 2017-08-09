/*
 * m_pynetwork for network function
 *
 * Author: Yuhuang Hu
 * Email : duguyue100@gmail.com
 */

// PyBind11
#include <pybind11/pybind11.h>

// libcaer CPP
#include <libcaercpp/network.hpp>

namespace py = pybind11;
using namespace libcaer::network;

void pynetwork_module(py::module &libpycaer)
{
    py::module pynetwork = libpycaer.def_submodule("network", "The Network submodule");
    py::class_<AEDAT3NetworkHeader>(pynetwork, "AEDAT3NetworkHeader")
    	.def(py::init<>())
        .def(py::init<const uint8_t *>())
    	.def("getMagicNumber", &AEDAT3NetworkHeader::getMagicNumber)
        .def("checkMagicNumber", &AEDAT3NetworkHeader::checkMagicNumber)
        .def("getSequenceNumber", &AEDAT3NetworkHeader::getSequenceNumber)
        .def("incrementSequenceNumber", &AEDAT3NetworkHeader::incrementSequenceNumber)
        .def("getVersionNumber", &AEDAT3NetworkHeader::getVersionNumber)
        .def("checkVersionNumber", &AEDAT3NetworkHeader::checkVersionNumber)
        .def("getFormatNumber", &AEDAT3NetworkHeader::getFormatNumber)
        .def("setFormatNumber", &AEDAT3NetworkHeader::setFormatNumber)
        .def("getSourceID", &AEDAT3NetworkHeader::getSourceID)
        .def("setSourceID", &AEDAT3NetworkHeader::setSourceID);
}

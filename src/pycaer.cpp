/*
 * pycaer.cpp
 *
 *  Created on: 29 Jul 2017
 *      Author: Yuhuang Hu
 *      Email : duguyue100@gmail.com
 */

#include <pybind11/pybind11.h>
#include <libcaercpp/libcaer.hpp>
#include <libcaercpp/network.hpp>
#include <libcaercpp/events/utils.hpp>
#include <libcaercpp/events/packetContainer.hpp>

namespace py = pybind11;
//using namespace libcaer::log;
using namespace libcaer::network;

PYBIND11_MODULE(pycaer, libpycaer) {
	libpycaer.doc() = "The master module of libcaer";

	// Log
    py::module pylog = libpycaer.def_submodule("log", "The log submodule");

    py::enum_<libcaer::log::logLevel>(pylog, "logLevel")
    	.value("EMERGENCY", libcaer::log::logLevel::EMERGENCY)
		.value("ALERT", libcaer::log::logLevel::ALERT)
		.value("CRITICAL", libcaer::log::logLevel::CRITICAL)
		.value("ERROR", libcaer::log::logLevel::ERROR)
		.value("WARNING", libcaer::log::logLevel::WARNING)
		.value("NOTICE", libcaer::log::logLevel::NOTICE)
		.value("INFO", libcaer::log::logLevel::INFO)
		.value("DEBUG", libcaer::log::logLevel::DEBUG)
		.export_values();

    pylog.def("logLevelSet", &libcaer::log::logLevelSet);
    pylog.def("logLevelGet", &libcaer::log::logLevelGet);
    pylog.def("fileDescriptorsSet", &libcaer::log::fileDescriptorsSet);
    pylog.def("fileDescriptorsGetFirst", &libcaer::log::fileDescriptorsGetFirst);
    pylog.def("fileDescriptorsGetSecond", &libcaer::log::fileDescriptorsGetSecond);
    pylog.def("log", [](libcaer::log::logLevel l, const char *subSystem, const char *format, py::args args){
        libcaer::log::log(l, subSystem, format, *args);
    });
    // pylog.def("logVA", [](libcaer::log::logLevel l, const char *subSystem, const char *format, py::kwargs kwargs){
    //     libcaer::log::logVA(l, subSystem, format, **kwargs);
    // });
    // pylog.def("logVAFull", [](int logFileDescriptor1, int logFileDescriptor2, uint8_t systemLogLevel, libcaer::log::logLevel l, const char *subSystem, const char *format, py::kwargs kwargs){
    //     libcaer::log::logVAFull(logFileDescriptor1, logFileDescriptor2, systemLogLevel, l, subSystem, format, **kwargs);
    // });
    // pylog.def("logVA", &libcaer::log::logVA);
    // pylog.def("logVAFull", &libcaer::log::logVAFull);

    // Network
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

    // Events

    // Devices
}


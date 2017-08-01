/*
 * pycaer.cpp
 *
 *  Created on: 29 Jul 2017
 *      Author: Yuhuang Hu
 *      Email : duguyue100@gmail.com
 */

#include <pybind11/pybind11.h>
#include <libcaer/libcaer.h>
#include <libcaercpp/libcaer.hpp>
#include <libcaercpp/network.hpp>
#include <libcaercpp/events/utils.hpp>
#include <libcaercpp/events/packetContainer.hpp>
#include <libcaercpp/devices/device.hpp>
#include <libcaercpp/devices/davis.hpp>
#include <libcaercpp/devices/dvs128.hpp>
#include <libcaercpp/devices/dynapse.hpp>
#include <libcaercpp/devices/edvs.hpp>

namespace py = pybind11;
//using namespace libcaer::log;
using namespace libcaer::network;
using namespace libcaer::events;
using namespace libcaer::devices;

PYBIND11_MODULE(pycaer, libpycaer) {
	libpycaer.doc() = "The master module of libcaer";

	// ------ Log ------
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

    pylog.def("log", [](libcaer::log::logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        std::string formatted = format.format(*args, **kwargs);
        libcaer::log::log(l, subSystem, formatted.c_str());
    });

    pylog.def("logVA", [](libcaer::log::logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        va_list argumentList;
        std::string formatted = format.format(*args, **kwargs);

        caerLogVA(static_cast<enum caer_log_level>(static_cast<typename std::underlying_type<libcaer::log::logLevel>::type>(l)), subSystem, formatted.c_str(), argumentList);
    });

    pylog.def("logVAFull", [](int logFileDescriptor1, int logFileDescriptor2, uint8_t systemLogLevel, libcaer::log::logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        va_list argumentList;
        std::string formatted = format.format(*args, **kwargs);

        caerLogVAFull(logFileDescriptor1, logFileDescriptor2, systemLogLevel,
                static_cast<enum caer_log_level>(static_cast<typename std::underlying_type<libcaer::log::logLevel>::type>(l)), subSystem, formatted.c_str(), argumentList);

    });

    // ------ Network ------
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

    // ------ Events ------
    py::module pyevents = libpycaer.def_submodule("events", "The Events submodule");

    // EventPacket
    py::class_<EventPacket> event_packet(pyevents, "EventPacket");
    event_packet
        // .def("header", &EventPacket::header)
        // .def("isMemoryOwner", &EventPacket::isMemoryOwner)
        // .def(py::init<>())
        .def(py::init<caerEventPacketHeader, bool>())
        .def(py::init<const EventPacket &>())
        // TODO missed many inits
        .def("getEventType", &EventPacket::getEventType)
        .def("setEventType", &EventPacket::setEventType)
        .def("getEventSource", &EventPacket::getEventSource)
        .def("setEventSource", &EventPacket::setEventSource)
        .def("getEventSize", &EventPacket::getEventSize)
        .def("setEventSize", &EventPacket::setEventSize)
        .def("getEventTSOffset", &EventPacket::getEventTSOffset)
        .def("setEventTSOffset", &EventPacket::setEventTSOffset)
        .def("getEventTSOverflow", &EventPacket::getEventTSOverflow)
        .def("setEventTSOverflow", &EventPacket::setEventTSOverflow)
        .def("getEventCapacity", &EventPacket::getEventCapacity)
        .def("setEventCapacity", &EventPacket::setEventCapacity)
        .def("getEventNumber", &EventPacket::getEventNumber)
        .def("setEventNumber", &EventPacket::setEventNumber)
        .def("getEventValid", &EventPacket::getEventValid)
        .def("setEventValid", &EventPacket::setEventValid)
        // .def("GenericEvent",
        .def("genericGetEvent", &EventPacket::genericGetEvent)
        .def("getDataSize", &EventPacket::getDataSize)
        .def("getSize", &EventPacket::getSize)
        .def("clear", &EventPacket::clear)
        .def("clean", &EventPacket::clean)
        .def("resize", &EventPacket::resize)
        .def("shrink_to_fit", &EventPacket::shrink_to_fit)
        .def("grow", &EventPacket::grow)
        .def("append", &EventPacket::append)
        // copyTypes
        .def("copy", &EventPacket::copy)
        .def("swap", &EventPacket::swap)
        // .def("getHeaderPointer", &EventPacket::getHeaderPointer)
        //caerEventPacketHeaderConst getHeaderPointer()
        .def("isPacketMemoryOwner", &EventPacket::isPacketMemoryOwner)
        .def("capacity", &EventPacket::capacity)
        .def("size", &EventPacket::size)
        .def("empty", &EventPacket::empty);

    // ------ Devices ------
    py::module pydevices = libpycaer.def_submodule("devices", "The Devices submodule");

    // DEVICE
    py::class_<device> ddevice(pydevices, "device");
    ddevice
        .def("sendDefaultConfig", &device::sendDefaultConfig)
        .def("configSet", &device::configSet)
        .def("configGet", (void (device::*)(int8_t, uint8_t, uint32_t *) const) &device::configGet, "return void")
        .def("configGet", (uint32_t (device::*)(int8_t, uint8_t) const) &device::configGet, "return uint32_t")
        .def("dataStart", &device::dataStart)
        .def("dataStop", &device::dataStop)
        .def("dataGet", &device::dataGet);
    
    // DAVIS
    py::class_<davis> davis_device(pydevices, "davis");
    davis_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>())
        .def("infoGet", &davis::infoGet)
        .def_static("biasVDACGenerate", &davis::biasVDACGenerate)
        .def_static("biasVDACParse", &davis::biasVDACParse)
        .def_static("biasCoarseFineGenerate", &davis::biasCoarseFineGenerate)
        .def_static("biasCoarseFineParse", &davis::biasCoarseFineParse)
        .def_static("biasShiftedSourceGenerate", &davis::biasShiftedSourceGenerate)
        .def_static("biasShiftedSourceParse", &davis::biasShiftedSourceParse);

    py::class_<davisfx2> davisfx2_device(pydevices, "davisfx2");
    davisfx2_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>());

    py::class_<davisfx3> davisfx3_device(pydevices, "davisfx3");
    davisfx3_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>());

    // DVS128
    py::class_<dvs128> dvs128_device(pydevices, "dvs128");
    dvs128_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>())
        .def("infoGet", &dvs128::infoGet);

    // DYNAPSE
    py::class_<dynapse> dynapse_device(pydevices, "dynapse");
    dynapse_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>())
        .def("infoGet", &dynapse::infoGet)
        .def("sendDataToUSB", &dynapse::sendDataToUSB)
        .def("writeSramWords", &dynapse::writeSramWords)
        .def("writeSram", &dynapse::writeSram)
        .def("writePoissonSpikeRate", &dynapse::writePoissonSpikeRate)
        .def("writeCam", &dynapse::writeCam)
        .def("generateCamBits", &dynapse::generateCamBits);

    // eDVS
    py::class_<edvs> edvs_device(pydevices, "edvs");
    edvs_device
        .def(py::init<uint16_t, const std::string &, uint32_t>())
        .def("infoGet", &edvs::infoGet);

}


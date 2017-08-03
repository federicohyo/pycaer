/*
 * pycaer.cpp
 *
 *  Created on: 29 Jul 2017
 *      Author: Yuhuang Hu
 *      Email : duguyue100@gmail.com
 */

// PyBind11
#include <pybind11/pybind11.h>

// libcaer C
#include <libcaer/libcaer.h>
#include <libcaer/events/config.h>
#include <libcaer/events/ear.h>
#include <libcaer/events/frame.h>
#include <libcaer/frame_utils.h>
#include <libcaer/events/imu6.h>
#include <libcaer/events/imu9.h>
#include <libcaer/events/point1d.h>
#include <libcaer/events/point2d.h>
#include <libcaer/events/point3d.h>
#include <libcaer/events/point4d.h>
#include <libcaer/events/polarity.h>
#include <libcaer/events/sample.h>
#include <libcaer/events/special.h>
#include <libcaer/events/spike.h>

// libcaer CPP
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

    // EventPacketIterator
    // not implemented TODO

    // EventPacket
    py::class_<EventPacket> event_packet(pyevents, "EventPacket");
    event_packet
        .def(py::init<caerEventPacketHeader, bool>())
        .def(py::init<const EventPacket &>())
        // .def(py::init<EventPacket &&>())
        // TODO operators ==, !=
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
        // GenericEvent
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
        .def("getHeaderPointer", (caerEventPacketHeader (EventPacket::*)()) &EventPacket::getHeaderPointer)
        .def("getHeaderPointer", (caerEventPacketHeaderConst (EventPacket::*)() const) &EventPacket::getHeaderPointer)
        .def("isPacketMemoryOwner", &EventPacket::isPacketMemoryOwner)
        .def("capacity", &EventPacket::capacity)
        .def("size", &EventPacket::size)
        .def("empty", &EventPacket::empty);

    py::class_<EventPacket::GenericEvent>(event_packet, "GenericEvent")
        .def_readwrite("event", &EventPacket::GenericEvent::event)
        .def_readwrite("header", &EventPacket::GenericEvent::header)
        .def("getTimestamp", &EventPacket::GenericEvent::getTimestamp)
        .def("getTimestamp64", &EventPacket::GenericEvent::getTimestamp64)
        .def("isValid", &EventPacket::GenericEvent::isValid);

    py::enum_<EventPacket::copyTypes>(event_packet, "copyTypes")
        .value("FULL", EventPacket::copyTypes::FULL)
        .value("EVENTS_ONLY", EventPacket::copyTypes::EVENTS_ONLY)
        .value("VALID_EVENTS_ONLY", EventPacket::copyTypes::VALID_EVENTS_ONLY)
        .export_values();

    // EventPacketCommon
    // TODO
    // py::class_<EventPacketCommon, EventPacket> event_packet_common(pyevents, "EventPacketCommon");
    // event_packet_common
    //     .def("getEvent", (EventPacketCommon::reference (EventPacketCommon::*)(EventPacketCommon::size_type)) &EventPacketCommon::getEvent)
    //     .def("getEvent", (EventPacketCommon::const_reference (EventPacketCommon::*)(EventPacketCommon::size_type) const) &EventPacketCommon::getEvent);

    // ConfigurationEvent
    py::class_<caer_configuration_event> caer_config_event(pyevents, "caer_configuration_event");
    caer_config_event
        .def_readwrite("moduleAddress", &caer_configuration_event::moduleAddress)
        .def_readwrite("parameterAddress", &caer_configuration_event::parameterAddress)
        .def_readwrite("parameter", &caer_configuration_event::parameter)
        .def_readwrite("timestamp", &caer_configuration_event::timestamp);

    py::class_<ConfigurationEvent, caer_configuration_event> config_event(pyevents, "ConfigurationEvent");
    config_event
        .def("getTimestamp", &ConfigurationEvent::getTimestamp)
        .def("getTimestamp64", &ConfigurationEvent::getTimestamp64)
        .def("setTimestamp", &ConfigurationEvent::setTimestamp)
        .def("isValid", &ConfigurationEvent::isValid)
        .def("validate", &ConfigurationEvent::validate)
        .def("invalidate", &ConfigurationEvent::invalidate)
        .def("getModuleAddress", &ConfigurationEvent::getModuleAddress)
        .def("setModuleAddress", &ConfigurationEvent::setModuleAddress)
        .def("getParameterAddress", &ConfigurationEvent::getParameterAddress)
        .def("setParameterAddress", &ConfigurationEvent::setParameterAddress)
        .def("getParameter", &ConfigurationEvent::getParameter)
        .def("setParameter", &ConfigurationEvent::setParameter);

    // ConfigurationEventPacket
    py::class_<EventPacketCommon<ConfigurationEventPacket, ConfigurationEvent>>(pyevents, "EventPacketCommon<ConfigurationEventPacket, ConfigurationEvent>");
    py::class_<ConfigurationEventPacket, EventPacketCommon<ConfigurationEventPacket, ConfigurationEvent>> config_event_packet(pyevents, "ConfigurationEventPacket");
    config_event_packet
        .def(py::init<EventPacketCommon<ConfigurationEventPacket, ConfigurationEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerConfigurationEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());
    
    // EarEvent
    py::class_<EarEvent, caer_ear_event> ear_event(pyevents, "EarEvent");
    ear_event
        .def("getTimestamp", &EarEvent::getTimestamp)
        .def("getTimestamp64", &EarEvent::getTimestamp64)
        .def("setTimestamp", &EarEvent::setTimestamp)
        .def("isValid", &EarEvent::isValid)
        .def("validate", &EarEvent::validate)
        .def("invalidate", &EarEvent::invalidate)
        .def("getEar", &EarEvent::getEar)
        .def("setEar", &EarEvent::setEar)
        .def("getChannel", &EarEvent::getChannel)
        .def("setChannel", &EarEvent::setChannel)
        .def("getNeuron", &EarEvent::getNeuron)
        .def("setNeuron", &EarEvent::setNeuron)
        .def("getFilter", &EarEvent::getFilter)
        .def("setFilter", &EarEvent::setFilter);

    // EarEventPacket
    py::class_<EarEventPacket, EventPacketCommon<EarEventPacket, EarEvent>> ear_event_packet(pyevents, "EarEventPacket");
    ear_event_packet
        .def(py::init<EventPacketCommon<EarEventPacket, EarEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerEarEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // FrameEvent
    py::class_<FrameEvent, caer_frame_event> frame_event(pyevents, "FrameEvent");
    frame_event
        // colorChannels
        // colorFilter
        .def("getTSStartOfFrame", &FrameEvent::getTSStartOfFrame)
        .def("getTSStartOfFrame64", &FrameEvent::getTSStartOfFrame64)
        .def("setTSStartOfFrame", &FrameEvent::setTSStartOfFrame)
        .def("getTSEndOfFrame", &FrameEvent::getTSEndOfFrame)
        .def("getTSEndOfFrame64", &FrameEvent::getTSEndOfFrame64)
        .def("setTSEndOfFrame", &FrameEvent::setTSEndOfFrame)
        .def("getTSStartOfExposure", &FrameEvent::getTSStartOfExposure)
        .def("getTSStartOfExposure64", &FrameEvent::getTSStartOfExposure64)
        .def("setTSStartOfExposure", &FrameEvent::setTSStartOfExposure)
        .def("getTSEndOfExposure", &FrameEvent::getTSEndOfExposure)
        .def("getTSEndOfExposure64", &FrameEvent::getTSEndOfExposure64)
        .def("setTSEndOfExposure", &FrameEvent::setTSEndOfExposure)
        .def("getTimestamp", &FrameEvent::getTimestamp)
        .def("getTimestamp64", &FrameEvent::getTimestamp64)
        .def("getExposureLength", &FrameEvent::getExposureLength)
        .def("isValid", &FrameEvent::isValid)
        .def("validate", &FrameEvent::validate)
        .def("invalidate", &FrameEvent::invalidate)
        .def("getROIIdentifier", &FrameEvent::getROIIdentifier)
        .def("setROIIdentifier", &FrameEvent::setROIIdentifier)
        .def("getColorFilter", &FrameEvent::getColorFilter)
        .def("setColorFilter", &FrameEvent::setColorFilter)
        .def("getLengthX", &FrameEvent::getLengthX)
        .def("getLengthY", &FrameEvent::getLengthY)
        .def("getChannelNumber", &FrameEvent::getChannelNumber)
        .def("setLengthXLengthYChannelNumber", &FrameEvent::setLengthXLengthYChannelNumber)
        .def("getPixelsMaxIndex", &FrameEvent::getPixelsMaxIndex)
        .def("getPixelsSize", &FrameEvent::getPixelsSize)
        .def("getPositionX", &FrameEvent::getPositionX)
        .def("setPositionX", &FrameEvent::setPositionX)
        .def("getPositionY", &FrameEvent::getPositionY)
        .def("setPositionY", &FrameEvent::setPositionY)
        .def("getPixel", (uint16_t (FrameEvent::*)(int32_t, int32_t) const) &FrameEvent::getPixel)
        .def("setPixel", (void (FrameEvent::*)(int32_t, int32_t, uint16_t)) &FrameEvent::setPixel)
        .def("getPixel", (uint16_t (FrameEvent::*)(int32_t, int32_t, uint8_t) const) &FrameEvent::getPixel)
        .def("setPixel", (void (FrameEvent::*)(int32_t, int32_t, uint8_t, uint16_t)) &FrameEvent::setPixel)
        .def("getPixelUnsafe", (uint16_t (FrameEvent::*)(int32_t, int32_t) const) &FrameEvent::getPixelUnsafe)
        .def("setPixelUnsafe", (void (FrameEvent::*)(int32_t, int32_t, uint16_t)) &FrameEvent::setPixelUnsafe)
        .def("getPixelUnsafe", (uint16_t (FrameEvent::*)(int32_t, int32_t, uint8_t) const) &FrameEvent::getPixelUnsafe)
        .def("setPixelUnsafe", (void (FrameEvent::*)(int32_t, int32_t, uint8_t, uint16_t)) &FrameEvent::setPixelUnsafe)
        .def("getPixelArrayUnsafe", (uint16_t * (FrameEvent::*)()) &FrameEvent::getPixelArrayUnsafe)
        .def("getPixelArrayUnsafe", (const uint16_t * (FrameEvent::*)() const) &FrameEvent::getPixelArrayUnsafe);

    py::enum_<FrameEvent::colorChannels>(frame_event, "colorChannels")
        .value("GRAYSCALE", FrameEvent::colorChannels::GRAYSCALE)
        .value("RGB", FrameEvent::colorChannels::RGB)
        .value("RGBA", FrameEvent::colorChannels::RGBA)
        .export_values();

    py::enum_<FrameEvent::colorFilter>(frame_event, "colorFilter")
        .value("MONO", FrameEvent::colorFilter::MONO)
        .value("RGBG", FrameEvent::colorFilter::RGBG)
        .value("GRGB", FrameEvent::colorFilter::GRGB)
        .value("GBGR", FrameEvent::colorFilter::GBGR)
        .value("BGRG", FrameEvent::colorFilter::BGRG)
        .value("RGBW", FrameEvent::colorFilter::RGBW)
        .value("GRWB", FrameEvent::colorFilter::GRWB)
        .value("WBGR", FrameEvent::colorFilter::WBGR)
        .value("BWRG", FrameEvent::colorFilter::BWRG)
        .export_values();

    // FrameEventPacket
    py::class_<FrameEventPacket, EventPacketCommon<FrameEventPacket, FrameEvent>> frame_event_packet(pyevents, "FrameEventPacket");
    frame_event_packet
        .def(py::init<EventPacketCommon<FrameEventPacket, FrameEvent>::size_type, int16_t, int32_t, int32_t, int32_t, int16_t>())
        .def(py::init<caerFrameEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>())
        .def("getPixelsSize", &FrameEventPacket::getPixelsSize)
        .def("getPixelsMaxIndex", &FrameEventPacket::getPixelsMaxIndex)
        .def("demosaic", &FrameEventPacket::demosaic)
        .def("contrast", &FrameEventPacket::contrast);

    py::enum_<FrameEventPacket::demosaicTypes>(frame_event_packet, "demosaicTypes")
        .value("STANDARD", FrameEventPacket::demosaicTypes::STANDARD)
        .export_values();

    py::enum_<FrameEventPacket::contrastTypes>(frame_event_packet, "contrastTypes")
        .value("STANDARD", FrameEventPacket::contrastTypes::STANDARD)
        .export_values();

    // IMU6Event
    py::class_<IMU6Event, caer_imu6_event> imu6_event(pyevents, "IMU6Event");
    imu6_event
        .def("getTimestamp", &IMU6Event::getTimestamp)
        .def("getTimestamp64", &IMU6Event::getTimestamp64)
        .def("setTimestamp", &IMU6Event::setTimestamp)
        .def("isValid", &IMU6Event::isValid)
        .def("validate", &IMU6Event::validate)
        .def("invalidate", &IMU6Event::invalidate)
        .def("getAccelX", &IMU6Event::getAccelX)
        .def("setAccelX", &IMU6Event::setAccelX)
        .def("getAccelY", &IMU6Event::getAccelY)
        .def("setAccelY", &IMU6Event::setAccelY)
        .def("getAccelZ", &IMU6Event::getAccelZ)
        .def("setAccelZ", &IMU6Event::setAccelZ)
        .def("getGyroX", &IMU6Event::getGyroX)
        .def("setGyroX", &IMU6Event::setGyroX)
        .def("getGyroY", &IMU6Event::getGyroY)
        .def("setGyroY", &IMU6Event::setGyroY)
        .def("getGyroZ", &IMU6Event::getGyroZ)
        .def("setGyroZ", &IMU6Event::setGyroZ)
        .def("getTemp", &IMU6Event::getTemp)
        .def("setTemp", &IMU6Event::setTemp);

    // IMU6EventPacket
    py::class_<IMU6EventPacket, EventPacketCommon<IMU6EventPacket, IMU6Event>> imu6_event_packet(pyevents, "IMU6EventPacket");
    imu6_event_packet
        .def(py::init<EventPacketCommon<IMU6EventPacket, IMU6Event>::size_type, int16_t, int32_t>())
        .def(py::init<caerIMU6EventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // IMU9Event
    py::class_<IMU9Event, caer_imu9_event> imu9_event(pyevents, "IMU9Event");
    imu9_event
        .def("getTimestamp", &IMU9Event::getTimestamp)
        .def("getTimestamp64", &IMU9Event::getTimestamp64)
        .def("setTimestamp", &IMU9Event::setTimestamp)
        .def("isValid", &IMU9Event::isValid)
        .def("validate", &IMU9Event::validate)
        .def("invalidate", &IMU9Event::invalidate)
        .def("getAccelX", &IMU9Event::getAccelX)
        .def("setAccelX", &IMU9Event::setAccelX)
        .def("getAccelY", &IMU9Event::getAccelY)
        .def("setAccelY", &IMU9Event::setAccelY)
        .def("getAccelZ", &IMU9Event::getAccelZ)
        .def("getAccelZ", &IMU9Event::getAccelZ)
        .def("getGyroX", &IMU9Event::getGyroX)
        .def("setGyroX", &IMU9Event::setGyroX)
        .def("getGyroY", &IMU9Event::getGyroY)
        .def("setGyroY", &IMU9Event::setGyroY)
        .def("getGyroZ", &IMU9Event::getGyroZ)
        .def("setGyroZ", &IMU9Event::setGyroZ)
        .def("getTemp", &IMU9Event::getTemp)
        .def("setTemp", &IMU9Event::setTemp)
        .def("getCompX", &IMU9Event::getCompX)
        .def("setCompX", &IMU9Event::setCompX)
        .def("getCompY", &IMU9Event::getCompY)
        .def("setCompY", &IMU9Event::setCompY)
        .def("getCompZ", &IMU9Event::getCompZ)
        .def("setCompZ", &IMU9Event::setCompZ);

    // IMU9EventPacket
    py::class_<IMU9EventPacket, EventPacketCommon<IMU9EventPacket, IMU9Event>> imu9_event_packet(pyevents, "IMU9EventPacket");
    imu9_event_packet
        .def(py::init<EventPacketCommon<IMU9EventPacket, IMU9Event>::size_type, int16_t, int32_t>())
        .def(py::init<caerIMU9EventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // Point1DEvent 
    py::class_<Point1DEvent, caer_point1d_event> point1d_event(pyevents, "Point1DEvent");
    point1d_event
        .def("getTimestamp", &Point1DEvent::getTimestamp)
        .def("getTimestamp64", &Point1DEvent::getTimestamp64)
        .def("setTimestamp", &Point1DEvent::setTimestamp)
        .def("isValid", &Point1DEvent::isValid)
        .def("validate", &Point1DEvent::validate)
        .def("invalidate", &Point1DEvent::invalidate)
        .def("getType", &Point1DEvent::getType)
        .def("setType", &Point1DEvent::setType)
        .def("getScale", &Point1DEvent::getScale)
        .def("setScale", &Point1DEvent::setScale)
        .def("getX", &Point1DEvent::getX)
        .def("setX", &Point1DEvent::setX);

    // Point1DEventPacket
    py::class_<Point1DEventPacket, EventPacketCommon<Point1DEventPacket, Point1DEvent>> point1d_event_packet(pyevents, "Point1DEventPacket");
    point1d_event_packet
        .def(py::init<EventPacketCommon<Point1DEventPacket, Point1DEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerPoint1DEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // Point2DEvent
    py::class_<Point2DEvent, caer_point2d_event> point2d_event(pyevents, "Point2DEvent");
    point2d_event
        .def("getTimestamp", &Point2DEvent::getTimestamp)
        .def("getTimestamp64", &Point2DEvent::getTimestamp64)
        .def("setTimestamp", &Point2DEvent::setTimestamp)
        .def("isValid", &Point2DEvent::isValid)
        .def("validate", &Point2DEvent::validate)
        .def("invalidate", &Point2DEvent::invalidate)
        .def("getType", &Point2DEvent::getType)
        .def("setType", &Point2DEvent::setType)
        .def("getScale", &Point2DEvent::getScale)
        .def("setScale", &Point2DEvent::setScale)
        .def("getX", &Point2DEvent::getX)
        .def("setX", &Point2DEvent::setX)
        .def("getY", &Point2DEvent::getY)
        .def("setY", &Point2DEvent::setY);

    // Point2DEventPacket
    py::class_<Point2DEventPacket, EventPacketCommon<Point2DEventPacket, Point2DEvent>> point2d_event_packet(pyevents, "Point2DEventPacket");
    point2d_event_packet
        .def(py::init<EventPacketCommon<Point2DEventPacket, Point2DEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerPoint2DEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // Point3DEvent
    py::class_<Point3DEvent, caer_point3d_event> point3d_event(pyevents, "Point3DEvent");
    point3d_event
        .def("getTimestamp", &Point3DEvent::getTimestamp)
        .def("getTimestamp64", &Point3DEvent::getTimestamp64)
        .def("setTimestamp", &Point3DEvent::setTimestamp)
        .def("isValid", &Point3DEvent::isValid)
        .def("validate", &Point3DEvent::validate)
        .def("invalidate", &Point3DEvent::invalidate)
        .def("getType", &Point3DEvent::getType)
        .def("setType", &Point3DEvent::setType)
        .def("getScale", &Point3DEvent::getScale)
        .def("setScale", &Point3DEvent::setScale)
        .def("getX", &Point3DEvent::getX)
        .def("setX", &Point3DEvent::setX)
        .def("getY", &Point3DEvent::getY)
        .def("setY", &Point3DEvent::setY)
        .def("getZ", &Point3DEvent::getZ)
        .def("setZ", &Point3DEvent::setZ);

    // Point3DEventPacket
    py::class_<Point3DEventPacket, EventPacketCommon<Point3DEventPacket, Point3DEvent>> point3d_event_packet(pyevents, "Point3DEventPacket");
    point3d_event_packet
        .def(py::init<EventPacketCommon<Point3DEventPacket, Point3DEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerPoint3DEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // Point4DEvent
    py::class_<Point4DEvent, caer_point4d_event> point4d_event(pyevents, "Point4DEvent");
    point4d_event
        .def("getTimestamp", &Point4DEvent::getTimestamp)
        .def("getTimestamp64", &Point4DEvent::getTimestamp64)
        .def("setTimestamp", &Point4DEvent::setTimestamp)
        .def("isValid", &Point4DEvent::isValid)
        .def("validate", &Point4DEvent::validate)
        .def("invalidate", &Point4DEvent::invalidate)
        .def("getType", &Point4DEvent::getType)
        .def("setType", &Point4DEvent::setType)
        .def("getScale", &Point4DEvent::getScale)
        .def("setScale", &Point4DEvent::setScale)
        .def("getX", &Point4DEvent::getX)
        .def("setX", &Point4DEvent::setX)
        .def("getY", &Point4DEvent::getY)
        .def("setY", &Point4DEvent::setY)
        .def("getZ", &Point4DEvent::getZ)
        .def("setZ", &Point4DEvent::setZ)
        .def("getW", &Point4DEvent::getW)
        .def("setW", &Point4DEvent::setW);

    // Point4DEventPacket
    py::class_<Point4DEventPacket, EventPacketCommon<Point4DEventPacket, Point4DEvent>> point4d_event_packet(pyevents, "Point4DEventPacket");
    point4d_event_packet
        .def(py::init<EventPacketCommon<Point4DEventPacket, Point4DEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerPoint4DEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // PolarityEvent
    py::class_<PolarityEvent, caer_polarity_event> polarity_event(pyevents, "PolarityEvent");
    polarity_event
        .def("getTimestamp", &PolarityEvent::getTimestamp)
        .def("getTimestamp64", &PolarityEvent::getTimestamp64)
        .def("setTimestamp", &PolarityEvent::setTimestamp)
        .def("isValid", &PolarityEvent::isValid)
        .def("validate", &PolarityEvent::validate)
        .def("invalidate", &PolarityEvent::invalidate)
        .def("getPolarity", &PolarityEvent::getPolarity)
        .def("setPolarity", &PolarityEvent::setPolarity)
        .def("getY", &PolarityEvent::getY)
        .def("setY", &PolarityEvent::setY)
        .def("getX", &PolarityEvent::getX)
        .def("setX", &PolarityEvent::setX);

    // PolarityEventPacket
    py::class_<PolarityEventPacket, EventPacketCommon<PolarityEventPacket, PolarityEvent>> polarity_event_packet(pyevents, "PolarityEventPacket");
    polarity_event_packet
        .def(py::init<EventPacketCommon<PolarityEventPacket, PolarityEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerPolarityEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // SampleEvent
    py::class_<SampleEvent, caer_sample_event> sample_event(pyevents, "SampleEvent");
    sample_event
        .def("getTimestamp", &SampleEvent::getTimestamp)
        .def("getTimestamp64", &SampleEvent::getTimestamp64)
        .def("setTimestamp", &SampleEvent::setTimestamp)
        .def("isValid", &SampleEvent::isValid)
        .def("validate", &SampleEvent::validate)
        .def("invalidate", &SampleEvent::invalidate)
        .def("getType", &SampleEvent::getType)
        .def("setType", &SampleEvent::setType)
        .def("getSample", &SampleEvent::getSample)
        .def("setSample", &SampleEvent::setSample);

    // SampleEventPacket
    py::class_<SampleEventPacket, EventPacketCommon<SampleEventPacket, SampleEvent>> sample_event_packet(pyevents, "SampleEventPacket");
    sample_event_packet
        .def(py::init<EventPacketCommon<SampleEventPacket, SampleEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerSampleEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // SpecialEvent
    py::class_<SpecialEvent, caer_special_event> special_event(pyevents, "SpecialEvent");
    special_event
        .def("getTimestamp", &SpecialEvent::getTimestamp)
        .def("getTimestamp64", &SpecialEvent::getTimestamp64)
        .def("setTimestamp", &SpecialEvent::setTimestamp)
        .def("isValid", &SpecialEvent::isValid)
        .def("validate", &SpecialEvent::validate)
        .def("invalidate", &SpecialEvent::invalidate)
        .def("getType", &SpecialEvent::getType)
        .def("setType", &SpecialEvent::setType)
        .def("getData", &SpecialEvent::getData)
        .def("setData", &SpecialEvent::setData);

    // SpecialEventPacket
    py::class_<SpecialEventPacket, EventPacketCommon<SpecialEventPacket, SpecialEvent>> special_event_packet(pyevents, "SpecialEventPacket");
    special_event_packet
        .def(py::init<EventPacketCommon<SpecialEventPacket, SpecialEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerSpecialEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // SpikeEvent
    py::class_<SpikeEvent, caer_spike_event> spike_event(pyevents, "SpikeEvent");
    spike_event
        .def("getTimestamp", &SpikeEvent::getTimestamp)
        .def("getTimestamp64", &SpikeEvent::getTimestamp64)
        .def("setTimestamp", &SpikeEvent::setTimestamp)
        .def("isValid", &SpikeEvent::isValid)
        .def("validate", &SpikeEvent::validate)
        .def("invalidate", &SpikeEvent::invalidate)
        .def("getSourceCoreID", &SpikeEvent::getSourceCoreID)
        .def("setSourceCoreID", &SpikeEvent::setSourceCoreID)
        .def("getChipID", &SpikeEvent::getChipID)
        .def("setChipID", &SpikeEvent::setChipID)
        .def("getNeuronID", &SpikeEvent::getNeuronID)
        .def("setNeuronID", &SpikeEvent::setNeuronID)
        .def("getX", &SpikeEvent::getX)
        .def("getY", &SpikeEvent::getY);

    // SpikeEventPacket
    py::class_<SpikeEventPacket, EventPacketCommon<SpikeEventPacket, SpikeEvent>> spike_event_packet(pyevents, "SpikeEventPacket");
    spike_event_packet
        .def(py::init<EventPacketCommon<SpikeEventPacket, SpikeEvent>::size_type, int16_t, int32_t>())
        .def(py::init<caerSpikeEventPacket, bool>())
        .def(py::init<caerEventPacketHeader, bool>());

    // Utils
    py::module pyevents_utils = pyevents.def_submodule("utils", "Event utils.");

    pylog.def("makeUniqueFromCStruct", &utils::makeUniqueFromCStruct);
    pylog.def("makeSharedFromCStruct", &utils::makeSharedFromCStruct);

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
    py::class_<davis, usb> davis_device(pydevices, "davis");
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

    py::class_<davisfx2, davis> davisfx2_device(pydevices, "davisfx2");
    davisfx2_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>());

    py::class_<davisfx3, davis> davisfx3_device(pydevices, "davisfx3");
    davisfx3_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>());

    // DVS128
    py::class_<dvs128, usb> dvs128_device(pydevices, "dvs128");
    dvs128_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>())
        .def("infoGet", &dvs128::infoGet);

    // DYNAPSE
    py::class_<dynapse, usb> dynapse_device(pydevices, "dynapse");
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
    py::class_<edvs, serial> edvs_device(pydevices, "edvs");
    edvs_device
        .def(py::init<uint16_t, const std::string &, uint32_t>())
        .def("infoGet", &edvs::infoGet);

}


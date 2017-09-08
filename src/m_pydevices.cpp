/*
 * m_pydevices for supporting devices
 *
 * Author: Yuhuang Hu
 * Email : duguyue100@gmail.com
 */

// PyBind11
#include <pybind11/pybind11.h>

// libcaer C
#include <libcaer/devices/device.h>
#include <libcaer/devices/dvs128.h>

// libcaer CPP
#include <libcaercpp/devices/device.hpp>
#include <libcaercpp/devices/davis.hpp>
#include <libcaercpp/devices/dvs128.hpp>
#include <libcaercpp/devices/dynapse.hpp>
#include <libcaercpp/devices/edvs.hpp>
#include <libcaercpp/events/spike.hpp>
#include <libcaercpp/events/special.hpp>

namespace py = pybind11;
using namespace libcaer::devices;
using namespace libcaer::events;

void pydevices_module(py::module &libpycaer)
{
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

    // USB
    py::class_<usb, device>(pydevices, "usb");

    // Serial
    py::class_<serial, device>(pydevices, "serial");

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
    py::class_<caer_dvs128_info>(pydevices, "caer_dvs128_info")
        .def_readwrite("deviceID", &caer_dvs128_info::deviceID)
        .def_readonly("deviceSerialNumber", &caer_dvs128_info::deviceSerialNumber)
        .def_readwrite("deviceUSBBusNumber", &caer_dvs128_info::deviceUSBBusNumber)
        .def_readwrite("deviceUSBDeviceAddress", &caer_dvs128_info::deviceUSBDeviceAddress)
        .def_readonly("deviceString", &caer_dvs128_info::deviceString)
        .def_readwrite("logicVersion", &caer_dvs128_info::logicVersion)
        .def_readwrite("deviceIsMaster", &caer_dvs128_info::deviceIsMaster)
        .def_readwrite("dvsSizeX", &caer_dvs128_info::dvsSizeX)
        .def_readwrite("dvsSizeY", &caer_dvs128_info::dvsSizeY);

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
        .def("writeSramN", &dynapse::writeSramN)
        .def("writePoissonSpikeRate", &dynapse::writePoissonSpikeRate)
        .def("writeCam", &dynapse::writeCam)
        .def("biasDynapseGenerate", &dynapse::biasDynapseGenerate)
        .def("biasDynapseParse", &dynapse::biasDynapseParse)
        .def("generateCamBits", &dynapse::generateCamBits)
        .def("generateSramBits", &dynapse::generateSramBits)
        .def("coreXYToNeuronId", &dynapse::coreXYToNeuronId)
        .def("coreAddrToNeuronId", &dynapse::coreAddrToNeuronId)
        // .def("spikeEventGetX", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent &)) &dynapse::spikeEventGetX)
        // .def("spikeEventGetY", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent &)) &dynapse::spikeEventGetY)
        // .def("spikeEventGetX", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent *)) &dynapse::spikeEventGetX)
        // .def("spikeEventGetY", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent *)) &dynapse::spikeEventGetY)
        // .def("spikeEventGetX", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent &)) &dynapse::spikeEventGetX)
        // .def("spikeEventGetY", (uint16_t (dynapse::*)(const libcaer::events::SpikeEvent &)) &dynapse::spikeEventGetY)
        .def("spikeEventFromXY", &dynapse::spikeEventFromXY);

    // eDVS
    py::class_<edvs, serial> edvs_device(pydevices, "edvs");
    edvs_device
        .def(py::init<uint16_t, const std::string &, uint32_t>())
        .def("infoGet", &edvs::infoGet);
}

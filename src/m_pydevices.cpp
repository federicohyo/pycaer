/*
 * m_pydevices for supporting devices
 *
 * Author: Yuhuang Hu
 * Email : duguyue100@gmail.com
 */

#include <csignal>
#include <atomic>
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
using namespace std;
using namespace libcaer::devices;
using namespace libcaer::events;


struct sigaction shutdownAction;
static atomic_bool globalShutdown(false);

static void globalShutdownSignalHandler(int signal) {
    // Simply set the running flag to false on SIGTERM and SIGINT (CTRL+C) for global shutdown.
    if (signal == SIGTERM || signal == SIGINT) {
        globalShutdown.store(true);
    }
}

static void usbShutdownHandler(void *ptr) {
    (void)(ptr); // UNUSED.

    globalShutdown.store(true);
}

void signal_handler()
{
    // Install signal handler for global shutdown.
    shutdownAction.sa_handler = &globalShutdownSignalHandler;
    shutdownAction.sa_flags = 0;
    sigemptyset(&shutdownAction.sa_mask);
    sigaddset(&shutdownAction.sa_mask, SIGTERM);
    sigaddset(&shutdownAction.sa_mask, SIGINT);
}

class DVS128Exporter {
    private:
        dvs128 dvsHandle;
        struct caer_dvs128_info dvsInfo;
    public:

        DVS128Exporter() :
        dvsHandle(1, 0, 0, "")
        {
            dvsInfo = dvsHandle.infoGet();
            dvsHandle.sendDefaultConfig();
            dvsHandle.configSet(DVS128_CONFIG_BIAS, DVS128_CONFIG_BIAS_PR, 695);
            dvsHandle.configSet(DVS128_CONFIG_BIAS, DVS128_CONFIG_BIAS_FOLL, 867);
        }

        void exporter_start()
        {
            dvsHandle.dataStart(nullptr, nullptr, nullptr, &usbShutdownHandler, nullptr);
            dvsHandle.configSet(CAER_HOST_CONFIG_DATAEXCHANGE, CAER_HOST_CONFIG_DATAEXCHANGE_BLOCKING, true);
        }


        void export_data()
        {
            if (!globalShutdown.load(memory_order_relaxed))
            {
                std::unique_ptr<EventPacketContainer> packetContainer = dvsHandle.dataGet();

                for (auto &packet : *packetContainer)
                {
                    if (packet->getEventType() == POLARITY_EVENT)
                    {
                        std::shared_ptr<const PolarityEventPacket> polarity = std::static_pointer_cast<PolarityEventPacket>(packet);

                        // Get full timestamp and addresses of first event.
                        const PolarityEvent &firstEvent = (*polarity)[0];

                        int32_t ts = firstEvent.getTimstamp();
                        uint16_t x = firstEvent.getX();
                        uint16_t y = firstEvent.getY();
                        bool pol = firstEvent.getPolarity();

                    }
                }
            }
            else
            {
                printf("no events");
            }
        }

        void exporter_stop()
        {
            dvsHandle.dataStop();
        }

        struct caer_dvs128_info get_info()
        {
            return dvsInfo;
        }
};


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

    py::class_<DVS128Exporter> dvs128_exporter(pydevices, "DVS128Exporter");
    dvs128_exporter
        .def(py::init())
        .def("exporter_start", &DVS128Exporter::exporter_start)
        .def("exporter_stop", &DVS128Exporter::exporter_stop)
        .def("export_data", &DVS128Exporter::export_data)
        .def("get_info", &DVS128Exporter::get_info);

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

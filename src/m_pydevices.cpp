/*
 * m_pydevices for supporting devices
 *
 * Author: Yuhuang Hu
 * Email : duguyue100@gmail.com
 */

#include <csignal>
#include <atomic>
#include <vector>
// PyBind11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// libcaer C
#include <libcaer/devices/device.h>
#include <libcaer/devices/dvs128.h>

// libcaer CPP
#include <libcaercpp/devices/device.hpp>
#include <libcaercpp/devices/davis.hpp>
#include <libcaercpp/devices/dvs128.hpp>
#include <libcaercpp/devices/dynapse.hpp>
#include <libcaercpp/devices/edvs.hpp>
#include <libcaercpp/events/utils.hpp>
#include <libcaercpp/events/polarity.hpp>

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

struct DVSEvent{
    int32_t ts;
    uint16_t x;
    uint16_t y;
    bool pol;
};

struct DYNAPSEEvent {
    int32_t ts;
    uint16_t neuid;
    uint16_t coreid;
    uint16_t chipid;
};

class DVS128Exporter {
    public:
        dvs128 dvsHandle;
        struct caer_dvs128_info dvsInfo;

        DVS128Exporter() :
        dvsHandle(1, CAER_DEVICE_DVS128, 0, "")
        {
            shutdownAction.sa_handler = &globalShutdownSignalHandler;
            shutdownAction.sa_flags = 0;
            sigemptyset(&shutdownAction.sa_mask);
            sigaddset(&shutdownAction.sa_mask, SIGTERM);
            sigaddset(&shutdownAction.sa_mask, SIGINT);

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

        std::vector<DVSEvent> export_data()
        {
            if (!globalShutdown.load(memory_order_relaxed))
            {
                std::unique_ptr<EventPacketContainer> packetContainer = dvsHandle.dataGet();

                std::shared_ptr<EventPacket> packet = (*packetContainer)[-1];
                printf("Packet of type %d -> %d events, %d capacity.\n", packet->getEventType(), packet->getEventNumber(), packet->getEventCapacity());

                if (packet->getEventType() == POLARITY_EVENT)
                {
                    std::shared_ptr<const PolarityEventPacket> polarity = std::static_pointer_cast<PolarityEventPacket>(packet);

                    std::vector<DVSEvent> dvs_events;
                    for (int i=0; i<polarity->getEventNumber(); i++)
                    {
                        const PolarityEvent &curr_event = (*polarity)[i];
                        DVSEvent dvs_event;
                        dvs_event.ts = curr_event.getTimestamp();
                        dvs_event.x = curr_event.getX();
                        dvs_event.y = curr_event.getY();
                        dvs_event.pol = curr_event.getPolarity();
                        dvs_events.push_back(dvs_event); 
                    }
                    return dvs_events;
                }
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

class DYNAPSEExporter {
    public:
		dynapse dynapseHandle;
        struct caer_dynapse_info dynapseInfo;

        DYNAPSEExporter() :
        	dynapseHandle(1, CAER_DEVICE_DYNAPSE, 0, "")
        {
            shutdownAction.sa_handler = &globalShutdownSignalHandler;
            shutdownAction.sa_flags = 0;
            sigemptyset(&shutdownAction.sa_mask);
            sigaddset(&shutdownAction.sa_mask, SIGTERM);
            sigaddset(&shutdownAction.sa_mask, SIGINT);

            dynapseInfo = dynapseHandle.infoGet();
            dynapseHandle.sendDefaultConfig();
        }

        void exporter_start()
        {
        	dynapseHandle.dataStart(nullptr, nullptr, nullptr, &usbShutdownHandler, nullptr);
            dynapseHandle.configSet(CAER_HOST_CONFIG_DATAEXCHANGE, CAER_HOST_CONFIG_DATAEXCHANGE_BLOCKING, true);
        }

        std::vector<DYNAPSEEvent> export_data()
        {
            if (!globalShutdown.load(memory_order_relaxed))
            {
                std::unique_ptr<EventPacketContainer> packetContainer = dynapseHandle.dataGet();

                std::shared_ptr<EventPacket> packet = (*packetContainer)[-1];
                printf("Packet of type %d -> %d events, %d capacity.\n", packet->getEventType(), packet->getEventNumber(), packet->getEventCapacity());

                if (packet->getEventType() == SPIKE_EVENT)
                {
                    std::shared_ptr<const SpikeEventPacket> spike = std::static_pointer_cast<SpikeEventPacket>(packet);

                    std::vector<DYNAPSEEvent> dynapse_events;
                    for (int i=0; i<spike->getEventNumber(); i++)
                    {
                        const SpikeEvent &curr_event = (*spike)[i];
                        DYNAPSEEvent dynapse_event;
                        dynapse_event.ts = curr_event.getTimestamp();
                        dynapse_event.neuid = curr_event.getNeuronID();
                        dynapse_event.coreid = curr_event.getSourceCoreID();
                        dynapse_event.chipid = curr_event.getChipID();
                        dynapse_events.push_back(dynapse_event);
                    }
                    return dynapse_events;
                }
            }
        }

        void exporter_stop()
        {
        	dynapseHandle.dataStop();
        }

        struct caer_dynapse_info get_info()
        {
            return dynapseInfo;
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

    py::class_<DVSEvent>(pydevices, "DVSEvent")
        .def_readonly("ts", &DVSEvent::ts)
        .def_readonly("x", &DVSEvent::x)
        .def_readonly("y", &DVSEvent::y)
        .def_readonly("pol", &DVSEvent::pol);

    py::class_<dvs128, usb> dvs128_device(pydevices, "dvs128");
    dvs128_device
        .def(py::init<uint16_t>())
        .def(py::init<uint16_t, uint8_t, uint8_t, const std::string &>())
        .def("infoGet", &dvs128::infoGet);

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

    py::class_<DYNAPSEEvent>(pydevices, "DYNAPSEEvent")
        .def_readonly("ts", &DYNAPSEEvent::ts)
        .def_readonly("neuid", &DYNAPSEEvent::neuid)
        .def_readonly("coreid", &DYNAPSEEvent::coreid)
        .def_readonly("chipid", &DYNAPSEEvent::chipid);

    py::class_<DYNAPSEExporter> dynapse_exporter(pydevices, "DYNAPSEExporter");
    dynapse_exporter
        .def(py::init())
        .def("exporter_start", &DYNAPSEExporter::exporter_start)
        .def("exporter_stop", &DYNAPSEExporter::exporter_stop)
        .def("export_data", &DYNAPSEExporter::export_data)
        .def("get_info", &DYNAPSEExporter::get_info);

    // eDVS
    py::class_<edvs, serial> edvs_device(pydevices, "edvs");
    edvs_device
        .def(py::init<uint16_t, const std::string &, uint32_t>())
        .def("infoGet", &edvs::infoGet);
}

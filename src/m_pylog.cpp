/*
 * m_pylog for logging functions
 *
 * Author: Yuhuang Hu
 * Email : duguyue100@gmail.com
 */

// PyBind11
#include <pybind11/pybind11.h>

// libcaer CPP
#include <libcaercpp/libcaer.hpp>

namespace py = pybind11;
using namespace libcaer::log;

void pylog_module(py::module &libpycaer)
{
    py::module pylog = libpycaer.def_submodule("log", "The log submodule");
    py::enum_<logLevel>(pylog, "logLevel")
    	.value("EMERGENCY", logLevel::EMERGENCY)
        .value("ALERT", logLevel::ALERT)
        .value("CRITICAL", logLevel::CRITICAL)
        .value("ERROR", logLevel::ERROR)
        .value("WARNING", logLevel::WARNING)
        .value("NOTICE", logLevel::NOTICE)
        .value("INFO", logLevel::INFO)
        .value("DEBUG", logLevel::DEBUG)
        .export_values();

    pylog.def("logLevelSet", &logLevelSet);
    pylog.def("logLevelGet", &logLevelGet);
    pylog.def("fileDescriptorsSet", &fileDescriptorsSet);
    pylog.def("fileDescriptorsGetFirst", &fileDescriptorsGetFirst);
    pylog.def("fileDescriptorsGetSecond", &fileDescriptorsGetSecond);

    pylog.def("log", [](logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        std::string formatted = format.format(*args, **kwargs);
        log(l, subSystem, formatted.c_str());
    });

    pylog.def("logVA", [](logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        va_list argumentList;
        std::string formatted = format.format(*args, **kwargs);

        caerLogVA(static_cast<enum caer_log_level>(static_cast<typename std::underlying_type<logLevel>::type>(l)), subSystem, formatted.c_str(), argumentList);
    });

    pylog.def("logVAFull", [](int logFileDescriptor1, int logFileDescriptor2, uint8_t systemLogLevel, logLevel l, const char *subSystem, py::str format, py::args args, py::kwargs kwargs){
        va_list argumentList;
        std::string formatted = format.format(*args, **kwargs);

        caerLogVAFull(logFileDescriptor1, logFileDescriptor2, systemLogLevel,
                static_cast<enum caer_log_level>(static_cast<typename std::underlying_type<logLevel>::type>(l)), subSystem, formatted.c_str(), argumentList);

    });
}

cmake_minimum_required(VERSION 2.8.12)
project(pycaer)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(LIBCAER REQUIRED libcaer>=2.2.0)

set(PYBIND11_CPP_STANDARD -std=c++11)
add_subdirectory(pybind11)

INCLUDE_DIRECTORIES(${LIBCAER_INCLUDE_DIRS})
LINK_DIRECTORIES(${LIBCAER_LIBRARY_DIRS})

set(PYCAER_SRCS
    src/pycaer.cpp
    src/m_pylog.cpp
    src/m_pynetwork.cpp
    src/m_pyevents.cpp
    src/m_pydevices.cpp)

pybind11_add_module(pycaer ${PYCAER_SRCS})
TARGET_LINK_LIBRARIES(pycaer LINK_PRIVATE caer)

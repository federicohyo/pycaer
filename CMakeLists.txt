cmake_minimum_required(VERSION 2.8.12)
project(pycaer)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(LIBCAER REQUIRED libcaer>=2.2.0)

set(PYBIND11_CPP_STANDARD -std=c++11)
add_subdirectory(pybind11)

INCLUDE_DIRECTORIES(${LIBCAER_INCLUDE_DIRS})
LINK_DIRECTORIES(${LIBCAER_LIBRARY_DIRS})

pybind11_add_module(pycaer src/pycaer.cpp)
TARGET_LINK_LIBRARIES(pycaer LINK_PRIVATE caer)

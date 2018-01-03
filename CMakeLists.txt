cmake_minimum_required(VERSION 2.6)

# MEL CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Updated: 12/2017

#===============================================================================
# HOW TO USE THIS FILE
#===============================================================================

# 1) Set user options below to desired settings
# 2) Open console and navigate to the /build folder
# 3a) To generate makefile for NI hardware, run:
#     $: cmake .. -G "MinGW Makefiles"
# 3b) To generate Visual Studio *.sln file for Quanser hardware, run:
#     $: cmake .. -G "Visual Studio 15 2017 Win64"
# 4) Proceed to build makefile or *.sln file with necessary software

#===============================================================================
# USER OPTIONS
#===============================================================================

option(NI "Turn ON if compiling for NI hardware and NI Realtime Linux" OFF)
option(QUANSER "Turn ON if compiling for Quanser hardware and MSVC" OFF)
option(LEGACY "Turn ON if you need depreciated MEL classes." OFF)
option(MELSHARE "Turn ON to include MelShare. Requires Boost." OFF)
option(EXAMPLES "Turn ON to build example executable(s)" OFF)

#===============================================================================
# SETUP BUILD
#===============================================================================

# set NI RT Linux specific options if enabled
if (NI)
    # set compiler (must be called before project/enable)
    set(CMAKE_C_COMPILER x86_64-nilrt-linux-gcc)
    set(CMAKE_CXX_COMPILER x86_64-nilrt-linux-g++)
    # set to cross compile
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_CROSSCOMPILING 1)
    # setup sysroot (for NI RT cross compiler)
    set(CMAKE_SYSROOT C:/dev/nirlt-linux-g++/sysroots/core2-64-nilrt-linux)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # need pthread for multithreading
endif()

# enable C++11
set(CMAKE_CXX_STANDARD 11)

# create project
project(MEL)

# enable C/C++
enable_language(C)
enable_language(CXX)

# set binary output locations
if(WIN32)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/win32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/win32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/win32)
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/linux)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/linux)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/linux)
endif()

# add definitons
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

#===============================================================================
# PERFORM CHECKS
#===============================================================================

if(NI AND QUANSER)
    message(FATAL_ERROR "FATAL ERROR: MEL cannot be built for NI and Quanser hardware simultaneously. Choose only one.")
endif()

if(QUANSER AND NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"))
    message(FATAL_ERROR "FATAL ERROR: Must use MSVC compiler when using Quanser hardware.")
endif()

if(NI AND NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    message(FATAL_ERROR "FATAL_ERROR: Must use Linux G++ compiler when using NI hardware.")
endif()

if(MELSHARE AND NOT WIN32)
    message(WARNING "WARNING: MelShare can only be compiled on Windows. Disabling MelShare.")
    set(MELSHARE OFF)
endif()

#===============================================================================
# INCLUDES
#===============================================================================

# MEL Lib
set(INCLUDE_DIRS include)

# Hardware Lib(s)
if (NI)
    list(APPEND INCLUDE_DIRS
        C:/dev/nirlt-linux-g++/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2
        C:/dev/nirlt-linux-g++/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2/x86_64-nilrt-linux)
elseif (QUANSER)
    list(APPEND INCLUDE_DIRS
        C:/dev/quarc/include)
endif()

# Boost Lib
if (MELSHARE)
    set(Boost_INCLUDE_DIR C:/dev/boost_1_64_0)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(Boost_LIBRARY_DIR C:/dev/boost_1_64_0/lib64-msvc-14.1)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(Boost_LIBRARY_DIR C:/dev/boost_1_64_0/lib64-mgw-71)
    endif()
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost REQUIRED)
    list(APPEND INCLUDE_DIRS
        ${Boost_INCLUDE_DIR})
endif()

# if (SFML)
#     list(APPEND INCLUDE_DIRS
#     C:/Git/SFML/include)
# endif()

# list(APPEND INCLUDE_DIRS openwrist)

# include directories
include_directories(${INCLUDE_DIRS})

#===============================================================================
# FIND MEL SOURCE FILES
#===============================================================================

file(GLOB SRC_COMMUNICATIONS
    "${CMAKE_SOURCE_DIR}/include/MEL/Communications/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/*.cpp"
)

file(GLOB SRC_CORE
    "${CMAKE_SOURCE_DIR}/include/MEL/Core/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Core/*.cpp"
)

file(GLOB SRC_DAQ
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/*.cpp"
)

file(GLOB SRC_NI
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/NI/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/*.cpp"
)

file(GLOB SRC_QUANSER
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Quanser/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/Quanser/*.cpp"
)

file(GLOB SRC_DEVICES
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/*.cpp"
)

file(GLOB SRC_EXOSKELETONS
    "${CMAKE_SOURCE_DIR}/include/MEL/Exoskeletons/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Exoskeletons/*.cpp"
)

file(GLOB SRC_LEGACY
    "${CMAKE_SOURCE_DIR}/include/MEL/Legacy/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Legacy/*.cpp"
)

file(GLOB SRC_MATH
    "${CMAKE_SOURCE_DIR}/include/MEL/Math/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Math/*.cpp"
)

file(GLOB SRC_UTILITY
    "${CMAKE_SOURCE_DIR}/include/MEL/Utility/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Utility/*.cpp"
)

file(GLOB SRC_WINDOWS
    "${CMAKE_SOURCE_DIR}/include/MEL/Utility/Windows/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Utility/Windows/*.cpp"
)

#===============================================================================
# SELECT SOURCE FILES BASED ON OPTIONS/PLATFORM
#===============================================================================

set(SOURCE_FILES
    ${SRC_COMMUNICATIONS}
    ${SRC_CORE}
    ${SRC_DAQ}
    # ${SRC_DEVICES}
    #${SRC_EXOSKELETONS}
    ${SRC_MATH}
    ${SRC_UTILITY}
)

if(LEGACY)
    list(APPEND SOURCE_FILES
        ${SRC_LEGACY})
endif()

if(WIN32)
    list(APPEND SOURCE_FILES
        ${SRC_WINDOWS})
endif()

if(NI)
    list(APPEND SOURCE_FILES
        ${SRC_NI})
elseif (QUANSER)
    list(APPEND SOURCE_FILES
        ${SRC_QUANSER})
endif()

#===============================================================================
# LINK DIRECTORIES
#===============================================================================

if (QUANSER)
    link_directories(
        C:/dev/quarc/lib/win64)
endif()

if (MELSHARE)
    link_directories(
        ${Boost_LIBRARY_DIR})
endif()

#===============================================================================
# CREATE MEL LIBRARY
#===============================================================================

add_library(MEL STATIC ${SOURCE_FILES})

source_group("Communications" FILES ${SRC_COMMUNICATIONS})
source_group("Core" FILES ${SRC_CORE})
source_group("Daq" FILES ${SRC_DAQ})
source_group("NI" FILES ${SRC_NI})
source_group("Quanser" FILES ${SRC_QUANSER})
source_group("Devices" FILES ${SRC_DEVICES})
source_group("Exoskeletons" FILES ${SRC_EXOSKELETONS})
source_group("Legacy" FILES ${SRC_LEGACY})
source_group("Math" FILES ${SRC_MATH})
source_group("Utility" FILES ${SRC_UTILITY})
source_group("Windows" FILES ${SRC_WINDOWS})

#===============================================================================
# LINK EXTERNAL LIBS
#===============================================================================

set(LINK_LIBS "")

if(NI)
    list(APPEND LINK_LIBS dl)
endif()

if(QUANSER)
    list(APPEND LINK_LIBS
        hil
        quanser_communications
        quanser_runtime
        quanser_common
        bufferoverflowU
        legacy_stdio_definitions) # not sure if needed anymore
endif()

if(WIN32)
    list(APPEND LINK_LIBS ws2_32 winmm Pdh Psapi)
endif()

# link libraries
target_link_libraries(MEL ${LINK_LIBS})

#===============================================================================
# CREATE EXAMPLES EXECUTABLES
#===============================================================================

if(EXAMPLES)

    if(QUANSER)
        add_executable(q8usb examples/q8usb.cpp)
        target_link_libraries(q8usb MEL)
    endif()

    if(NI)
        add_executable(crio examples/crio.cpp)
        target_link_libraries(crio MEL)
    endif()

    # if(MELSHARE)
    #     add_executable(melscope examples/melscope.cpp)
    #     target_link_libraries(melscope MEL)
    # endif()

    add_executable(udp examples/udp.cpp)
    target_link_libraries(udp MEL)

    add_executable(mutex examples/mutexes.cpp)
    target_link_libraries(mutex MEL)

    add_executable(melnet examples/melnet.cpp)
    target_link_libraries(melnet MEL)

    add_executable(melshare examples/melshare.cpp)
    target_link_libraries(melshare MEL)

    add_executable(shared_memory examples/shared_memory.cpp)
    target_link_libraries(shared_memory MEL)

    # add_executable(ctrl_c_handling examples/ctrl_c_handling.cpp)
    # target_link_libraries(ctrl_c_handling MEL)

    # add_executable(openwrist openwrist/main.cpp openwrist/Pendulum.cpp)
    # target_link_libraries(openwrist MEL)

endif()

# testing
add_executable(testing tests/testing.cpp)
target_link_libraries(testing MEL)

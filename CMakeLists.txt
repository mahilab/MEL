# MEL CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 6/2018

#===============================================================================
# HOW TO USE THIS FILE TO BUILD MEL
#===============================================================================

# OPTION 1: WITH SUBLIME TEXT
# 1) Open the MEL Sublime project file (util/MEL.sublime-project) with Sublime
#
# 2) Run the "Clean" build system (CTRL+SHIFT+B -> Clean) to remove existing
#    build files and/or generate the necessary folders
#
# 3) Turn on the desired build options below by changing "OFF" to "ON"
#
# 4) Run one of the provided CMake (...) build systems to generate build files
#    for the desired build toolchain:
#
#    a) CMake (MSVC) for Quanser, NI DAQ, or Sensoray hardware
#    b) CMake (Make) or CMake (Ninja) for embedded NI cRIO, NI myRIO
#
# 5) Run the matching Build (...) build system to compile MEL
#
# 6) If you decide to change options, run the Reset Cache build system first

cmake_minimum_required(VERSION 3.7)

#===============================================================================
# USER OPTIONS
#===============================================================================

option(QUANSER    "Turn ON if compiling for Quanser hardware"                  OFF) # e.g. Q8 USB, Q2 USB
option(NI_DAQ     "Turn ON if compiling for NI DAQmx compatible hardware"      OFF) # e.g. NI myDAQ
option(NI_X64     "Turn ON if compiling for NI Intel x64 embeded hardware"     OFF) # e.g. NI cRIO
option(NI_ARM     "Turn ON if compiling for NI Intel x64 embeded hardware"     OFF) # e.g. NI myRIO
option(SENSORAY   "Turn ON if compiling for Sensoray hardware"                 OFF) # e.g. Sensoray 826
option(MYO        "Turn ON if compiling for Thalmic Labs Myo Armband"          OFF)
option(EXAMPLES   "Turn ON to build example executable(s)"                     OFF)
option(TESTS      "Turn ON to build test executable(s)"                        OFF)
option(BUILD_DOC  "Turn ON to build MEL documentation"                         OFF)

set(QUANSER_ROOT  "/Program Files/Quanser"                    CACHE FILEPATH "Absolute path to Quanser installation directory.")
set(NI_ROOT       "/Program Files (x86)/National Instruments" CACHE FILEPATH "Absolute path to NI installation directory.")
set(NI_X64_ROOT   "/dev/nirlt-linux-x64"                      CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (x64).")
set(NI_ARM_ROOT   "/dev/nirlt-linux-arm"                      CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (ARM).")
set(SENSORAY_ROOT "/Program Files (x86)/Sensoray"             CACHE FILEPATH "Absolute path to Sensoray installation directory.")
set(MYO_ROOT      "/dev/myo-sdk-win-0.9.0"                    CACHE FILEPATH "Absolute path to Myo SDK.")

#===============================================================================
# FIND MEL SOURCE FILES
#===============================================================================

set(INCLUDE_DIRS include)
set(LINK_DIRS "")
set(LINK_LIBS "")

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

file(GLOB SRC_ENGINE
    "${CMAKE_SOURCE_DIR}/include/MEL/Engine/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Engine/*.cpp"
)

file(GLOB SRC_NI_MYRIO
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/NI/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/myRIO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/myRIO/Detail/*.h"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/myRIO/Detail/*.c"
)

file(GLOB SRC_QUANSER
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Quanser/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/Quanser/*.cpp"
)

file(GLOB SRC_SENSORAY
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Sensoray/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/Sensoray/*.cpp"
)

file(GLOB SRC_DEVICES
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/*.cpp"
)

file(GLOB SRC_MYO
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/Myo/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/Myo/*.cpp"
)

file(GLOB SRC_LOGGING
    "${CMAKE_SOURCE_DIR}/include/MEL/Logging/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Logging/*.cpp"
)

file(GLOB SRC_LOGGING_FORMATTERS
    "${CMAKE_SOURCE_DIR}/include/MEL/Logging/Formatters/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Logging/Formatters/*.cpp"
)

file(GLOB SRC_LOGGING_WRITERS
    "${CMAKE_SOURCE_DIR}/include/MEL/Logging/Writers/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Logging/Writers/*.cpp"
)

file(GLOB SRC_MATH
    "${CMAKE_SOURCE_DIR}/include/MEL/Math/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Math/*.cpp"
)

file(GLOB SRC_UTILITY
    "${CMAKE_SOURCE_DIR}/include/MEL/Utility/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Utility/*.cpp"
)

file(GLOB SRC_UTILITY_WINDOWS
    "${CMAKE_SOURCE_DIR}/include/MEL/Utility/Windows/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Utility/Windows/*.cpp"
)

# create source groups
source_group("Communications" FILES ${SRC_COMMUNICATIONS})
source_group("Core" FILES ${SRC_CORE})
source_group("Daq" FILES ${SRC_DAQ})
source_group("Daq\\NI" FILES ${SRC_NI_MYRIO})
source_group("Daq\\Quanser" FILES ${SRC_QUANSER})
source_group("Daq\\Sensoray" FILES ${SRC_SENSORAY})
source_group("Devices" FILES ${SRC_DEVICES})
source_group("Engine" FILES ${SRC_ENGINE})
source_group("Devices\\Myo" FILES ${SRC_MYO})
source_group("Legacy" FILES ${SRC_LEGACY})
source_group("Logging" FILES ${SRC_LOGGING})
source_group("Logging\\Formatters" FILES ${SRC_LOGGING_FORMATTERS})
source_group("Logging\\Writers" FILES ${SRC_LOGGING_WRITERS})
source_group("Math" FILES ${SRC_MATH})
source_group("Utility" FILES ${SRC_UTILITY})
source_group("Utility\\Windows" FILES ${SRC_UTILITY_WINDOWS})

set(SOURCE_FILES
    ${SRC_COMMUNICATIONS}
    ${SRC_CORE}
    ${SRC_DAQ}
    ${SRC_DEVICES}
    ${SRC_ENGINE}
    ${SRC_LOGGING}
    ${SRC_LOGGING_FORMATTERS}
    ${SRC_LOGGING_WRITERS}
    ${SRC_MATH}
    ${SRC_UTILITY}
)

#===============================================================================
# SETUP CROSS COMPILERS
#===============================================================================

# set NI RT Linux specific options if enabled
if (NI_X64)
    # add compiler to path
    LIST(APPEND CMAKE_PROGRAM_PATH ${NI_X64_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/x86_64-nilrt-linux)
    # set compiler (must be called before project/enable)
    set(CMAKE_C_COMPILER x86_64-nilrt-linux-gcc)
    set(CMAKE_CXX_COMPILER x86_64-nilrt-linux-g++)
    # set to cross compile
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_CROSSCOMPILING 1)
    # setup sysroot (for NI RT cross compiler)
    set(CMAKE_SYSROOT ${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux)
elseif (NI_ARM)
    # add compiler to path
    LIST(APPEND CMAKE_PROGRAM_PATH ${NI_ARM_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/arm-nilrt-linux-gnueabi)
    # set compiler (must be called before project/enable)
    set(CMAKE_C_COMPILER arm-nilrt-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER arm-nilrt-linux-gnueabi-g++)
    # set to cross compile
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_CROSSCOMPILING 1)
    # setup sysroot (for NI RT cross compiler)
    set(CMAKE_SYSROOT ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi)
endif()

#===============================================================================
# BASIC SETUP
#===============================================================================

# enable C/C++
enable_language(C)
enable_language(CXX)

# enable C++11
set(CMAKE_CXX_STANDARD 11)

# create project
project(MEL)

# set binary output locations
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#===============================================================================
# PLATFORM SPECIFIC SETUP
#===============================================================================

set(MEL_64BIT OFF)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MEL_64BIT ON)
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    list(APPEND LINK_LIBS ws2_32 winmm Pdh Psapi XInput)
    list(APPEND SOURCE_FILES ${SRC_UTILITY_WINDOWS})
endif()

#===============================================================================
# COMPILER SPECIFIC SETUP
#===============================================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # all warnings
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4") # warning level 4
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP") # multicore build
    set(CMAKE_CONFIGURATION_TYPES Release)
endif()

#===============================================================================
# HARDWARE SPECIFIC SETUP
#===============================================================================

if (QUANSER)
    # hardware includes
    list(APPEND INCLUDE_DIRS "${QUANSER_ROOT}/QUARC/include")
    # hardware libs
    if (MEL_64BIT)
        list(APPEND LINK_DIRS "${QUANSER_ROOT}/QUARC/lib/win64")
    else()
        list(APPEND LINK_DIRS "${QUANSER_ROOT}/QUARC/lib/windows")
    endif()
    list(APPEND LINK_LIBS hil quanser_communications quanser_runtime quanser_common bufferoverflowU legacy_stdio_definitions)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_QUANSER})
endif()

if(NI_DAQ)
    # hardware includes
    list(APPEND INCLUDE_DIRS "${NI_ROOT}/NI-DAQ/DAQmx ANSI C Dev/include")
    # hardware libs
    list(APPEND LINK_DIRS "${NI_ROOT}/NI-DAQ/DAQmx ANSI C Dev/lib/msvc")
    list(APPEND LINK_LIBS NIDAQmx.lib)
endif()

if (NI_X64)
    # compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    # hardware includes
    list(APPEND INCLUDE_DIRS
        "${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2"
        "${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2/x86_64-nilrt-linux")
    # hardware libs
    list(APPEND LINK_LIBS dl rt)
endif()

if(NI_ARM)
    # compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -mfpu=vfpv3 -mfloat-abi=softfp")
    # hardware inclues
    list(APPEND INCLUDE_DIRS
        "${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2"
        "${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2/arm-nilrt-linux-gnueabi")
    # hardware libs
    list(APPEND LINK_LIBS dl rt)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_NI_MYRIO})
endif()

if (SENSORAY)
    # hardware includes
    list(APPEND INCLUDE_DIRS "${SENSORAY_ROOT}/826/API")
    if (MEL_64BIT)
        list(APPEND LINK_DIRS "${SENSORAY_ROOT}/826/API/x64")
    else()
        list(APPEND LINK_DIRS "${SENSORAY_ROOT}/826/API/x32")
    endif()
    list(APPEND LINK_LIBS s826)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_SENSORAY})
endif()

if (MYO)
    # hardware includes
    list(APPEND INCLUDE_DIRS "${MYO_ROOT}/include")
    # hardware libs
    list(APPEND LINK_DIRS "${MYO_ROOT}/lib")
    if (MEL_64BIT)
        list(APPEND LINK_LIBS myo64)
    else()
        list(APPEND LINK_LIBS myo32)
    endif()
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_MYO})
endif()

#===============================================================================
# CREATE MEL LIBRARY
#===============================================================================

include_directories(${INCLUDE_DIRS})
add_library(MEL STATIC ${SOURCE_FILES})
link_directories(${LINK_DIRS})
target_link_libraries(MEL ${LINK_LIBS})

#===============================================================================
# EXECUTABLES
#===============================================================================

if(EXAMPLES)
    add_subdirectory(examples)
endif()

if(TESTS)
    add_subdirectory(tests)
endif()

#==============================================================================
# DOCUMENTATION
#==============================================================================

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

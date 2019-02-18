# MEL CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 2/2019

cmake_minimum_required(VERSION 3.7)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MEL_STATIC  "Turn ON to build MEL as a static library (default shared)"                  OFF)
option(DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"                 OFF)
option(EXAMPLES    "Turn ON to build example executable(s)"                                     OFF)
option(MOVE_BINS   "Turn ON to move binaries to conventional bin/lib folders after compile"     OFF)
option(BUILD_DOC   "Turn ON to build MEL documentation"                                         OFF)

# Quanser HIL (# e.g. Q8 USB, Q2 USB)
option(QUANSER   "Turn ON if compiling for Quanser hardware" OFF) 
set(QUANSER_ROOT "/Program Files/Quanser" CACHE FILEPATH "Absolute path to Quanser installation directory.")

# NI DAQmx (e.g. myDAQ)
option(NI_DAQMX "Turn ON if compiling for NI DAQmx compatible hardware"  OFF) 
set(NI_DAQMX_ROOT "/Program Files (x86)/National Instruments" CACHE FILEPATH "Absolute path to NI installation directory.")

# Thalmic Labs Myro Armband
option(MYO   "Turn ON if compiling for Thalmic Labs Myo Armband" OFF)
set(MYO_ROOT "/dev/myo-sdk-win-0.9.0" CACHE FILEPATH "Absolute path to Myo SDK.")

#===============================================================================
# FIND MEL SOURCE FILES
#===============================================================================

set(INCLUDE_DIRS include)
set(LINK_DIRS "")
set(LINK_LIBS "")

file (GLOB SRC_BASE
    "${CMAKE_SOURCE_DIR}/include/MEL/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/*.cpp"
)

file(GLOB SRC_COMMUNICATIONS
    "${CMAKE_SOURCE_DIR}/include/MEL/Communications/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/Detail/*.h"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/Detail/*.c"
)

file(GLOB SRC_CORE
    "${CMAKE_SOURCE_DIR}/include/MEL/Core/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Core/*.cpp"
)

file(GLOB SRC_DAQ
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/*.cpp"
)

file(GLOB SRC_DAQ_DETAIL
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Detail/*.inl"
)

file(GLOB SRC_NI_MYRIO
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/NI/MyRio/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/MyRIO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/MyRIO/Detail/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/MyRIO/Detail/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/MyRIO/Detail/MyRioFpga60/*.h"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/MyRIO/Detail/MyRioFpga60/*.c"
)

file(GLOB SRC_QUANSER
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Quanser/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/Quanser/*.cpp"
)

file(GLOB SRC_DEVICES
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/*.cpp"
)

file(GLOB SRC_DEVICES_MYO
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/Myo/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/Myo/*.cpp"
)

set(SRC_KEYBOARD_WINDOWS
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/Windows/Keyboard.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/Windows/Keyboard.cpp"
)

set(SRC_XINPUT_WINDOWS
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/Windows/XboxController.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/Windows/XboxController.cpp"
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

file(GLOB SRC_MECHATRONICS
    "${CMAKE_SOURCE_DIR}/include/MEL/Mechatronics/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Mechatronics/*.cpp"
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
source_group("\\" FILES ${SRC_BASE})
source_group("Communications" FILES ${SRC_COMMUNICATIONS})
source_group("Core" FILES ${SRC_CORE})
source_group("Daq" FILES ${SRC_DAQ})
source_group("Daq\\Detail" FILES ${SRC_DAQ_DETAIL})
source_group("Daq\\NI" FILES ${SRC_NI_MYRIO})
source_group("Daq\\Quanser" FILES ${SRC_QUANSER})
source_group("Devices" FILES ${SRC_DEVICES})
source_group("Devices\\Myo" FILES ${SRC_DEVICES_MYO})
source_group("Devices\\Windows" FILES ${SRC_KEYBOARD_WINDOWS} ${SRC_XINPUT_WINDOWS})
source_group("Legacy" FILES ${SRC_LEGACY})
source_group("Logging" FILES ${SRC_LOGGING})
source_group("Logging\\Formatters" FILES ${SRC_LOGGING_FORMATTERS})
source_group("Logging\\Writers" FILES ${SRC_LOGGING_WRITERS})
source_group("Mechatronics" FILES ${SRC_MECHATRONICS})
source_group("Math" FILES ${SRC_MATH})
source_group("Utility" FILES ${SRC_UTILITY})
source_group("Utility\\Windows" FILES ${SRC_UTILITY_WINDOWS})

set(SOURCE_FILES
    ${SRC_BASE}
    ${SRC_COMMUNICATIONS}
    ${SRC_CORE}
    ${SRC_DAQ}
    ${SRC_DAQ_DETAIL}
    ${SRC_DEVICES}
    ${SRC_LOGGING}
    ${SRC_LOGGING_FORMATTERS}
    ${SRC_LOGGING_WRITERS}
    ${SRC_MATH}
    ${SRC_MECHATRONICS}
    ${SRC_UTILITY}
)

#===============================================================================
# BASIC SETUP
#===============================================================================

# enable C/C++ 11
enable_language(C)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

# create project
project(MEL)

# turn static compilation on/off
if (MEL_STATIC)
    add_definitions(-DMEL_STATIC)
endif()

# turn default logger on/off
if (DISABLE_LOG)
    add_definitions(-DMEL_DISABLE_LOG)
endif()

# set binary output locations
if (MOVE_BINS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

#===============================================================================
# PLATFORM SPECIFIC SETUP
#===============================================================================

set(MEL_64BIT OFF)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MEL_64BIT ON)
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    list(APPEND LINK_LIBS ws2_32 winmm Pdh Psapi)
    list(APPEND SOURCE_FILES ${SRC_UTILITY_WINDOWS} ${SRC_KEYBOARD_WINDOWS})
    # XInput for Xbox controllers
    find_library(LIB_XINPUT XInput)
    if (LIB_XINPUT)
        list(APPEND LINK_LIBS XInput)
        list(APPEND SOURCE_FILES ${SRC_XINPUT_WINDOWS})
        message(STATUS "Found XInput")
    else()
        message(WARNING "Could not find XInput")
    endif()
endif()

#===============================================================================
# COMPILER SPECIFIC SETUP
#===============================================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # all warnings
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4") # warning level 4
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP") # multicore build
endif()

set(default_build_type "Release") # set default configurationi to Release

#===============================================================================
# HARDWARE SPECIFIC SETUP
#===============================================================================

if (QUANSER)
    # for users with full QUARC installation
    if (EXISTS "${QUANSER_ROOT}/QUARC/")
        # hardware includes
        list(APPEND INCLUDE_DIRS "${QUANSER_ROOT}/QUARC/include")
        # hardware libs
        if (MEL_64BIT)
            list(APPEND LINK_DIRS "${QUANSER_ROOT}/QUARC/lib/win64")
        else()
            list(APPEND LINK_DIRS "${QUANSER_ROOT}/QUARC/lib/windows")
        endif()
    # for users with HIL SDK standalone
    else()
        # hardware includes
        list(APPEND INCLUDE_DIRS "${QUANSER_ROOT}/HIL SDK/include")
        # hardware libs
        if (MEL_64BIT)
            list(APPEND LINK_DIRS "${QUANSER_ROOT}/HIL SDK/lib/win64")
        else()
            list(APPEND LINK_DIRS "${QUANSER_ROOT}/HIL SDK/lib/windows")
        endif()
    endif()
    list(APPEND LINK_LIBS hil quanser_runtime quanser_common bufferoverflowU legacy_stdio_definitions)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_QUANSER})
endif()

if(NI_DAQMX)
    # hardware includes
    list(APPEND INCLUDE_DIRS "${NI_DAQMX_ROOTNI-DAQ/DAQmx ANSI C Dev/include")
    # hardware libs
    list(APPEND LINK_DIRS "${NI_DAQMX_ROOTNI-DAQ/DAQmx ANSI C Dev/lib/msvc")
    list(APPEND LINK_LIBS NIDAQmx.lib)
endif()

if (MEL_NI_X64)
    # compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    # hardware includes
    list(APPEND INCLUDE_DIRS
        "${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2"
        "${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2/x86_64-nilrt-linux")
    # hardware libs
    list(APPEND LINK_LIBS dl rt)
    SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
endif()

if(MEL_NI_ARM)
    # compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -march=armv7-a -mfpu=vfpv3  -mfloat-abi=softfp -mcpu=cortex-a9")
    # hardware inclues
    list(APPEND INCLUDE_DIRS
        "${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2"
        "${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/4.9.2/arm-nilrt-linux-gnueabi")
    # hardware libs
    list(APPEND LINK_LIBS dl rt)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_NI_MYRIO})
    SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
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
    list(APPEND SOURCE_FILES ${SRC_DEVICES_MYO})
endif()

#===============================================================================
# CREATE MEL LIBRARY
#===============================================================================

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

include_directories(${INCLUDE_DIRS})
link_directories(${LINK_DIRS})
if (MEL_STATIC)
    add_library(MEL STATIC ${SOURCE_FILES})
else()
    add_library(MEL SHARED ${SOURCE_FILES})
    target_compile_definitions(MEL PRIVATE -DMEL_EXPORTS)
endif()
target_link_libraries(MEL ${LINK_LIBS})
set_target_properties(MEL PROPERTIES FOLDER "MEL")
# define MEL_EXPORTS for DLL exports
target_compile_definitions(MEL PRIVATE MyRio_1900)

#===============================================================================
# EXECUTABLES
#===============================================================================

if(EXAMPLES)
    add_subdirectory(examples)
endif()

#==============================================================================
# DOCUMENTATION
#==============================================================================

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

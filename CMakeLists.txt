# MEL CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 8/2018

#===============================================================================
# HOW TO USE THIS FILE TO BUILD MEL
#===============================================================================

# OPTION 1: SUBLIME TEXT
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

# hardware options
option(QUANSER     "Turn ON if compiling for Quanser hardware"                                  ON ) # e.g. Q8 USB, Q2 USB
option(NI_DAQ      "Turn ON if compiling for NI DAQmx compatible hardware"                      OFF) # e.g. NI myDAQ
option(NI_X64      "Turn ON if compiling for NI Intel x64 embeded hardware"                     OFF) # e.g. NI cRIO
option(NI_ARM      "Turn ON if compiling for NI Intel x64 embeded hardware"                     OFF) # e.g. NI myRIO
option(MYO         "Turn ON if compiling for Thalmic Labs Myo Armband"                          OFF)
# general options
option(MEL_STATIC  "Turn ON to build MEL as a static library (default shared)"                  OFF)
option(DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"                 OFF)
option(EXAMPLES    "Turn ON to build example executable(s)"                                     ON )
option(TESTS       "Turn ON to build test executable(s)"                                        ON )
option(MOVE_BINS   "Turn ON to move binaries to conventional bin/lib folders are compilation"   OFF)
option(BUILD_DOC   "Turn ON to build MEL documentation"                                         OFF)
# third party paths
set(QUANSER_ROOT   "/Program Files/Quanser"                    CACHE FILEPATH "Absolute path to Quanser installation directory.")
set(NI_ROOT        "/Program Files (x86)/National Instruments" CACHE FILEPATH "Absolute path to NI installation directory.")
set(NI_X64_ROOT    "/dev/nirlt-linux-x64"                      CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (x64).")
set(NI_ARM_ROOT    "/dev/nirlt-linux-arm"                      CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (ARM).")
set(SENSORAY_ROOT  "/Program Files (x86)/Sensoray"             CACHE FILEPATH "Absolute path to Sensoray installation directory.")
set(MYO_ROOT       "/dev/myo-sdk-win-0.9.0"                    CACHE FILEPATH "Absolute path to Myo SDK.")

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
)

file(GLOB SRC_CORE
    "${CMAKE_SOURCE_DIR}/include/MEL/Core/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Core/*.cpp"
)

file(GLOB SRC_DAQ
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/*.cpp"
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
source_group("\\" FILES ${SRC_BASE})
source_group("Communications" FILES ${SRC_COMMUNICATIONS})
source_group("Core" FILES ${SRC_CORE})
source_group("Daq" FILES ${SRC_DAQ})
source_group("Daq\\NI" FILES ${SRC_NI_MYRIO})
source_group("Daq\\Quanser" FILES ${SRC_QUANSER})
source_group("Daq\\Sensoray" FILES ${SRC_SENSORAY})
source_group("Devices" FILES ${SRC_DEVICES})
source_group("Devices\\Myo" FILES ${SRC_MYO})
source_group("Legacy" FILES ${SRC_LEGACY})
source_group("Logging" FILES ${SRC_LOGGING})
source_group("Logging\\Formatters" FILES ${SRC_LOGGING_FORMATTERS})
source_group("Logging\\Writers" FILES ${SRC_LOGGING_WRITERS})
source_group("Math" FILES ${SRC_MATH})
source_group("Utility" FILES ${SRC_UTILITY})
source_group("Utility\\Windows" FILES ${SRC_UTILITY_WINDOWS})

set(SOURCE_FILES
    ${SRC_BASE}
    ${SRC_COMMUNICATIONS}
    ${SRC_CORE}
    ${SRC_DAQ}
    ${SRC_DEVICES}
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
endif()

set(default_build_type "Release") # set default configurationi to Release

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
    SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
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
    SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
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

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

include_directories(${INCLUDE_DIRS})
link_directories(${LINK_DIRS})
if (MEL_STATIC)
    add_library(MEL STATIC ${SOURCE_FILES})
else()
    add_library(MEL SHARED ${SOURCE_FILES})
endif()
target_link_libraries(MEL ${LINK_LIBS})
set_target_properties(MEL PROPERTIES FOLDER "MEL")
# define MEL_EXPORTS for DLL exports
target_compile_definitions(MEL PRIVATE -DMEL_EXPORTS)

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

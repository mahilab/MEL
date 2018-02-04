cmake_minimum_required(VERSION 3.7)

# MEL CMakeLists.txt
# Evan Pezent (epezent@rice.edu)
# Updated: 1/2018

#===============================================================================
# HOW TO USE THIS FILE
#===============================================================================

# 1) Open console and navigate to the /build folder
# 2) To generate makefile for NI hardware, run:
#        $: cmake .. -G "Ninja" -DNI=ON
#    To generate Visual Studio MEL.sln file for Quanser hardware, run:
#        $: cmake .. -G "Visual Studio 15 2017 Win64" -DQUANSER=ON
# 3) To build OpenWrist or MAHI Exo-II classes, append -DOPENWRIST=ON or -DMAHIEXOII=ON
# 3) To build example programs in ./examples, append -DEXAMPLES=ON
# 4) Proceed to build generated makefile or MEL.sln file with the necessary software

#===============================================================================
# USER OPTIONS
#===============================================================================

option(NI        "Turn ON if compiling for NI hardware and NI Realtime Linux" OFF)
option(QUANSER   "Turn ON if compiling for Quanser hardware and MSVC"         OFF)
option(OPENWRIST "Turn ON to include OpenWrist source code"                   OFF)
option(MAHIEXOII "Turn ON to include MAHI Exo-II source code"                 OFF)
option(EXAMPLES  "Turn ON to build example executable(s)"                     OFF)
option(BUILD_DOC "Turn ON to build MEL documentation"                         OFF)
option(TESTS     "Turn ON to build test executable(s)"                        OFF)
option(LEGACY    "Turn ON if you need depreciated MEL classes."               OFF)

set(NI_ROOT      "C:/dev/nirlt-linux"       CACHE FILEPATH "Absolute path to NI Linux Realtime GNU compiler.")
set(QUANSER_ROOT "C:/Program Files/Quanser" CACHE FILEPATH "Absolute path to Quanser installation."          )
set(EIGEN_ROOT   "C:/dev/eigen"             CACHE FILEPATH "Absolute path to Eigen installation."            )

#===============================================================================
# SETUP BUILD
#===============================================================================

# set NI RT Linux specific options if enabled
if (NI)
    # add compiler to path
    LIST(APPEND CMAKE_PROGRAM_PATH ${NI_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/x86_64-nilrt-linux)
    # set compiler (must be called before project/enable)
    set(CMAKE_C_COMPILER x86_64-nilrt-linux-gcc)
    set(CMAKE_CXX_COMPILER x86_64-nilrt-linux-g++)
    # set to cross compile
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_CROSSCOMPILING 1)
    # setup sysroot (for NI RT cross compiler)
    set(CMAKE_SYSROOT ${NI_ROOT}/sysroots/core2-64-nilrt-linux)
    # need pthread for multithreading
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# enable C/C++
enable_language(C)
enable_language(CXX)

# enable C++11
set(CMAKE_CXX_STANDARD 11)

# create project
project(MEL)

# set binary output locations
if(WIN32)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/windows)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/windows)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/windows)
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/linux)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/linux)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/linux)
endif()

# add definitons
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

if (QUANSER)
    add_definitions(-DMEL_INCLUDE_QUANSER)
endif()

if (MAHIEXOII)
    add_definitions(-DMEL_INCLUDE_EIGEN)
endif()

# add configurations
# set(CMAKE_CONFIGURATION_TYPES release)

#===============================================================================
# COMPILER SPECIFIC
#===============================================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4") # WARNING LEVEL 4
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP") # MULTICORE BUILDS

endif()

#===============================================================================
# PERFORM CHECKS
#===============================================================================

if(NI AND QUANSER)
    message(FATAL_ERROR "FATAL ERROR: MEL cannot be built for NI and Quanser hardware simultaneously. Choose only one.")
endif()

if(NI AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    message(FATAL_ERROR "FATAL_ERROR: Must use Linux G++ compiler when using NI hardware.")
endif()

if(QUANSER AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "MSVC"))
    message(FATAL_ERROR "FATAL ERROR: Must use MSVC compiler when using Quanser hardware.")
endif()

#===============================================================================
# INCLUDES
#===============================================================================

# MEL Lib
set(INCLUDE_DIRS include)

# Hardware Lib(s)
if (NI)
    list(APPEND INCLUDE_DIRS
        ${NI_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2
        ${NI_ROOT}/sysroots/core2-64-nilrt-linux/usr/include/c++/4.9.2/x86_64-nilrt-linux)
elseif (QUANSER)
    list(APPEND INCLUDE_DIRS "${QUANSER_ROOT}/QUARC/include")
endif()

if (MAHIEXOII)
    list(APPEND INCLUDE_DIRS ${EIGEN_ROOT})
endif()

# include directories
include_directories(${INCLUDE_DIRS})

#===============================================================================
# FIND MEL SOURCE FILES
#===============================================================================

file(GLOB SRC_COMMUNICATIONS
    "${CMAKE_SOURCE_DIR}/include/MEL/Communications/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/*.cpp"
)

file(GLOB SRC_COMMUNICATIONS_WINDOWS
    "${CMAKE_SOURCE_DIR}/include/MEL/Communications/Windows/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Communications/Windows/*.cpp"
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
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/NI/*.h"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/NI/*.c"
)

file(GLOB SRC_QUANSER
    "${CMAKE_SOURCE_DIR}/include/MEL/Daq/Quanser/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Daq/Quanser/*.cpp"
)

file(GLOB SRC_DEVICES
    "${CMAKE_SOURCE_DIR}/include/MEL/Devices/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Devices/*.cpp"
)

file(GLOB SRC_OPENWRIST
    "${CMAKE_SOURCE_DIR}/include/MEL/Exoskeletons/OpenWrist/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Exoskeletons/OpenWrist/*.cpp"
)

file(GLOB SRC_MAHIEXOII
    "${CMAKE_SOURCE_DIR}/include/MEL/Exoskeletons/MahiExoII/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Exoskeletons/MahiExoII/*.cpp"
)

file(GLOB SRC_LEGACY
    "${CMAKE_SOURCE_DIR}/include/MEL/Legacy/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Legacy/*.cpp"
)

file(GLOB SRC_LOGGING
    "${CMAKE_SOURCE_DIR}/include/MEL/Logging/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Logging/*.cpp"
)

file(GLOB SRC_LOGGING_CONVERTERS
    "${CMAKE_SOURCE_DIR}/include/MEL/Logging/Converters/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/MEL/Logging/Converters/*.cpp"
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

# create filters for Visual Studio
source_group("Communications" FILES ${SRC_COMMUNICATIONS})
source_group("Communications\\Windows" FILES ${SRC_COMMUNICATIONS_WINDOWS})
source_group("Core" FILES ${SRC_CORE})
source_group("Daq" FILES ${SRC_DAQ})
source_group("Daq\\NI" FILES ${SRC_NI})
source_group("Daq\\Quanser" FILES ${SRC_QUANSER})
source_group("Devices" FILES ${SRC_DEVICES})
source_group("Exoskeletons\\OpenWrist" FILES ${SRC_OPENWRIST})
source_group("Exoskeletons\\MahiExoII" FILES ${SRC_MAHIEXOII})
source_group("Legacy" FILES ${SRC_LEGACY})
source_group("Logging" FILES ${SRC_LOGGING})
source_group("Logging\\Converters" FILES ${SRC_LOGGING_CONVERTERS})
source_group("Logging\\Formatters" FILES ${SRC_LOGGING_FORMATTERS})
source_group("Logging\\Writers" FILES ${SRC_LOGGING_WRITERS})

source_group("Math" FILES ${SRC_MATH})
source_group("Utility" FILES ${SRC_UTILITY})
source_group("Utility\\Windows" FILES ${SRC_UTILITY_WINDOWS})

#===============================================================================
# SELECT SOURCE FILES BASED ON OPTIONS/PLATFORM
#===============================================================================

set(SOURCE_FILES
    ${SRC_COMMUNICATIONS}
    ${SRC_CORE}
    ${SRC_DAQ}
    ${SRC_DEVICES}
    ${SRC_LOGGING}
    ${SRC_LOGGING_CONVERTERS}
    ${SRC_LOGGING_FORMATTERS}
    ${SRC_LOGGING_WRITERS}
    ${SRC_MATH}
    ${SRC_UTILITY}
)

if(OPENWRIST)
    list(APPEND SOURCE_FILES ${SRC_OPENWRIST})
endif()

if(MAHIEXOII)
    list(APPEND SOURCE_FILES ${SRC_MAHIEXOII})
endif()

if(LEGACY)
    list(APPEND SOURCE_FILES ${SRC_LEGACY})
endif()

if(WIN32)
    list(APPEND SOURCE_FILES ${SRC_COMMUNICATIONS_WINDOWS})
    list(APPEND SOURCE_FILES ${SRC_UTILITY_WINDOWS})
endif()

if(NI)
    list(APPEND SOURCE_FILES ${SRC_NI})
elseif (QUANSER)
    list(APPEND SOURCE_FILES ${SRC_QUANSER})
endif()

#===============================================================================
# CREATE MEL LIBRARY
#===============================================================================

add_library(MEL STATIC ${SOURCE_FILES})

#===============================================================================
# LINK EXTERNAL LIBS
#===============================================================================

set(LINK_LIBS "")

if(NI)
    list(APPEND LINK_LIBS dl)
elseif(QUANSER)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
        link_directories("${QUANSER_ROOT}/QUARC/lib/win64")
    else() # 32-bit
        link_directories("${QUANSER_ROOT}/QUARC/lib/windows")
    endif()
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

target_link_libraries(MEL ${LINK_LIBS})

#===============================================================================
# ADD SUBDIRECTORIES
#===============================================================================

if(EXAMPLES)
    add_subdirectory(examples)
endif()

if(TESTS)
    add_executable(testing tests/testing.cpp)
    target_link_libraries(testing MEL)
endif()

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

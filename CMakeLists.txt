# MEL CMakeLists.txt
# Evan Pezent     (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 2/2019

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MEL_STATIC      "Turn ON to build MEL as a static library (default shared)"              OFF)
option(MEL_DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"             OFF)
option(MEL_EXAMPLES    "Turn ON to build example executable(s)"                                 OFF)
option(MEL_MOVE_BINS   "Turn ON to move binaries to conventional bin/lib folders after compile" OFF)
option(MEL_BUILD_DOC   "Turn ON to build MEL documentation"                                     OFF)
# Hardware Support
option(MEL_QUANSER     "Turn ON if compiling for Quanser hardware"                              OFF) 
option(MEL_NIDAQMX     "Turn ON if compiling for NI DAQmx compatible hardware"                  OFF) 
option(MEL_MYO         "Turn ON if compiling for Thalmic Labs Myo Armband"                      OFF)

#===============================================================================
# FRONT MATTER
#===============================================================================

# add custom CMake modules
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

# enable C/C++ 11
enable_language(C)
enable_language(CXX)
# set(CMAKE_CXX_STANDARD 11)

# create project
project(MEL VERSION 1.2.0 LANGUAGES CXX)

# set binary output locations
if (MEL_MOVE_BINS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

#===============================================================================
# CREATE MEL LIBRARY
#===============================================================================

# Build MEL as shared or static library?
if (MEL_STATIC)
    add_library(mel STATIC "")
    target_compile_definitions(mel PUBLIC  -DMEL_STATIC)
else()
    add_library(mel SHARED "")
    target_compile_definitions(mel PRIVATE -DMEL_EXPORTS)
endif()

# alias 
add_library(MEL::mel ALIAS mel)

# turn default logger on/off
if (MEL_DISABLE_LOG)
    target_compile_definitions(mel PRIVATE -DMEL_DISABLE_LOG)
endif()

# include directories
target_include_directories(mel
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src                
)

# compile features
# complete listing:
# https://cmake.org/cmake/help/v3.14/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
target_compile_features(mel
    PUBLIC
        cxx_std_11
    PRIVATE
)

#===============================================================================
# FIND MEL SOURCE FILES
#===============================================================================

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

# base source files
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
# SOURCE GROUPS / IDE FOLDERS
#===============================================================================

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

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_target_properties(mel PROPERTIES FOLDER "MEL")

#===============================================================================
# PLATFORM SPECIFIC SETUP
#===============================================================================

# Windows
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    target_link_libraries(mel
        PRIVATE
            ws2_32 # Windows Sockets 2 (for Socket, UDP, TCP, etc.)
            winmm  # Windows Multimedia API (for high-precision timers)
            Pdh    # Windows Performance Data Helper (for performance queries)
            Psapi  # Windows Process Status API (for performance queries)
            XInput # Windows XInput (for Xbox 360/One controller support)
    )
    list(APPEND SOURCE_FILES ${SRC_UTILITY_WINDOWS} ${SRC_KEYBOARD_WINDOWS} ${SRC_XINPUT_WINDOWS})
endif()

# National Instruments Linux Realt-Time ARM/x64
if (MEL_NI_X64 OR MEL_NI_ARM)
    target_link_libraries(mel
        PRIVATE
            dl # Dynamic Linking Library
            rt # Realtime Extensions library
    )
    if (MEL_NI_ARM)
        target_compile_definitions(mel PRIVATE MyRio_1900) # needed by NI src
        # append MEL source
        list(APPEND SOURCE_FILES ${SRC_NI_MYRIO})
    endif()
endif()

#===============================================================================
# HARDWARE SPECIFIC SETUP
#===============================================================================

# Quanser HIL SDK
if (MEL_QUANSER)
    # find HIL
    find_package(HIL REQUIRED)
    target_link_libraries(mel
        PRIVATE
            HIL::HIL              # HIL SDK
            HIL::quanser_runtime  # HIL quanser_runtime
            HIL::quanser_common   # HIL quanser_common
            bufferoverflowU  
            legacy_stdio_definitions
    )
    # append MEL source
    list(APPEND SOURCE_FILES ${SRC_QUANSER})
endif()

# National Insruments DAQmx
if(MEL_NIDAQMX)
    find_package(NIDAQmx REQUIRED)
    target_link_libraries(mel
        PRIVATE
            NIDAQmx::NIDAQmx      # NIDAQmx
    )
endif()

# Thalmic Labs Myo SDK
if (MEL_MYO)
    find_package(myo REQUIRED)
    target_link_libraries(mel
        PRIVATE
            myo::myo              # myo
    )
    # append MEL source
    list(APPEND SOURCE_FILES ${SRC_DEVICES_MYO})
endif()

#===============================================================================
# SET MEL SOURCE FILES
#===============================================================================

# source files
target_sources(mel PRIVATE ${SOURCE_FILES})

#===============================================================================
# EXECUTABLES
#===============================================================================

if(MEL_EXAMPLES)
    add_subdirectory(examples)
endif()

#==============================================================================
# DOCUMENTATION
#==============================================================================

if(MEL_BUILD_DOC)
    add_subdirectory(doc)
endif()

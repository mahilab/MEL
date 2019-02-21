# MEL CMakeLists.txt
# Evan Pezent     (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 2/2019

cmake_minimum_required(VERSION 3.7)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MEL_STATIC      "Turn ON to build MEL as a static library (default shared)"              OFF)
option(MEL_DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"             OFF)
option(MEL_EXAMPLES    "Turn ON to build example executable(s)"                                 OFF)
option(MEL_MOVE_BINS   "Turn ON to move binaries to conventional bin/lib folders after compile" OFF)
option(MEL_BUILD_DOC   "Turn ON to build MEL documentation"                                     OFF)

# Hardware
option(MEL_QUANSER "Turn ON if compiling for Quanser hardware"             OFF) 
option(MEL_NIDAQMX "Turn ON if compiling for NI DAQmx compatible hardware" OFF) 
option(MEL_MYO     "Turn ON if compiling for Thalmic Labs Myo Armband"     OFF)

#===============================================================================
# FRONT MATTER
#===============================================================================

# add custom CMake modules
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

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
if (MEL_DISABLE_LOG)
    add_definitions(-DMEL_DISABLE_LOG)
endif()

# set binary output locations
if (MEL_MOVE_BINS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

#===============================================================================
# PLATFORM SPECIFIC SETUP
#===============================================================================

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

if (MEL_QUANSER)
    # find HIL
    find_package(HIL REQUIRED)
    list(APPEND LINK_LIBS 
        HIL::HIL 
        HIL::quanser_runtime 
        HIL::quanser_common  
        bufferoverflowU 
        legacy_stdio_definitions
    )
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_QUANSER})
endif()

if(MEL_NIDAQMX)
    find_package(NIDAQmx REQUIRED)
    list(APPEND LINK_LIBS NIDAQmx::NIDAQmx)
endif()

if (MEL_MYO)
    find_package(MYO REQUIRED)
    list(APPEND LINK_LIBS MYO::MYO)
    # MEL source
    list(APPEND SOURCE_FILES ${SRC_DEVICES_MYO})
endif()

if (MEL_NI_X64)
    list(APPEND LINK_LIBS dl rt)
endif()

if(MEL_NI_ARM)
    list(APPEND LINK_LIBS dl rt)
    list(APPEND SOURCE_FILES ${SRC_NI_MYRIO})
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
git starget_compile_definitions(MEL PRIVATE MyRio_1900)

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

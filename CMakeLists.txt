# MEL CMakeLists.txt
# Evan Pezent     (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 2/2019

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MEL_EXAMPLES    "Turn ON to build example executable(s)"                                 OFF)
option(MEL_DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"             OFF)
option(MEL_BUILD_DOC   "Turn ON to build MEL documentation"                                     OFF)

#===============================================================================
# FRONT MATTER
#===============================================================================

# create the MEL project
project(MEL VERSION 1.2.0 LANGUAGES C CXX)

# tell user they can't have shared version of MEL if they try
if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "MEL does not yet support shared libaries")
endif()

# add ./cmake to CMake module path so our .cmake files can be found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

include(Macros)         # MEL macros
include(MELSources)     # lists of MEL source and header files
include(GNUInstallDirs) # defines conventional GNU isntallation directories

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

#===============================================================================
# CREATE COMMON MEL LIBRARY
#===============================================================================

# create core MEL library (MEL::MEL)
mel_add_library(MEL SOURCES ${MEL_COMMON_SRC} ${MEL_COMMON_HEADERS})

# turn default logger on/off
if (MEL_DISABLE_LOG)
    message("Disabling MEL logger")
    target_compile_definitions(MEL PRIVATE -DMEL_DISABLE_LOG)
endif()

#===============================================================================
# WINDOWS ONLY
#===============================================================================

if(WIN32)
    target_compile_definitions(MEL
        PUBLIC
            -D_CRT_SECURE_NO_WARNINGS          # remove secure warnings (e.g sprinft_s)
            -DNOMINMAX                         # remove min/max macros
            -D_WINSOCK_DEPRECATED_NO_WARNINGS  # remove winsock deprecated warnings
    )

    target_link_libraries(MEL
        PRIVATE
            ws2_32 # Windows Sockets 2 (for Socket, UDP, TCP, etc.)
            winmm  # Windows Multimedia API (for high-precision timers)
            Pdh    # Windows Performance Data Helper (for performance queries)
            Psapi  # Windows Process Status API (for performance queries)
            XInput # Windows XInput (for Xbox 360/One controller support)
    )

    # append windows specific sources to MEL::MEL
    target_sources(MEL PRIVATE ${MEL_WINDOWS_SRC} ${MEL_WINDOWS_HEADERS})

    # the following only compile under MSVC
    if (MSVC)

        #=======================================================================
        # HIL / QUANSER
        #=======================================================================
        find_package(HIL)
        if (HIL_FOUND)
            option(MEL_QUANSER "Turn ON to build MEL Quanser support"  ON) 
        else()
            option(MEL_QUANSER "Turn ON to build MEL Quanser support"  OFF) 
        endif()
        if (MEL_QUANSER)
            # create Quanser library module
            mel_add_library(quanser SOURCES ${MEL_QUANSER_SRC} ${MEL_QUANSER_HEADERS})
            target_link_libraries(quanser
                PUBLIC
                    MEL
                PRIVATE
                    HIL::HIL                 # HIL SDK
                    HIL::quanser_runtime     # HIL quanser_runtime
                    HIL::quanser_common      # HIL quanser_common
                    legacy_stdio_definitions # needed for _vsnprintf in quanser_common
            )
        endif()

        #=======================================================================
        # NI DAQMX
        #=======================================================================
        find_package(NIDAQmx)
        if (NIDAQmx_FOUND)
            option(MEL_NIDAQMX "Turn ON to build MEL NI DAQmx support" ON)  
        else()
            option(MEL_NIDAQMX "Turn ON to build MEL NI DAQmx support" OFF)
        endif() 
        if(MEL_NIDAQMX)
            # target_link_libraries(MEL
            #     PRIVATE
            #         NIDAQmx::NIDAQmx      # NIDAQmx
            # )
        endif()

        #=======================================================================
        # EPOS
        #=======================================================================
        find_package(EPOS)
        if (EPOS_FOUND)
            option(MEL_EPOS "Turn ON to build MEL EPOS support" ON)  
        else()
            option(MEL_EPOS "Turn ON to build MEL NI DAQmx support" OFF)
        endif() 
        if(MEL_EPOS)
            target_link_libraries(epos
                PUBLIC
                    MEL
                PRIVATE
                    EPOS::EPOS     # EPOS
            )
        endif()

        #=======================================================================
        # THALMIC LABS MYO
        #=======================================================================
        find_package(Myo)
        if (Myo_FOUND)
            option(MEL_MYO "Turn ON to build MEL Myo suppport" ON)  
        else()
            option(MEL_MYO "Turn ON to build MEL Myo suppport" OFF)  
        endif()
        if (MEL_MYO)
            mel_add_library(myo SOURCES ${MEL_MYO_SRC} ${MEL_MYO_HEADERS})
            target_link_libraries(myo
                PUBLIC
                    MEL
                PRIVATE
                    Myo::Myo # myo
            )
        endif()

    endif(MSVC)

endif(WIN32)

#===============================================================================
# NI LINUX REAL-TIME ONLY
#===============================================================================

# detect incompatible options
mel_incompatible(MEL_NI_LRT MEL_QUANSER)
mel_incompatible(MEL_NI_LRT MEL_NIDAQMX)
mel_incompatible(MEL_NI_LRT MEL_MYO)

if (NI_LRT)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    target_link_libraries( MEL
        PRIVATE
            dl # Dynamic Linking Library
            rt # Realtime Extensions library
    )
    if (CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
        mel_add_library(myrio SOURCES ${MEL_MYRIO_SRC} ${MEL_MYRIO_HEADERS})
        target_link_libraries(myrio PUBLIC MEL)
        target_compile_definitions(myrio PRIVATE MyRio_1900) # needed by NI src
    endif()
endif()

#===============================================================================
# EXAMPLE EXECUTABLES
#===============================================================================

if(MEL_EXAMPLES)
    message("Building MEL examples")
    add_subdirectory(examples)
endif()

#===============================================================================
# INSTALL
#===============================================================================

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MEL_INSTALL_SUBDIR "msvc-x64")
    else()
        set(MEL_INSTALL_SUBDIR "msvc-x86")
    endif()
elseif(NI_LRT AND CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
    set(MEL_INSTALL_SUBDIR "nilrt-arm")
elseif(NI_LRT AND CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
    set(MEL_INSTALL_SUBDIR "nilrt-x64")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}-${MEL_INSTALL_SUBDIR}")

# install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# set where we want to install our config
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MEL)

# export the targets to a script
install(EXPORT MEL-targets
	FILE
		MELTargets.cmake
	NAMESPACE
		MEL::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

# include helper functions for creating config files that can be included by other projects to find and use a package
include(CMakePackageConfigHelpers)

# generate a package configuration file and a package version file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MELConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/MELConfig.cmake
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/MELConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

# install the config and configversion modules
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/MELConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/MELConfigVersion.cmake
	DESTINATION ${INSTALL_CONFIGDIR}
)

# install find modules
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindHIL.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindMyo.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindNIDAQmx.cmake
    DESTINATION ${INSTALL_CONFIGDIR}/Modules
)

# export from the build tree
export(EXPORT MEL-targets
	   NAMESPACE MEL::
	   FILE ${CMAKE_CURRENT_BINARY_DIR}/MELTargets.cmake)

#==============================================================================
# DOCUMENTATION
#==============================================================================

if(MEL_BUILD_DOC)
    message("Building MEL documentation")
    add_subdirectory(doc)
endif()

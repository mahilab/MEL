# MEL CMakeLists.txt
# Evan Pezent     (epezent@rice.edu)
# Craig McDonalod (craig.g.mcdonald@gmail.com)
# Updated: 2/2019

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# General
option(MEL_SHARED      "Turn ON to build MEL as a shared library (default static)"              OFF)
option(MEL_DISABLE_LOG "Turn ON to disable MEL's default console/file debug logger"             OFF)
option(MEL_EXAMPLES    "Turn ON to build example executable(s)"                                 OFF)
option(MEL_MOVE_BINS   "Turn ON to move binaries to conventional bin/lib folders after compile" OFF)
option(MEL_BUILD_DOC   "Turn ON to build MEL documentation"                                     OFF)

#===============================================================================
# FRONT MATTER
#===============================================================================

# create project
project(MEL VERSION 1.2.0 LANGUAGES C CXX)

# add custom CMake modules and macros
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
include(Macros)

# set binary output locations
if (MEL_MOVE_BINS)
    message("MEL libraries will be output to ${CMAKE_SOURCE_DIR}/lib")
    message("MEL binaries will be output to ${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# include GNU install dirs
include(GNUInstallDirs)

# include MEL source files (sets MEL_COMMON_SRC and MEL_COMMON_HEADERS among others)
include(MELSources)

#===============================================================================
# CREATE COMMON MEL LIBRARY
#===============================================================================

# does the user want a shared version?
if (MEL_SHARED)
    set(BUILD_SHARED_LIBS ON)
endif()

# create core MEL library
mel_add_library(MEL)
target_sources(MEL PRIVATE ${MEL_COMMON_SRC} ${MEL_COMMON_HEADERS})


# turn default logger on/off
if (MEL_DISABLE_LOG)
    message("Disabling MEL logger")
    target_compile_definitions(MEL PRIVATE -DMEL_DISABLE_LOG)
endif()

# source groups / IDE folders
mel_source_group_by_folder(MEL)

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_target_properties(MEL PROPERTIES FOLDER "MEL")

#===============================================================================
# WINDOWS ONLY
#===============================================================================

if(WIN32)
    target_compile_definitions( MEL
        PUBLIC
            -D_CRT_SECURE_NO_WARNINGS          # remove secure warnings (e.g sprinft_s)
            -DNOMINMAX                         # remove min/max macros
            -D_WINSOCK_DEPRECATED_NO_WARNINGS  # remove winsock deprecated warnings
    )

    target_link_libraries( MEL
        PRIVATE
            ws2_32 # Windows Sockets 2 (for Socket, UDP, TCP, etc.)
            winmm  # Windows Multimedia API (for high-precision timers)
            Pdh    # Windows Performance Data Helper (for performance queries)
            Psapi  # Windows Process Status API (for performance queries)
            XInput # Windows XInput (for Xbox 360/One controller support)
    )

    target_sources(MEL PRIVATE ${MEL_WINDOWS_SRC} ${MEL_WINDOWS_HEADERS})

    #===========================================================================
    # HIL / QUANSER
    #===========================================================================
    find_package(HIL)
    if (HIL_FOUND)
        option(MEL_QUANSER "Turn ON to build MEL with Quanser support"  ON) 
    else()
        option(MEL_QUANSER "Turn ON to build MEL with Quanser support"  OFF) 
    endif()
    if (MEL_QUANSER)
        message("Building MEL with Quanser support")
        # find HIL
        target_link_libraries(MEL
            PRIVATE
                HIL::HIL                 # HIL SDK
                HIL::quanser_runtime     # HIL quanser_runtime
                HIL::quanser_common      # HIL quanser_common
                legacy_stdio_definitions # needed for _vsnprintf in quanser_common
        )
        target_sources(MEL PRIVATE ${MEL_QUANSER_SRC} ${MEL_QUANSER_HEADERS})
    endif()

    #===========================================================================
    # NI DAQMX
    #===========================================================================
    find_package(NIDAQmx)
    if (NIDAQmx_FOUND)
        option(MEL_NIDAQMX "Turn ON to build MEL with NI DAQmx support" ON)  
    else()
        option(MEL_NIDAQMX "Turn ON to build MEL with NI DAQmx support" OFF)
    endif() 
    if(MEL_NIDAQMX)
        message("Building MEL with NI-DAQmx support")
        target_link_libraries(MEL
            PRIVATE
                NIDAQmx::NIDAQmx      # NIDAQmx
        )
    endif()

    #===========================================================================
    # THALMIC LABS MYO
    #===========================================================================
    find_package(Myo)
    if (Myo_FOUND)
        option(MEL_MYO "Turn ON to build MEL with Myo suppport" ON)  
    else()
        option(MEL_MYO "Turn ON to build MEL with Myo suppport" ON)  
    endif()
    if (MEL_MYO)
        message("Building MEL with Myo support")
        target_link_libraries(MEL
            PRIVATE
                Myo::Myo              # myo
        )
        target_sources(MEL PRIVATE ${MEL_MYO_SRC} ${MEL_MYO_HEADERS})
    endif()

endif()

#===============================================================================
# NI LINUX REAL-TIME ONLY
#===============================================================================

# detect incompatible options
mel_incompatible(MEL_NI_LRT MEL_QUANSER)
mel_incompatible(MEL_NI_LRT MEL_NIDAQMX)
mel_incompatible(MEL_NI_LRT MEL_MYO)

if (MEL_NI_LRT)
    target_link_libraries( MEL
        PRIVATE
            dl # Dynamic Linking Library
            rt # Realtime Extensions library
    )
    if (CMAKE_SYSTEM_PROCESSOR MATCHES ARM)
        message("Building MEL with myRIO support")
        target_compile_definitions(MEL PRIVATE MyRio_1900) # needed by NI src
        target_sources(MEL PRIVATE ${MEL_MYRIO_SRC} ${MEL_MYRIO_HEADERS})
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

# install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# set where  we want to install our confgig
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

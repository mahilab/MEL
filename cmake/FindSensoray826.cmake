#  MIT License
# 
#  MEL - Mechatronics Engine & Library
#  Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
# 
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  Author(s): Evan Pezent (epezent@rice.edu)

# Locates Sensoray 826 SDK
# This module defines
#   Sensoray826_FOUND
#   Sensoray826_LIBRARY
#   Sensoray826_INCLUDE_DIR

set(Sensoray826_ROOT "C:/Program Files (x86)/Sensoray/826")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(Sensoray826_ARCH_PATH "API/x64")
else()
  SET(Sensoray826_ARCH_PATH "API/x32")
endif()

# find includes
find_path(Sensoray826_INCLUDE_DIR 
			NAMES "826api.h"
      PATHS ${Sensoray826_ROOT}
      PATH_SUFFIXES "API")

# find lib
find_library(Sensoray826_LIBRARY 
			NAMES "s826"
            PATHS ${Sensoray826_ROOT}
            PATH_SUFFIXES ${Sensoray826_ARCH_PATH})          

# if (Sensoray826_INCLUDE_DIR)
#   get_filename_component(PARENT_DIR ${Sensoray826_INCLUDE_DIR} DIRECTORY)
#   set(Sensoray826_INCLUDE_DIR ${PARENT_DIR})
# endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sensoray826 REQUIRED_VARS Sensoray826_LIBRARY Sensoray826_INCLUDE_DIR)

if (Sensoray826_FOUND AND NOT Sensoray::Sensoray826)
    add_library(Sensoray::Sensoray826 STATIC IMPORTED)
    set_target_properties(Sensoray::Sensoray826 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${Sensoray826_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Sensoray826_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(Sensoray826_LIBRARY Sensoray826_INCLUDE_DIR)

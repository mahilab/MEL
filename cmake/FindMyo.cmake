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

# Locates Locates Thalmic Lab's Myo SDK
# This module defines
#   NIDAQmx_FOUND
#   NIDAQmx_LIBRARY
#   NIDAQmx_INCLUDE_DIR

set(MYO_ROOT "/dev/myo-sdk-win-0.9.0")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(MYO_LIB_NAMES "myo64")
else()
  SET(MYO_LIB_NAMES "myo32")
endif()

# find includes
find_path(MYO_INCLUDE_DIR 
			NAMES "myo.hpp"
      PATHS ${MYO_ROOT}
      PATH_SUFFIXES "include/myo")

# find lib
find_library(MYO_LIBRARY 
			NAMES ${MYO_LIB_NAMES}
            PATHS ${MYO_ROOT}
            PATH_SUFFIXES "lib")          

if (MYO_INCLUDE_DIR)
  get_filename_component(PARENT_DIR ${MYO_INCLUDE_DIR} DIRECTORY)
  set(MYO_INCLUDE_DIR ${PARENT_DIR})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MYO REQUIRED_VARS MYO_LIBRARY MYO_INCLUDE_DIR)

if (MYO_FOUND AND NOT MYO::MYO)
    add_library(MYO::MYO STATIC IMPORTED)
    set_target_properties(MYO::MYO PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${MYO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MYO_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(MYO_LIBRARY MYO_INCLUDE_DIR)

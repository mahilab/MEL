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

# Locates Locates Thalmic Lab's myo SDK
# This module defines
#   Myo_FOUND
#   Myo_LIBRARY
#   Myo_INCLUDE_DIR

set(Myo_ROOT "C:/Program Files/myo-sdk-win-0.9.0")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(Myo_LIB_NAMES "myo64")
else()
  SET(Myo_LIB_NAMES "myo32")
endif()

# find includes
find_path(Myo_INCLUDE_DIR 
			NAMES "myo.hpp"
      PATHS ${Myo_ROOT}
      PATH_SUFFIXES "include/myo")

# find lib
find_library(Myo_LIBRARY 
			NAMES ${Myo_LIB_NAMES}
            PATHS ${Myo_ROOT}
            PATH_SUFFIXES "lib")          

if (Myo_INCLUDE_DIR)
  get_filename_component(PARENT_DIR ${Myo_INCLUDE_DIR} DIRECTORY)
  set(Myo_INCLUDE_DIR ${PARENT_DIR})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Myo REQUIRED_VARS Myo_LIBRARY Myo_INCLUDE_DIR)

if (Myo_FOUND AND NOT Myo::Myo)
    add_library(Myo::Myo STATIC IMPORTED)
    set_target_properties(Myo::Myo PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${Myo_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Myo_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(Myo_LIBRARY Myo_INCLUDE_DIR)

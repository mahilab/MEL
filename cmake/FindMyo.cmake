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
#   myo_FOUND
#   myo_LIBRARY
#   myo_INCLUDE_DIR

set(myo_ROOT "/dev/myo-sdk-win-0.9.0")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(myo_LIB_NAMES "myo64")
else()
  SET(myo_LIB_NAMES "myo32")
endif()

# find includes
find_path(myo_INCLUDE_DIR 
			NAMES "myo.hpp"
      PATHS ${myo_ROOT}
      PATH_SUFFIXES "include/myo")

# find lib
find_library(myo_LIBRARY 
			NAMES ${myo_LIB_NAMES}
            PATHS ${myo_ROOT}
            PATH_SUFFIXES "lib")          

if (myo_INCLUDE_DIR)
  get_filename_component(PARENT_DIR ${myo_INCLUDE_DIR} DIRECTORY)
  set(myo_INCLUDE_DIR ${PARENT_DIR})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(myo REQUIRED_VARS myo_LIBRARY myo_INCLUDE_DIR)

if (myo_FOUND AND NOT myo::myo)
    add_library(myo::myo STATIC IMPORTED)
    set_target_properties(myo::myo PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${myo_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${myo_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(myo_LIBRARY myo_INCLUDE_DIR)

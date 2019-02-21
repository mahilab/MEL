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

# Locates National Instrument's DAQmx (NI-DAQmx) library
# This module defines
#   NIDAQmx_FOUND
#   NIDAQmx_LIBRARY
#   NIDAQmx_INCLUDE_DIR

set(NIDAQmx_ROOT "/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(NIDAQmx_PATH_SUFFIXES "lib64/msvc")
else()
  SET(NIDAQmx_PATH_SUFFIXES "lib32/msvc")
endif()

# find includes
find_path(NIDAQmx_INCLUDE_DIR 
			NAMES "NIDAQmx.h"
            PATHS ${NIDAQmx_ROOT}
            PATH_SUFFIXES "include")

# find lib
find_library(NIDAQmx_LIBRARY 
			NAMES "NIDAQmx"
            PATHS ${NIDAQmx_ROOT}
            PATH_SUFFIXES ${NIDAQmx_PATH_SUFFIXES})          

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NIDAQmx REQUIRED_VARS NIDAQmx_LIBRARY NIDAQmx_INCLUDE_DIR)

if (NIDAQmx_FOUND AND NOT NIDAQmx::NIDAQmx)
    add_library(NIDAQmx::NIDAQmx STATIC IMPORTED)
    set_target_properties(NIDAQmx::NIDAQmx PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${NIDAQmx_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${NIDAQmx_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(NIDAQmx_INCLUDE_DIR NIDAQmx_LIBRARY)

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
#  Author(s): Zane Zook (gadzooks@rice.edu)

# Locates Maxon Motor's EPOS (EposCmd) library
# This module defines
#   EPOS_FOUND
#   EPOS_LIBRARY
#   EPOS_INCLUDE_DIR

set(EPOS_ROOT "C:/Program Files (x86)/maxon motor ag/EPOS Positioning Controller/EPOS4/04 Programming/Windows DLL/Microsoft Visual C++/Definition Files")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(EPOS_PATH_SUFFIXES "Cmd64")
else()
  SET(EPOS_PATH_SUFFIXES "Cmd")
endif()

# find includes
find_path(EPOS_INCLUDE_DIR 
			NAMES "Definitions.h"
            PATHS ${EPOS_ROOT}
            PATH_SUFFIXES "include")

# find lib
find_library(EPOS_LIBRARY 
			NAMES "EPOS"
            PATHS ${EPOS_ROOT}
            PATH_SUFFIXES ${EPOS_PATH_SUFFIXES})          

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NIDAQmx REQUIRED_VARS NIDAQmx_LIBRARY NIDAQmx_INCLUDE_DIR)

if (EPOS_FOUND AND NOT EPOS::EPOS)
    add_library(EPOS::EPOS STATIC IMPORTED)
    set_target_properties(EPOS::EPOS PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${EPOS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${EPOS_INCLUDE_DIR}"
    )
endif()
    
mark_as_advanced(EPOS_INCLUDE_DIR EPOS_LIBRARY)
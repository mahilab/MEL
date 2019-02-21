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
#  Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

# Locates Quanser's Hardware In the Loop (HIL) library
# This module defines
# HIL_FOUND,
# HIL_LIBRARIES,
# HIL_INCLUDE_DIRS, where to find hil.h 

set(HIL_ROOT "/Program Files/Quanser")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(HIL_PATH_SUFFIXES "lib" "lib/win64")
else()
  SET(HIL_PATH_SUFFIXES "lib" "lib/windows")
endif()

find_path(HIL_INCLUDE_DIR hil.h
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES include
  PATHS
  ""
)

find_library(HIL_LIBRARY 
  NAMES hil libhil
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ""
)

find_library(QUANSER_COMMON_LIBRARY 
  NAMES quanser_common libquanser_common
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ""
)

find_library(QUANSER_RUNTIME_LIBRARY 
  NAMES quanser_runtime libquanser_runtime
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ""
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HIL
  REQUIRED_VARS HIL_LIBRARY QUANSER_COMMON_LIBRARY QUANSER_RUNTIME_LIBRARY HIL_INCLUDE_DIR
)

if (HIL_FOUND AND NOT HIL::HIL)
  add_library(HIL::HIL STATIC IMPORTED)
  set_target_properties(HIL::HIL PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${HIL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${HIL_INCLUDE_DIR}"
  )
endif()

if (HIL_FOUND AND NOT HIL::quanser_common)
  add_library(HIL::quanser_common STATIC IMPORTED)
  set_target_properties(HIL::quanser_common PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${QUANSER_COMMON_LIBRARY}"
  )
endif()

if (HIL_FOUND AND NOT HIL::quanser_runtime)
  add_library(HIL::quanser_runtime STATIC IMPORTED)
  set_target_properties(HIL::quanser_runtime PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${QUANSER_RUNTIME_LIBRARY}"
  )
endif()

mark_as_advanced(HIL_INCLUDE_DIR HIL_LIBRARY QUANSER_COMMON_LIBRARY QUANSER_RUNTIME_LIBRARY)

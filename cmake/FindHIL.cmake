# Locate Quanser's Hardware In the Loop (HIL) Library
# This module defines
# HIL_FOUND,
# HIL_LIBRARIES,
# HIL_INCLUDE_DIRS, where to find hil.h 

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
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

find_library(HIL_LIBRARY 
  NAMES hil libhil
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

find_library(QUANSER_COMMON_LIBRARY 
  NAMES quanser_common libquanser_common
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

find_library(QUANSER_RUNTIME_LIBRARY 
  NAMES quanser_runtime libquanser_runtime
  HINTS
  $ENV{HIL_DIR}
  PATH_SUFFIXES ${HIL_PATH_SUFFIXES}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

mark_as_advanced(HIL_INCLUDE_DIR HIL_LIBRARY QUANSER_COMMON_LIBRARY QUANSER_RUNTIME_LIBRARY)

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
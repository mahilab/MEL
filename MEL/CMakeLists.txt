cmake_minimum_required(VERSION 3.7)
project(MEL)

# set standard to C++11
set(CMAKE_CXX_STANDARD 11)

# prevents errors about unsafe functions
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# link Boost library
set(Boost_INCLUDE_DIR C:/dev/boost_1_64_0)
set(Boost_LIBRARY_DIR C:/dev/boost_1_64_0/lib64-msvc-14.1)
set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# link Quarc library
include_directories(C:/dev/quarc/include)
link_directories(C:/dev/quarc/lib/win64)

# define source files (!!!automatic but must rebuild CMakeLists.txt if new files are added!!!)
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB LIB_FILES "include/*.h")

# create .lib
add_library( MEL ${SRC_FILES} ${LIB_FILES})

# link libraries to .lib
target_link_libraries( MEL ${Boost_LIBRARIES} hil quanser_communications quanser_runtime quanser_common bufferoverflowU "legacy_stdio_definitions.lib")

# make include files publicly available to other CMakeLists (see OpenWrst)
target_include_directories (MEL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

cmake_minimum_required(VERSION 3.7)
project(OpenWrist)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(Boost_INCLUDE_DIR C:/dev/boost_1_64_0)
set(Boost_LIBRARY_DIR C:/dev/boost_1_64_0/lib64-msvc-14.1)
set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(C:/dev/quarc/include)
link_directories(C:/dev/quarc/lib/win64)

# file(GLOB SRC_FILES "src/*.cpp")
set(SRC_FILES src/main_testing.cpp)

add_executable( OpenWrist ${SRC_FILES} )
target_link_libraries( OpenWrist LINK_PUBLIC MEL ${Boost_LIBRARIES} "legacy_stdio_definitions.lib")

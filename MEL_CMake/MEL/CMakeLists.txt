cmake_minimum_required(VERSION 3.7)
project(MEL)

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

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB LIB_FILES "include/*.h")

add_library( MEL ${SRC_FILES} ${LIB_FILES} )
target_link_libraries( MEL ${Boost_LIBRARIES} hil quanser_communications quanser_runtime quanser_common bufferoverflowU "legacy_stdio_definitions.lib")
target_include_directories (MEL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

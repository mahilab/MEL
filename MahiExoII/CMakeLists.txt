cmake_minimum_required(VERSION 3.7)
project(MahiExoII)

# set standard to C++11
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# prevent errors about unsafe functions
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

# include eigen
include_directories(C:/dev/eigen)
#set(Eigen3_DIR C:/dev/eigen/eigen)
#find_package (Eigen3 3.3 REQUIRED NO_MODULE)


# define source files
set(SRC_FILES src/main_testing.cpp)

# create .exe
add_executable( MahiExoII ${SRC_FILES} )

# link libraries to .exe
target_link_libraries( MahiExoII LINK_PUBLIC MEL ${Boost_LIBRARIES} "legacy_stdio_definitions.lib")
#target_link_libraries (example Eigen3::Eigen)

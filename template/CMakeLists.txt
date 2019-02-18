cmake_minimum_required(VERSION 3.7)

# enable C++11
enable_language(C)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

# create project
project(MyProject)

# set binary output location to conventional "bin" (
# optional, but recommended for one-off projects)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# include directories
include_directories(
    "include"              # your include directory
    "../../../MEL/include" # MEL's include directory
)

# indicate where MEL.lib is
link_directories("../../../MEL/lib")

# remove or uncomment if linking MEL statically
# you must also manually link your app to MEL's dependencies
# add_definitions(-DMEL_STATIC)

# create your application
add_executable(my_app
               include/MyClass.hpp
               src/MyClass.cpp
               src/main.cpp)

# link MEL to your application
target_link_libraries(my_app MEL)

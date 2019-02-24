cmake_minimum_required(VERSION 3.13)

# create your project
project(MyProject VERSION 0.1.0 LANGUAGES CXX)

# find MEL
find_package(MEL REQUIRED)

# add your include directories
include_directories("include")

# create an app from your source files
add_executable(my_app
    "include/MyClass.hpp"
    "src/MyClass.cpp"
    "src/my_app.cpp"
)

# link MEL core lib and/or MEL sublibs to your app
target_link_libraries(my_app 
    MEL::MEL       # common MEL classes
    # MEL::quanser # needed if using MEL Quanser classes 
    # MEL::myo     # needed if using MEL Myo classes
)

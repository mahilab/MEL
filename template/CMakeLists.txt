cmake_minimum_required(VERSION 3.13)

# create your project
project(MyProject VERSION 0.1.0 LANGUAGES CXX)

# find MEL
find_package(MEL REQUIRED CONFIG)

# add your include directories
include_directories("include")

# create a executable from your source files
add_executable(my_app
    "include/MyClass.hpp"
    "src/MyClass.cpp"
    "src/my_app.cpp"
)

# link MEL to your application
target_link_libraries(my_app 
    MEL::MEL       # common MEL classes
    # MEL::quanser # needed if using MEL Quanser classes 
    # MEL::myo     # needed if using MEL Myo classes
)

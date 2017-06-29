cmake_minimum_required(VERSION 3.7)
project(MAHIEXOSKELETONLIB)

# include all subdirectories containing individual CMakeLists.txt(s)
add_subdirectory (MEL)
add_subdirectory (MahiExoII)
add_subdirectory (OpenWrist)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
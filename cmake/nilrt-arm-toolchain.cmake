# CMake toolchain file for NI Linux Real-Time (ARM)
# Evan Pezent (epezent@rice.edu)

SET(CMAKE_SYSTEM_NAME Linux)

# set flag for MEL CMakeLists.txt
set(MEL_NI_ARM ON)

# root to cross compiler
set(NI_ARM_ROOT "/dev/nilrt-arm" CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (ARM).")

# add compiler program path
LIST(APPEND CMAKE_PROGRAM_PATH ${NI_ARM_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/arm-nilrt-linux-gnueabi)

# set compiler
set(CMAKE_C_COMPILER   arm-nilrt-linux-gnueabi-gcc CACHE FILEPATH "NI LRT ARM C Compiler")
set(CMAKE_CXX_COMPILER arm-nilrt-linux-gnueabi-g++ CACHE FILEPATH "NI LRT ARM C++ Compiler")

# set to cross compile
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)

# setup sysroot (for NI RT cross compiler)
set(CMAKE_SYSROOT ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi CACHE FILEPATH "Path to NI ARM Cross Compiler Sysroot")
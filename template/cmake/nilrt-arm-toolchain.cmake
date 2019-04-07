# CMake toolchain file for NI Linux Real-Time (ARM)
# Evan Pezent (epezent@rice.edu)

SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# set flags for MEL CMakeLists.txt
set(NI_LRT ON)
set(NI_ARM ON)

# root to cross compiler
set(NI_ARM_ROOT "/dev/nilrt-arm" CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (ARM).")

# add compiler program path
LIST(APPEND CMAKE_PROGRAM_PATH ${NI_ARM_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/arm-nilrt-linux-gnueabi)

# set compiler
set(CMAKE_C_COMPILER   arm-nilrt-linux-gnueabi-gcc CACHE FILEPATH "NI LRT ARM C Compiler")
set(CMAKE_CXX_COMPILER arm-nilrt-linux-gnueabi-g++ CACHE FILEPATH "NI LRT ARM C++ Compiler")

# set to cross compile
set(CMAKE_CROSSCOMPILING 1)

# setup sysroot (for NI RT cross compiler)
set(CMAKE_SYSROOT ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi CACHE FILEPATH "Path to NI ARM Cross Compiler Sysroot")
set(CMAKE_FIND_ROOT_PATH 
    "C:/Program Files (x86)/MEL-nilrt-arm/lib/cmake/MEL"
    "C:/Program Files/MEL-nilrt-arm/lib/cmake/MEL"
)

set(CMAKE_C_FLAGS_INIT   "-pthread -march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp -mcpu=cortex-a9")
set(CMAKE_CXX_FLAGS_INIT "-pthread -march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp -mcpu=cortex-a9")

# includes for VS Code to work
include_directories(
    ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/6.3.0
    ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include
    ${NI_ARM_ROOT}/sysroots/cortexa9-vfpv3-nilrt-linux-gnueabi/usr/include/c++/6.3.0/arm-nilrt-linux-gnueabi
)
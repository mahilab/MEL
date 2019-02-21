# CMake toolchain file for NI Linux Real-Time (x64)
# Evan Pezent (epezent@rice.edu)

set(CMAKE_SYSTEM_NAME Linux)

# set flag for MEL CMakeLists.txt
set(MEL_NI_X64 ON)

# root to cross compiler
set(NI_X64_ROOT  "/dev/nilrt-x64"  CACHE FILEPATH "Absolute path to NI Linux Realtime GNU toolchain (x64).")

# add compiler program path
LIST(APPEND CMAKE_PROGRAM_PATH ${NI_X64_ROOT}/sysroots/i686-nilrtsdk-mingw32/usr/bin/x86_64-nilrt-linux)

# set compiler
set(CMAKE_C_COMPILER   x86_64-nilrt-linux-gcc CACHE FILEPATH "NI LRT x64 C Compiler")
set(CMAKE_CXX_COMPILER x86_64-nilrt-linux-g++ CACHE FILEPATH "NI LRT x64 C++ Compiler")

# set to cross compile
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)

# setup sysroot (for NI RT cross compiler)
set(CMAKE_SYSROOT ${NI_X64_ROOT}/sysroots/core2-64-nilrt-linux CACHE FILEPATH "Path to NI x64 Cross Compiler Sysroot")

set(CMAKE_C_FLAGS_INIT   "-pthread")
set(CMAKE_CXX_FLAGS_INIT "-pthread")

SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

![MEL Logo](https://raw.githubusercontent.com/epezent/MEL/master/logo.png)

# MEL - MAHI Exoskeleton Library

## Developers

- **[Evan Pezent](http://evanpezent.com)** (epezent@rice.edu)
- **[Craig McDonald](http://craiggmcdonald.com/)** (craig.g.mcdonald@gmail.com)

## Requirements

The basic MEL library has no external dependencies but does rely on a few common utilities. Before getting started, download these tools, making sure they are added to the [PATH system variable](https://www.java.com/en/download/help/path.xml):

- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/)

Depending on how you use MEL, you may need to install a few additional hardware specific libraries:

#### National Instruments Linux Real-Time (Optional)

If you plan to use MEL for developing on NI Linux Real-Time hardware (e.g. cRIO), you'll need to install [NI's GNU/Linux cross-compiler](http://www.ni.com/download/labview-real-time-module-2017/6762/en/). MEL expects the top level of the compiler directories to be in `C:/dev/nirlt-linux`, which should contain `sysroots/`, `environment-setup-core2-64-nilrt-linux`, `relocate_sdk.py`, etc. If you want to change the installation directory, make sure you update CMakeLists.txt.

You will also want a suitable build system. The recommended tool is the [Ninja](https://ninja-build.org/), but you can also `make`, (Linux host) `mingw32-make`, or `nmake` (Windows host).

#### Quanser QUARC (Optional)

If you plan to use MEL for developing on Quanser hardware (e.g. Q8 USB), you'll need to install QUARC (v2.5 or greater). MEL expects to find the necessary QUARC headers and libs in the following directories:
```shell
C:/Program Files/Quanser/QUARC/include
C:/Program Files/Quanser/QUARC/lib/windows
C:/Program Files/Quanser/QUARC/lib/win64
```
These directories and files are created automatically when you install QUARC. If you installed QUARC in another location, make sure you update CMakeLists.txt. Note that **QUARC can only be compiled with MSVC** so make sure you install [Visual Studio](https://www.visualstudio.com/).

#### Eigen (Optional)

If you are developing for the MAHI Exo-II, you will need to install [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) for forward and inverse kinematics. MEL expects to find Eigen in `C:/dev/eigen`, which should contain `bench/`, `blas/`, `cmake/`, etc. If you want to change the installation directory, make sure you update CMakeLists.txt.

#### Python / C# (Optional)

MEL comes with Python and C# bindings for it's high-level communication classes, MELShare and MELNet. It also comes with an awesome real-time scoping application MELScope, which requires Python and PyQt. For Python installation requirements, go [here](https://github.com/epezent/MEL/tree/master/python). For C# (e.g. if using [Unity Engine](https://unity3d.com/) for visualizations), you should be able to use [Visual Studio](https://www.visualstudio.com/) or [MonoDevelop](http://www.monodevelop.com/) as is.

## Building MEL

Navigate to the directory you wish to install MEL and run the following commands:

```shell
git clone https://github.com/epezent/MEL    # clone MEL into ./MEL directory
cd MEL                                      # change directory to ./MEL
mkdir build                                 # make a build directory for CMake
cd build                                    # change directory to ./build
```

At this point, you can customize MEL to suit your platform and hardware needs. MEL provides the following options when building with CMake:

* `-DNI=ON` adds MEL implementations for NI hardware (cRIO, myRIO, etc.), and sets the compiler to NI's GNU/Linux cross-compiler
* `-DQUANSER=ON` adds MEL implementations for Quanser hardware (Q8 USB, Q2 USB, etc.) and statically links MEL to QUARC
* `-DOPENWRIST=ON` adds OpenWrist classes to MEL
* `-DMAHIEXOII=ON` adds MAHI Exo-II classes to MEL and includes Eigen as a dependency
* `-DEXAMPLES=ON` builds all MEL example executables which are compatible with the platform, compiler, and any of the options above which are specified

Here are a few examples:

**Example 1: Basic MEL Library**
```shell
cmake ..
```
This will generate a build file for the bare bones MEL library (i.e. no hardware implementations) using whatever the CMake thinks the default generator/compiler is. If you're on Linux using GCC, this will probably be a `makefile` in which case you simple run `make` to compile MEL. If you're on Windows using MSVC, this will be a `.sln` file which you can open, edit, compile, and debug using Visual Studio.

**Example 2: MEL + NI Hardware + OpenWrist + Ninja**
```shell
cmake .. -G "Ninja" -DNI=ON -DOPENWRIST=ON
ninja
```
The first line generates Ninja build files for MEL with NI hardware and OpenWrist classes. The second line then calls Ninja which builds MEL using NI's cross-compiler.

**Example 3: MEL + Quanser Hardware + MAHI Exo-II + Visual Studio**
```shell
cmake .. -G "Visual Studio 15 2017 Win64" -DQUANSER=ON -DMAHIEXOII=ON
```
This generates a `.sln` file for MEL with Quanser hardware and MAHI Exo-II classes. The `.sln` can then be opened and compiled with Visual Studio, or built from an MSVC Developer Command Prompt using the command `msbuild MEL.sln`.

## Creating Projects for MEL

After the build/compile process has completed, the library binary will be output to `MEL/lib/[platform]/` while example executables will be output to `MEL/bin/<platform>/` where `<platform>` may be either `linux` or `windows`. To use MEL, you simply need to include the `MEL/include` directory and link to the MEL library binary.

Since you already have CMake installed, here's a typical CMake folder structure and `CMakeLists.txt` to get you started:
```
my_project
├── bin
├── build
├── include
|   ├── MyClass.hpp
├── src
|   ├── MyClass.cpp
|   ├── ...
|   ├── main.cpp
├── CMakeLists.txt
```

The `CMakeLists.txt` would then look like this:

```cmake
cmake_minimum_required(VERSION 3.7)
# enable C++11
set(CMAKE_CXX_STANDARD 11)
# create project
project(MyProject)
# set binary output location (optional, but recommended)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
# include directories (your includes and MEL's)
include_directories("include" "/path/to/.../MEL/include")
# indicate where MEL.lib is
link_directories("/path/to/.../MEL/lib/<platform>")
# create application
add_executable(MyApp include/MyClass.hpp src/MyClass.cpp src/main.cpp)
# link MEL
target_link_libraries(MyApp MEL)
```
Finally, to build your project open a command prompt at the top level of your project directory and run:
```shell
cd build   # change directory to ./build
cmake ..   # run CMake (optionally pass -G "Generator String" to specify a non-default generator)
```
That's it! Now you can proceed to compile your project with the appropriate software. Remember, you should use the same compiler that compiled MEL.



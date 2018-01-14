
![MEL Logo](https://raw.githubusercontent.com/epezent/MEL/master/logo.png)

# MEL - MAHI Exoskeleton Library

## Developers

- [Evan Pezent](http://evanpezent.com) (epezent@rice.edu)
- [Craig McDonald](http://craiggmcdonald.com/) (craig.g.mcdonald@gmail.com)

## Requirements

The basic MEL library has no external dependencies but does rely on a few common utilities. Before getting started, download these tools:

- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/)

Depending on how you use MEL, you may need to install a few additional hardware specific libraries:

#### National Instruments (Optional)

If you plan to use MEL for developing on NI Linux Real-Time hardware (e.g. cRIO), you'll need to install [NI's GNU/Linux cross-compiler](http://www.ni.com/download/labview-real-time-module-2017/6762/en/). MEL expects the top level of the compiler directories to be in ```C:/dev/nirlt-linux-g++```, which should contain ```sysroots/```, ```environment-setup-core2-64-nilrt-linux```, ```relocate_sdk.py```, etc. If you want to change the installation directory, make sure you update CMakeLists.txt.

#### Quanser QUARC (Optional)

If you plan to use MEL for developing on Quanser hardware (e.g. Q8 USB), you'll need to install QUARC (v2.5 or greater). MEL expects to find the necessary QUARC headers and libs in the following directories:
```
C:/Program Files/Quanser/QUARC/include
C:/Program Files/Quanser/QUARC/lib/windows
C:/Program Files/Quanser/QUARC/lib/win64
```
These directories and files are created automatically when you install QUARC. If you installed QUARC in another location, make sure you update CMakeLists.txt. Note that QUARC can only be compiled with [Visual Studio](https://www.visualstudio.com/), so make sure you have that installed.

#### Eigen (Optional)

If you are developing for the MAHI Exo-II, you will need to install [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) for forward and inverse kinematics. MEL expects to find Eigen in ```C:/dev/eigen```, which should contain ```bench/```, ```blas/```, ```cmake/```, etc. If you want to change the installation directory, make sure you update CMakeLists.txt.

## Building MEL

Navigate to the directory you wish to install MEL and run the following commands:

```
git clone https://github.com/epezent/MEL
cd MEL
mkdir build
cd build
```

At this point, you can choose from one of the following build methods using CMake. If you want to build **OpenWrist** and/or **MAHI Exo-II** classes, append ```-DOPENWRIST=ON``` and ```-DMAHIEXOII=ON```, respectively. If you want to build the **example programs**, append ```-DEXAMPLES=ON```.


**Option 1: Basic MEL Library**
```
cmake ..
```
Run ```make``` or ```mingw32-make``` if using a GNU compiler or open the generated MEL.sln file in Visual Studio if using the MSVC compiler to proceed with build process.

**Option 2: MEL + NI Hardware + Linux**
```
cmake .. -G "MinGW Makefiles" -DNI=ON
make
```

**Option 3: MEL + Quanser Hardware + Windows**
```
cmake .. -G "Visual Studio 15 2017 Win64" -DQUANSER=ON
```
Open the generated MEL.sln file in Visual Studio to proceed with build process

## Linking and Using MEL

After the build/compile process has completed, binaries will be output to ```MEL/lib``` as **MEL.a** (GNU) or **MEL.lib** (MSVC) while example executables will be output to ```MEL/bin```.



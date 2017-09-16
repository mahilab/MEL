
![MEL Logo](https://raw.githubusercontent.com/epezent/MEL/master/logo.png)

# MEL - MAHI Exoskeleton Library

## Developers

- [Evan Pezent](http://evanpezent.com) (epezent@rice.edu)
- [Craig McDonald](http://craiggmcdonald.com/) (craig.g.mcdonald@gmail.com)

## Table of Contents

1. [Git / GitHub Setup](#git--github-installation)
2. [Installation (Windows 64-bit)](#installation-windows-64-bit)
    - [Visual Studio 2017 - Visual C++](#visual-studio-2017---visual-c)
    - [Required C++ Libraries](#required-c-libraries-boost-eigen-quarc)
3. [Building MEL (Windows 64-bit)](#building-mel-windows-64-bit)
4. [Using MEL (Windows 64-bit)](#using-mel-in-your-projects-windows-64-bit)

##   Git / GitHub Setup

**Option 1 (Recommended):**

1. Download and install [git](https://git-scm.com/downloads) for Windows.
2. Open a command prompt in the directory you would like to clone MEL to and enter the following command:

```
git clone https://github.com/epezent/MEL
```

**Option 2:**

1. Download and install [GitHub Desktop](https://desktop.github.com/). Login using your GitHub account once installation is complete.

2. Choose **Clone a repository**

3. Enter the MEL URL: **https://github.com/epezent/MEL**

4. Set the local path to **C:\whatever\you\like\MEL**

3. Clone the repository. Navigate to the local path in Windows Explorer and verify that the repository has been cloned.

## Installation (Windows 64-bit)

**IMPORTANT:** It is important that you install all items **exactly** where specified. If you choose to install items in locations other than **C:\dev**, you will break dependency location references in the MEL Visual Studio Solution and will have to update them yourself.

###  Visual Studio 2017 - Visual C++

1. Download [Visual Studio 2017 Community](https://www.visualstudio.com/)

2. Run the VS2017 installer and check the following options:

    Under the **Workloads** tab:

    - Desktop development with C++

    Under the **Individual components** tab:

    - Git for Windows
    - GitHub extension for Visual Studio
    - Python language support (if you plan to use Python for GUIs, scopes, or machine learning)
    - Visual Studio Tools for Unity (if you plan to use Unity Engine for games and visualizations)

3. Start and complete the VS2017 installation

###   Required C++ Libraries (Boost, Eigen, Quarc)

1. Download and run [Boost 1.64.0 - MSVC 14.1 (64-bit)](https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/boost_1_64_0-msvc-14.1-64.exe/download)

    - Set the installation directory to **C:\dev\boost\_1\_64\_0**
    - Alternatively, you can choose to build the [Boost](http://www.boost.org/) yourself

2. Download the latest stable release of [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)

    - Extract the folder labeled **eigen-eigen-5a10...** to **C:\dev** and change the folder name to just **eigen**
    - If you are in **C:\dev\eigen** you should see folders *bench*, *blas*, *cmake*, *debug*, etc.

3. Navigate to **C:\Program Files\Quanser\QUARC** (you must have Quarc 2.5 or greater installed)

    - Copy the *include* and *lib* folders to a new directory *C:\dev\quarc*
    - From *C:\dev\quarc\lib* you can delete all folders EXCEPT *windows* and *win64*
    - If you don't have Quarc installed, you can get the folders from another MAHI lab member who does

## Building MEL (Windows 64-bit)

If you are on Windows and using Visual Studio, building MEL is fairly straight-forward using the provided Solution file.

1. Navigate to your cloned MEL repository, and open the MEL solution (*MEL.sln*)

2. The MEL solution has 3 projects:

    1. **MEL** - This is the MEL library, itself. It outputs **MEL.lib** to *\bin*.

    2. **MELShareDLL** - This is an optionally compiled dynamically linked library that exposes an interface to MEL's shared memory system, **MELShare**, for developing apps in Unity, Python, etc. It outputs **MELShare.dll** to *\bin*.

    3. **Examples** - This is an optionally compiled executable that demonstrates basic MEL use. It outputs **Examples.exe** to *\bin*.

3. Make sure the Visual Studio build configuration is set to **Release x64**. You can build all three projects at once by selecting **Build >> Build Solution** from the menu. If you only want to build one project, right-click the project in the Solution Explorer and select **Build**. The resulting binaries (.lib, .dll, or. exe) will output to *\bin*.

## Using MEL In Your Projects (Windows 64-bit)

1. If you are starting a new project, first create a new Visual Studio solution and project. Go to **File >> New >> Project**. Choose **Win32 Console Application**  and give your Project and Solution a name. In the following wizard, uncheck **Precompiled Headers** (unless you know what they are and want to use them).

2. There are a few ways to link your project against MEL. If you plan to make changes to the MEL source code yourself, follow **(A)** to add the MEL project to your VS solution. This way any changes you make to MEL will be reflected in your project, and MEL will rebuild when you attempt to build your project. If you just want to use MEL as is, you can simply include MEL as you would any other library **(B)**.

    **METHOD A: Add the MEL Project to Your Solution**

    - With your solution open, go to **File >> Add >> Existing Project...** and navigate to and select **path-to-your-MEL-repository/MEL/MEL.vcxproj**

    - Right-click *your* project in the Solution Explorer and select **Add >> Reference...** and check MEL

    - Right-click *your* project in the Solution Explorer and select **Properties**. Make the following changes/additions:

        - **C/C++ >> General >> Additional Include Directories**
            - add: *path-to-your-MEL-repository\MEL;C:\dev\boost_1_64_0\;C:\dev\eigen;
        - **Linker >> General >> Additional Library Directories**
            - add: *path-to-your-MEL-repository\bin;C:\dev\boost_1_64_0\lib64-msvc-14.1;*

    **METHOD B: Link MEL.lib to Your Project**

    - Right-click *your* project in the Solution Explorer and select **Properties**. Make the following changes/additions:

        - **C/C++ >> General >> Additional Include Directories**
            - add: *path-to-your-MEL-repository\MEL;C:\dev\boost_1_64_0\;C:\dev\eigen;*
        - **Linker >> General >> Additional Library Directories**
            - add: *path-to-your-MEL-repository\bin;C:\dev\boost_1_64_0\lib64-msvc-14.1;*
        - **Linker >> Input >> Additional Dependencies**
            - add: *MEL.lib;*

3. Your project should now be configured to use MEL! For usage examples, check out the [Examples project](https://github.com/epezent/MEL/tree/master/Examples), the official MEL documentation, or the [OpenWrist](https://github.com/epezent/OpenWrist) and [MAHI Exo-II](https://github.com/craigmc707/MEII) repositories.



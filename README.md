

# MEL - MAHI Exoskeleton Library

## Developers

- Evan Pezent (epezent@rice.edu)
- Craig McDonald (craig.g.mcdonald@gmail.com)

## Installation (Windows)

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

###   C++ Libraries (Boost, Eigen, Quarc)

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

###   Git / GitHub Installation

1. Download and install [GitHub Desktop](https://desktop.github.com/). Login using your GitHub account once installation is complete.

2. Choose **Clone a repository**

3. Enter the MEL URL: **https://github.com/epezent/MEL**

4. Set the local path to **C:\whatever\you\like\MEL**

3. Clone the repository. Navigate to the local path in Windows Explorer and verify that the repository has been cloned.

###   Python 3.6 + NumPy + PyQt5 + PyQtGraph

1. Download [Python 3.6.1 (64-bit)](https://www.python.org/downloads/release/python-361/)

2. Choose Customize installation. Click Next. Check **Install for all users** and **Add Python to environment variables**. Set the installation location to **C:\dev\Python36**.

3. Go to the [Unofficial Windows Binaries for Python Extension Packages](http://www.lfd.uci.edu/~gohlke/pythonlibs/) page. Download the latest binaries for **NumPy**, **SciPy**, and **MatPlotLib**. Make sure you select the correct build (i.e. listings ending with -cp36-cp36m-win\_amd64.whl). At the time of writing, the latest versions were:

    - numpy-1.13.1+mkl-cp36-cp36m-win_amd64.whl
    - scipy-0.19.1-cp36-cp36m-win_amd64
    - matplotlib-2.0.2-cp36-cp36m-win_amd64

4. Add the .whl files to a new directory **C:\dev\Python36\wheels**

3. While in this directory in Windows Explorer, Shift+Right-Click in an empty space and select "Open command (or PowerShell) window here". Execute the following console commands **IN THIS ORDER**:

   ```
   pip3 install --upgrade pip
   pip3 install numpy-1.13.1+mkl-cp36-cp36m-win_amd64.whl
   pip3 install scipy-0.19.1-cp36-cp36m-win_amd64.whl
   pip3 install matplotlib-2.0.2-cp36-cp36m-win_amd64.whl
   pip3 install PyOpenGL
   pip3 install PyOpenGL_accelerate
   pip3 install PyQt5
   pip3 install iPython
   ```

   **NOTE:** PyOpenGL_accelerate may fail to install with the following error:

   *error: Microsoft Visual C++ 14.0 is required. Get it with "Microsoft Visual C++ Build Tools": http://landinghub.visualstudio.com/visual-cpp-build-tools*

   If this happens, follow the URL and download and install the standalone compiler (Visual C++ 2015 Build Tools). Go to the installation location **C:\Program Files (x86)\Microsoft Visual C++ Build Tools** and run **Visual C++ 2015 x86 x64 Cross Build Tools Command Prompt**. Execute **pip3 install PyOpenGL_accelerate** from here.

5. Download the [PyQtGraph (64-bit) Installer](http://www.pyqtgraph.org/). Run the installer and insure that the Python installation directory is correctly identified as C:\dev\Python36

6. Run the following Python script to test PyQt5 + PyQtGraph. Play around with each test example.

    ```python
    import pyqtgraph.examples
    pyqtgraph.examples.run()
    ```








# MEL - MAHI Exoskeleton Library




##  Visual Studio 2017 C++ Installation


1. Download Visual Studio 2017 Community:

   https://www.visualstudio.com/

2. Run the VS2017 installer and check the following options:

   Under the Workloads Tab:

   -Desktop development with C++

   Under the Individual components Tab:

   -Git for Windows
   -GitHub extension for Visual Studio
   -VC++ 2015.3 v140 toolset (x86,x64)
   -Python language support (if you plan to use Python for GUIs/scopes)
   -Visual Studio Tools for Unity (if you plan to use Unity for games)

3. Start and complete the VS2017 installation


##   C++ Library Installations (Boost, Quarc, & Eigen)


1. Download and run boost\_1\_64\_0-msvc-14.1-64.exe from:

   https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/boost_1_64_0-msvc-14.1-64.exe/download

   Set the installation directory to C:\dev\boost_1_64_0

   Alternatively, you can choose to build the Boost library yourself:

   http://www.boost.org/

2. Download the latest stable release (.zip) of Eigen (3.3.4 at the time of writing)

   http://eigen.tuxfamily.org/index.php?title=Main_Page

   From the zip, extract "eigen-eigen-5a10.." to C:\dev\
   Change the folder name to just "eigen"
   If you are in C:\dev\eigen you should see folders "bench", "blas", "cmake", "debug", ... etc.
   If this is not the folder structure you have, change it to be such

3. Navigate to C:\Program Files\Quanser\QUARC (you must have Quarc 2.5 or greater installed)

   COPY the "include" and "lib" folders to a new directory to C:\dev\quarc\
   From C:\dev\quarc\lib you can delete all foldrs EXCEPT "windows" and "win64"

   If you don't have Quarc installed, you can get these folders from another MAHI lab member
   who has Quarc installed.


##   Git / GitHub Installation


1. Download and install GitHub Desktop. Login using your GitHub account once
   installation is complete.

2. Choose "Clone a repository" and enter the following URL:

   https://github.com/epezent/MEL

   Set the local path to C:\whatever\you\like\MEL

3. Clone the repository. Navigate to the local path in Windows Explorer and
   verify that the repository has been cloned.

##   Python 3.6 + NumPy + PyQt5 + PyQtGraph (Reccommended)


1. Run python-3.6.1-amd64.exe. Choose Custom installation, and set the installation
   directory to C:\dev\Python36. Ensure that the "Add python.exe to path" option is
   enabled.

2. Create a new directory C:\dev\Python36\wheels and extract the following .whl
   to this directory:

   numpy-1.13.1+mkl-cp36-cp36m-win_amd64.whl
   scipy-0.19.1-cp36-cp36m-win_amd64
   matplotlib-2.0.2-cp36-cp36m-win_amd64

3. While in this directory in Windows Explorer, Shift+Right-Click in an empty
   space and select "Open command (or PowerShell) window here".

4. Execute the following console commands in this order:

   pip3 install --upgrade pip
   pip3 install numpy-1.13.1+mkl-cp36-cp36m-win_amd64.whl
   pip3 install scipy-0.19.1-cp36-cp36m-win_amd64
   pip3 install matplotlib-2.0.2-cp36-cp36m-win_amd64
   pip3 install PyOpenGL PyOpenGL_accelerate

   NOTE: If PyOpenGL fails to install, run visualcppbuildtools_full.exe
   and then try again. If it fails again, copy rc.exe and rcdll.dll from
   C:\Program Files (x86)\Windows Kits\8.1\bin\x64 (or this .zip) to
   C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\x86_amd64
   and try to install again. This should resolve the issue, if not...sorry!

   pip3 install PyQt5
   pip install ipython

5. Run pyqtgraph-0.10.0.win-amd64.exe. Ensure that the Python installation
   directory is correctly identified as C:\dev\Python36

6. Run test.py in the provided Python folder to ensure that the installation
   was successful. Try running all of the examples in the Benchmarks tab.

##   Python 2.7 + NumPy + PyQt4 + PyQtGraph (Legacy)

0. You can choose to install Python 2.7 over 3.6 if you know you will require Python
   libraries that do not yet support Python3.

1. Run python-2.7.13.amd64.msi. Set the installation directory to
   C:\dev\Python27. Ensure that the "Add python.exe to path" option is set to
   "Will be installed on local hard drive"

2. Create a new directory C:\dev\Python27\wheels and extract the following .whl
   to this directory:

   numpy-1.11.1+mkl-cp27-cp27m-win_amd64.whl
   scipy-0.18.0-cp27-cp27m-win_amd64.whl
   matplotlib-1.5.2-cp27-cp27m-win_amd64.whl

3. While in this directory in Windows Explorer, Shift+Right-Click in an empty
   space and select "Open command (or PowerShell) window here".

4. Execute the following console commands in this order:

   pip install --upgrade pip
   pip install numpy-1.11.1+mkl-cp27-cp27m-win_amd64.whl
   pip install scipy-0.18.0-cp27-cp27m-win_amd64.whl
   pip install matplotlib-1.5.2-cp27-cp27m-win_amd64.whl
   pip install PyOpenGL PyOpenGL_accelerate
   pip install ipython

   NOTE: If PyOpenGL(_acclerate) fails to install, run VCForPython27.msi and
   then try again.

5. Run PyQt4-4.11.4-gpl-Py2.7-Qt4.8.7-x64.exe. Ensure that the Python
   installation directory is correctly identified.

6. Run pyqtgraph-0.10.0.win-amd64.exe. Ensure that the Python installation
   directory is correctly identified as C:\dev\Python27

7. Run test.py in the provided Python folder to ensure that the installation
   was successful. Try running all of the examples in the Benchmarks tab.

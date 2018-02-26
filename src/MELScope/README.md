# MELScope

![MELScope](https://github.com/epezent/MEL/blob/master/src/MELScope/melscope_screen.png)

## Developers

- [Evan Pezent](http://evanpezent.com) (epezent@rice.edu)

## About

MELScope is a real-time signal plotting GUI for MEL. It communicates with MEL driven applications through the **MELShare** shared memory framework. Using MELScope is simple: First, you create MELShare map(s) storing a vector of doubles in your C++ MEL application to which you write updated values at some specific rate (typically this happens in your control loop). Next, in MELScope you add these MELShare maps by their string name. MELScope will then begin to plot whatever values are stored on the MELShare maps.

You can add as many MELShare maps to MELScope as you like. You also have the option of expanding the number of scope widgets, and adding I/O widgets which allow you to receive and send values back to your C++ application (for example, you may use this as an online **controller tunning** mechanism). Using a data matrix selector, you can even name and customize which data is displayed on each widget. You can also change **colors, line styles, ranges, legends and more**. Finally, once you have your MELScope configured the way you like it, you can **save your configuration** for opening later.

## Installation (Standalone)

The easiest way to use MELScope is to simply download and extract the latest  prebuilt binaries from the provided `.zip` archive on the Releases page. Just run `MELScope.exe` and you're good to go!

## Installation (Python 2.7)

If you'd rather have the flexibility of running MELScope from the Python interpreter, follow these directions:

1. Download [Python 2.7.13 (64-bit) Installer](https://www.python.org/downloads/release/python-2713/)

2. Run the installer and set the installation location to **C:\dev\Python27**. Ensure that the "Add python.exe to path" option is set to "Will be installed on local hard drive"

3. Download and install the [Microsoft Visual C++ Compiler for Python 2.7](https://www.microsoft.com/en-us/download/details.aspx?id=44266)

4. Go to the [Unofficial Windows Binaries for Python Extension Packages](http://www.lfd.uci.edu/~gohlke/pythonlibs/) page. Download the latest binaries for **NumPy**, **SciPy**, and **MatPlotLib**. Make sure you select the correct build (i.e. listings ending with **-cp27-cp27m-win\_amd64.whl**). At the time of writing, the latest versions were:

    - numpy-1.13.1+mkl-cp27-cp27m-win_amd64.whl
    - scipy-0.19.1-cp27-cp27m-win_amd64
    - matplotlib-2.0.2-cp27-cp27m-win_amd64

5. Add the .whl files to a new directory **C:\dev\Python27\wheels**

6. While in this directory in Windows Explorer, Shift+Right-Click in an empty space and select "Open command (or PowerShell) window here". Execute the following console commands **IN THIS ORDER**:

   ```
   pip install --upgrade pip
   pip install numpy-1.13.1+mkl-cp27-cp27m-win_amd64.whl
   pip install scipy-0.19.1-cp27-cp27m-win_amd64.whl
   pip install matplotlib-2.0.2-cp27-cp27m-win_amd64.whl
   pip install PyOpenGL
   pip install PyOpenGL_accelerate
   pip install PyYAML
   pip install qdarkstyle
   pip install pyflakes
   pip install pyinstaller
   pip install iPython
   ```

   **NOTE:** PyOpenGL_accelerate may fail to install if you skipped step 2. This is because it must be built from source and requires the legacy MSVC compiler be installed.

7. Download and the [PyQt4 binaries for Python 2.7 (64-bit)](https://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.11.4/PyQt4-4.11.4-gpl-Py2.7-Qt4.8.7-x64.exe/download?use_mirror=managedway&r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fpyqt%2Ffiles%2FPyQt4%2FPyQt-4.11.4%2F&use_mirror=managedway). Run the installer and ensure that the Python installation directory is correctly identified as **C:\dev\Python27**

8. Download the [PyQtGraph (x86-64) Installer](http://www.pyqtgraph.org/). Run the installer and ensure that the Python installation directory is correctly identified as **C:\dev\Python27**

9. Create a new Python script called **test.py** and paste the following code. Run the script to test PyQt4 + PyQtGraph. Play around with each test example, particularly the benchmarks, to ensure everything is working properly.

  ```python
  import pyqtgraph.examples
  pyqtgraph.examples.run()
  ```

  10. If you want to build MELScope as a executable yourself, use the following command in the command prompt (assuming you installed **pyinstaller**):

  ```
  pyinstaller --noconsole --icon=melscope_icon.ico --clean MELScope.pyw
  ```

**NOTE:** The newer Python 3.6 and PyQt5 have been tested and were found to be less stable and slower than the more mature Python 2.7 + PyQt4 pairing when used with PyQtGraph. You may choose to install Python 3.6 and PyQt5 (or any combination of the two, really) but note that the installation procedure will differ slightly (specifically when downloading wheels/binaries and building PyOpenGl_accelerate).



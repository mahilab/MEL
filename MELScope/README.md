# MELScope

![MELScpe](https://github.com/epezent/MEL/blob/master/MELScope/screenshot.png)

## Developers

- [Evan Pezent](http://evanpezent.com) (epezent@rice.edu)

## Table of Contents

1. [About](#about)
2. [Installation (Windows 64-bit)](#installation-windows-64-bit)
    - [Python 2.7 (Optional)](#python-27--numpy--pyqt4--pyqtgraph)
    - [Sublime Text 3 (Optional)](#sublime-text-3-optional)
3. [Building MELScope](#building-melscope)

## About

MELScope is a real-time signal plotting GUI for MEL. It communicates with MEL driven applications through the **MELShare** shared memory communication framework. Using MELScope is simple: First, you create qvMELShare map(s) storing **doubles** of vectors/arrays of doubles in your C++ MEL application to which you write updated values at some specific rate (typically this happens in your control loop). Next, in MELScope you add these MELShare maps by their string name. MELScope will then begin to plot whatever values are stored on the MELShare maps.

You can add as many MELShare maps to MELScope as you like. You also have the option of expanding the number of scope widgets, and adding I/O widgets which allow you to receive and send values back to your C++ application (for example, you may use this as an online **controller tunning** mechanism). Using a data matrix selector, you can even name and customize which data is displayed on each widget. You can also change **colors, line styles, ranges, legends and more**. Finally, once you have your MELScope configured the way you like it, you can **save your configuration** for opening later.

A example demonstrating MELShare/MELScope usage can be found in the [Examples](https://github.com/epezent/MEL/tree/master/Examples) project, and corresponds with the saved MELScope configuration **examples.scope** in this directory (see above).

## Installation

###   Python 2.7 + NumPy + PyQt4 + PyQtGraph

**Note:** The MELScope application is written entirely in Python using PyQt4 for GUIs and PyQtGraph for plotting widgets. If you plan to use MELScope or want to develop your own GUIs for MEL programs, follow theses steps to install the Python environment.

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

**NOTE:** The newer Python 3.6 and PyQt5 have been tested and were found to be less stable and slower than the more mature Python 2.7 + PyQt4 pairing when used with PyQtGraph. You may choose to install Python 3.6 and PyQt5 (or any combination of the two, really) but note that the installation procedure will differ slightly (specifically when downloading wheels/binaries and building PyOpenGl_accelerate).


### Sublime Text 3 (Optional)

**Note:** You can optionally install Sublime Text 3 to write Python code. Following these directions will turn Sublime into a very awesome, lightweight Python IDE!

1. Download [Sublime Text 3](https://www.sublimetext.com/)

2. Install [Package Control](https://packagecontrol.io/installation) for Sublime Text 3

3. Once Package Control is installed, press Ctrl+Shft+P in Sublime to open the Command Palette. Begin typing **"install"** and select **"Package Control: Install Package"** when it appears, then type the names of the following packages and press Enter to install them.

    - [Anaconda](https://packagecontrol.io/packages/Anaconda) (**required**)
    - [SublimeLinter](https://packagecontrol.io/packages/SublimeLinter) (**required**)
    - [SublimeLinter-pyflakes](https://packagecontrol.io/packages/SublimeLinter-pyflakes) (**required**)
    - [Terminal](https://packagecontrol.io/packages/Terminal) (recommended)
    - [SideBarEnhancements](https://packagecontrol.io/packages/SideBarEnhancements) (recommended)
    - [SyncedSideBar](https://packagecontrol.io/packages/SyncedSideBar) (recommended)
    - [BracketHighlighter](https://packagecontrol.io/packages/BracketHighlighter) (recommended)
    - [Git](https://packagecontrol.io/packages/Git) (recommended)
    - [GitGutter](https://packagecontrol.io/packages/GitGutter) (recommended)

4. Once Anaconda, SublimeLinter, and SublimeLinter-pyflakes are installed, navitage to **Preferences >> Package Settings >> Anaconda >> Settings - User** and add the following settings:

    ```json
    {
      "anaconda_linting": false,
      "swallow_startup_errors": true,
      "hide_snippets_on_completion": true,
      "complete_parameters": true,
      "auto_formatting_timeout": 5
    }
    ```

5. Open any *.py Python file in Sublime. Navigate to **Preferences >> Settings - Syntax Specific** and add the following into Python.sublime-settings:

    ```json
    {
      "auto_indent": true,
      "rulers": [79],
      "smart_indent": true,
      "trim_automatic_white_space": true,
      "use_tab_stops": true,
      "word_wrap": false,
      "wrap_width": 80
    }
    ```

6. Right-click in the Python file and choose **SublimeLiner >> Toggle Linter...** from the context menu. Make sure **pyflakes** is enabled.

7. Optionally choose a new Sublime theme from [Package Control](https://packagecontrol.io/). Evan's personal favorites are [Material Theme](https://packagecontrol.io/packages/Material%20Theme) and [Spacegray](https://packagecontrol.io/packages/Theme%20-%20Spacegray) with [A File Icon](https://packagecontrol.io/packages/A%20File%20Icon) installed on top.

## Building MELScope

Python is an intepreted language, so you don't have to build MELScope -- just run MELScope.pyw with Python. However, if you want to build MELScope as a executable (.exe), use the following command in the command prompt (assuming you installed **pyinstaller**):

```
pyinstaller --noconsole --clean MELScope.pyw
```

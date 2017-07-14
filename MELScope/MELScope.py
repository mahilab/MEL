# -*- coding: utf-8 -*-
# OpenWristScope
# Evan Pezent (epezent@rice.edu, evanpezent.com)
# 06/2017

from pyqtgraph.Qt import QtGui, QtCore
from pyqtgraph.ptime import time
import numpy as np
import pyqtgraph as pg
import ctypes
import math

# version number
ver = '1.1'

# define constants
RAD2DEG = 180 / math.pi
scope_name = "scope"

# create PyQt application, main window, and layout
app = QtGui.QApplication([])
mw = QtGui.QMainWindow()
mw.setWindowTitle('MEL Scope v'+ver)
scale = 1
mw.setFixedSize(400*scale, 300*scale)
#mw.setWindowFlags(QtCore.Qt.FramelessWindowHint)
pg.setConfigOption('background', (240,240,240))
pg.setConfigOption('foreground', 'k')


widg = QtGui.QWidget()
mw.setCentralWidget(widg)
grid = QtGui.QGridLayout()
widg.setLayout(grid)
# http://zetcode.com/gui/pyqt4/layoutmanagement/
# https://stackoverflow.com/questions/33194678/qt-gridlayout-spanning-multiple-columns
# https://www.tutorialspoint.com/pyqt/pyqt_qcombobox_widget.htm

# scope duration and refersh rate
dur = 10 # each scope will span 10 seconds of data
rate = 60 # the approximate refresh rate of all scopes
samples = dur * rate # the number of data points in each scope at any given time
timeTicks = [[(0,10),(60,9),(120,8),(180,7),(240,6),(300,5),(360,4),(420,3),(480,2),(540,1),(600,'')]]

# create and style plot widgets for position
pw0 = pg.PlotWidget(name=scope_name)
pw0.setRange(xRange=[-1, samples],yRange=[-1,1])
pw0.setLimits(xMin=0,xMax=samples)
pw0.showGrid(x=True, y=True)
pw0.setDownsampling(mode='peak')
ax0l = pw0.getAxis('left')
#ax0l.setTicks([[(100,''),(-100,''),(-80,-80),(-60,''),(-40,-40),(-20,''),(0,0),(80,80),(60,''),(40,40),(20,'')]])
ax0b = pw0.getAxis('bottom')
ax0b.setTicks(timeTicks)

# create plot labels
# http://pyqt.sourceforge.net/Docs/PyQt4/qfont.html
labelFont=QtGui.QFont()
labelFont.setBold(True)

l0 = QtGui.QLabel()
l0.setText(scope_name)
l0.setAlignment(QtCore.Qt.AlignLeft)
l0.setFont(labelFont)

# create textboxes for numerical value displays
v0 = QtGui.QLabel()
v0.setText('+0.00')
v0.setAlignment(QtCore.Qt.AlignRight)

# create footer widgets
footer = QtGui.QLabel()
footer.setText('MEL Scope v' + ver + ' | MAHI Lab | Rice University')
fpsText = QtGui.QLabel()
fpsText.setText('FPS:')
fpsText.setAlignment(QtCore.Qt.AlignRight)
fpsCounter = QtGui.QLabel()
fpsCounter.setText('0')
fpsCounter.setAlignment(QtCore.Qt.AlignRight)

# add all widgets to grid layout
# grid.addWidget(pw, row, column, rowspan, colspan)
grid.addWidget(l0, 0, 0, 1, 2)
grid.addWidget(v0, 0, 2, 1, 1)

grid.addWidget(pw0, 1, 0, 1, 3)

grid.addWidget(footer,2,0,1,1)
grid.addWidget(fpsText,2,1,1,1)
grid.addWidget(fpsCounter,2,2,1,1)

# button = QtGui.QPushButton('Test')
# combo = QtGui.QComboBox()
# combo.addItems(['A','B','C'])
# grid.addWidget(button, 3,0)
# grid.addWidget(combo, 3, 1)

# create curves and data containers
curveSize = 2
curve0 = pw0.plot(pen=pg.mkPen((196, 2, 51),width=curveSize))
data0 = np.empty(samples)


# define update function
def updateScope(data,curve,v,value):
	data[:-1] = data[1:]
	data[-1] = value
	curve.setData(data)
	v.setText('%+0.2f' %value)

# create FPS monitor function
lastTime = time()
fps = None
def updateFPS():
	global lastTime, fps
	now = time()
	dt = now - lastTime
	lastTime = now
	if fps is None:
		fps = 1.0/dt
	else:
		s = np.clip(dt*3., 0, 1)
		fps = fps * (1-s) + (1.0/dt) * s
	fpsCounter.setText('%0.0f' %fps)

# create MELShare object, necessary types, and variables
mel_share = ctypes.WinDLL("MELShare.dll")
size = 2
DoubleArray = ctypes.c_double * size
values = DoubleArray()

# define main update function to be passed to Qt
def update():
    global result
    result = mel_share.read_double_map(scope_name, ctypes.byref(values), size)
    updateScope( data0, curve0, v0, values[1] )
    updateFPS()

# show the main window
mw.show()

# start the plot
t = QtCore.QTimer()
t.timeout.connect(update)
t.start(1000/rate)

## Start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
	import sys
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()

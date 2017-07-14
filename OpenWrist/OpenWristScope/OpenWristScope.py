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

# create PyQt application, main window, and layout
app = QtGui.QApplication([])
mw = QtGui.QMainWindow()
mw.setWindowTitle('OpenWrist Scope v'+ver)
scale = 1
mw.setFixedSize(800*scale, 400*scale)
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
pw0 = pg.PlotWidget(name='Joint 0 Position')
pw0.setRange(xRange=[-1, samples],yRange=[-100,100])
pw0.setLimits(xMin=0,xMax=samples,yMin=-100, yMax=100)
pw0.showGrid(x=True, y=True)
pw0.setDownsampling(mode='peak')
ax0l = pw0.getAxis('left')
ax0l.setTicks([[(100,''),(-100,''),(-80,-80),(-60,''),(-40,-40),(-20,''),(0,0),(80,80),(60,''),(40,40),(20,'')]])
ax0b = pw0.getAxis('bottom')
ax0b.setTicks(timeTicks)



pw1 = pg.PlotWidget(name='Joint 1 Position')
pw1.setRange(xRange=[-1, samples],yRange=[-75,75])
pw1.setLimits(xMin=0,xMax=samples,yMin=-75, yMax=75)
pw1.showGrid(x=True, y=True)
pw1.setDownsampling(mode='peak')
ax1l = pw1.getAxis('left')
ax1l.setTicks([[(75,''),(-75,''),(-60,-60),(-45,''),(-30,-30),(-15,''),(0,0),(60,60),(45,''),(30,30),(15,'')]])
ax1b = pw1.getAxis('bottom')
ax1b.setTicks(timeTicks)

pw2 = pg.PlotWidget(name='Joint 2 Position')
pw2.setRange(xRange=[-1, samples],yRange=[-50,50])
pw2.setLimits(xMin=0,xMax=samples,yMin=-50, yMax=50)
pw2.showGrid(x=True, y=True)
pw2.setDownsampling(mode='peak')
ax2l = pw2.getAxis('left')
ax2l.setTicks([[(50,''),(-50,''),(-40,-40),(-30,''),(-20,-20),(-10,''),(0,0),(10,''),(20,20),(30,''),(40,40)]])
ax2b = pw2.getAxis('bottom')
ax2b.setTicks(timeTicks)

# create and style plots for velocity
velocityTicks = [[(500,''),(-500,''),(-400,-400),(-300,''),(-200,-200),(-100,''),(0,0),(100,''),(200,200),(300,''),(400,400)]]
vmax = 500

pw3 = pg.PlotWidget(name='Joint 0 Velocity')
pw3.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw3.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw3.showGrid(x=True, y=True)
pw3.setDownsampling(mode='peak')
ax3l = pw3.getAxis('left')
ax3l.setTicks(velocityTicks)
ax3b = pw3.getAxis('bottom')
ax3b.setTicks(timeTicks)

pw4 = pg.PlotWidget(name='Joint 1 Velocity')
pw4.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw4.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw4.showGrid(x=True, y=True)
pw4.setDownsampling(mode='peak')
ax4l = pw4.getAxis('left')
ax4l.setTicks(velocityTicks)
ax4b = pw4.getAxis('bottom')
ax4b.setTicks(timeTicks)

pw5 = pg.PlotWidget(name='Joint 2 Velocity')
pw5.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw5.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw5.showGrid(x=True, y=True)
pw5.setDownsampling(mode='peak')
ax5l = pw5.getAxis('left')
ax5l.setTicks(velocityTicks)
ax5b = pw5.getAxis('bottom')
ax5b.setTicks(timeTicks)


# create plot labels
# http://pyqt.sourceforge.net/Docs/PyQt4/qfont.html
labelFont=QtGui.QFont()
labelFont.setBold(True)

l0 = QtGui.QLabel()
l0.setText('      Joint 0 Position [deg]:')
l0.setAlignment(QtCore.Qt.AlignLeft)
l0.setFont(labelFont)

l1 = QtGui.QLabel()
l1.setText('      Joint 1 Position [deg]:')
l1.setAlignment(QtCore.Qt.AlignLeft)
l1.setFont(labelFont)

l2 = QtGui.QLabel()
l2.setText('      Joint 2 Position [deg]:')
l2.setAlignment(QtCore.Qt.AlignLeft)
l2.setFont(labelFont)

l3 = QtGui.QLabel()
l3.setText('      Joint 0 Velocity [deg/s]:')
l3.setAlignment(QtCore.Qt.AlignLeft)
l3.setFont(labelFont)

l4 = QtGui.QLabel()
l4.setText('      Joint 1 Velocity [deg/s]:')
l4.setAlignment(QtCore.Qt.AlignLeft)
l4.setFont(labelFont)

l5 = QtGui.QLabel()
l5.setText('      Joint 2 Velocity [deg/s]:')
l5.setAlignment(QtCore.Qt.AlignLeft)
l5.setFont(labelFont)

# create textboxes for numerical value displays
v0 = QtGui.QLabel()
v0.setText('+0.00')
v0.setAlignment(QtCore.Qt.AlignRight)

v1 = QtGui.QLabel()
v1.setText('+0.00')
v1.setAlignment(QtCore.Qt.AlignRight)

v2 = QtGui.QLabel()
v2.setText('+0.00')
v2.setAlignment(QtCore.Qt.AlignRight)

v3 = QtGui.QLabel()
v3.setText('+0.00')
v3.setAlignment(QtCore.Qt.AlignRight)

v4 = QtGui.QLabel()
v4.setText('+0.00')
v4.setAlignment(QtCore.Qt.AlignRight)

v5 = QtGui.QLabel()
v5.setText('+0.00')
v5.setAlignment(QtCore.Qt.AlignRight)

# create footer widgets
footer = QtGui.QLabel()
footer.setText('OpenWrist Scope v' + ver + ' | MAHI Lab | Rice University | Evan Pezent (epezent@rice.edu)')
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
grid.addWidget(l1, 0, 3, 1, 2)
grid.addWidget(v1, 0, 5, 1, 1)
grid.addWidget(l2, 0, 6, 1, 2)
grid.addWidget(v2, 0, 8, 1, 1)

grid.addWidget(pw0, 1, 0, 1, 3)
grid.addWidget(pw1, 1, 3, 1, 3)
grid.addWidget(pw2, 1, 6, 1, 3)

grid.addWidget(l3, 2, 0, 1, 2)
grid.addWidget(v3, 2, 2, 1, 1)
grid.addWidget(l4, 2, 3, 1, 2)
grid.addWidget(v4, 2, 5, 1, 1)
grid.addWidget(l5, 2, 6, 1, 2)
grid.addWidget(v5, 2, 8, 1, 1)

grid.addWidget(pw3, 3, 0, 1, 3)
grid.addWidget(pw4, 3, 3, 1, 3)
grid.addWidget(pw5, 3, 6, 1, 3)

grid.addWidget(footer,4,0,1,7)
grid.addWidget(fpsText,4,7,1,1)
grid.addWidget(fpsCounter,4,8,1,1)

# button = QtGui.QPushButton('Test')
# combo = QtGui.QComboBox()
# combo.addItems(['A','B','C'])
# grid.addWidget(button, 3,0)
# grid.addWidget(combo, 3, 1)

# create curves and data containers
curveSize = 2
curve0 = pw0.plot(pen=pg.mkPen((196, 2, 51),width=curveSize))
data0 = np.empty(samples)

curve1 = pw1.plot(pen=pg.mkPen((0, 159, 107),width=curveSize))
data1 = np.empty(samples)

curve2 = pw2.plot(pen=pg.mkPen((0, 135, 189),width=curveSize))
data2 = np.empty(samples)

curve3 = pw3.plot(pen=pg.mkPen((211, 30, 0),width=curveSize))
data3 = np.empty(samples)

curve4 = pw4.plot(pen=pg.mkPen((90, 212, 0),width=curveSize))
data4 = np.empty(samples)

curve5 = pw5.plot(pen=pg.mkPen((55, 120, 239),width=curveSize))
data5 = np.empty(samples)


# define update function
def updateScope(data,curve,v,value):
	data[:-1] = data[1:]
	data[-1] = value
	curve.setData(data)
	v.setText('%+0.4f' %value)

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
DoubleArray10 = ctypes.c_double * 10
state = DoubleArray10()
size = ctypes.c_int(10)
result = ctypes.c_int(0);

# define main update function to be passed to Qt
def update():
    global result
    result = mel_share.read_double_map("ow_state", ctypes.byref(state), 10)
    updateScope( data0, curve0, v0, state[1] * RAD2DEG )
    updateScope( data1, curve1, v1, state[2] * RAD2DEG )
    updateScope( data2, curve2, v2, state[3] * RAD2DEG )
    updateScope( data3, curve3, v3, state[4] * RAD2DEG )
    updateScope( data4, curve4, v4, state[5] * RAD2DEG )
    updateScope( data5, curve5, v5, state[6] * RAD2DEG )
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

# -*- coding: utf-8 -*-
# MEIIScope
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
mw.setWindowTitle('Mahi Exo II Scope v'+ver)
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

pw3 = pg.PlotWidget(name='Joint 3 Position')
pw3.setRange(xRange=[-1, samples],yRange=[-50,50])
pw3.setLimits(xMin=0,xMax=samples,yMin=-75, yMax=75)
pw3.showGrid(x=True, y=True)
pw3.setDownsampling(mode='peak')
ax3l = pw1.getAxis('left')
ax3l.setTicks([[(50,''),(-50,''),(-40,-40),(-30,''),(-20,-20),(-10,''),(0,0),(10,''),(20,20),(30,''),(40,40)]])
ax3b = pw1.getAxis('bottom')
ax3b.setTicks(timeTicks)

pw4 = pg.PlotWidget(name='Joint 4 Position')
pw4.setRange(xRange=[-1, samples],yRange=[-50,50])
pw4.setLimits(xMin=0,xMax=samples,yMin=-50, yMax=50)
pw4.showGrid(x=True, y=True)
pw4.setDownsampling(mode='peak')
ax4l = pw2.getAxis('left')
ax4l.setTicks([[(50,''),(-50,''),(-40,-40),(-30,''),(-20,-20),(-10,''),(0,0),(10,''),(20,20),(30,''),(40,40)]])
ax4b = pw2.getAxis('bottom')
ax4b.setTicks(timeTicks)

# create and style plots for velocity
velocityTicks = [[(500,''),(-500,''),(-400,-400),(-300,''),(-200,-200),(-100,''),(0,0),(100,''),(200,200),(300,''),(400,400)]]
vmax = 500

pw5 = pg.PlotWidget(name='Joint 0 Velocity')
pw5.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw5.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw5.showGrid(x=True, y=True)
pw5.setDownsampling(mode='peak')
ax5l = pw3.getAxis('left')
ax5l.setTicks(velocityTicks)
ax5b = pw3.getAxis('bottom')
ax5b.setTicks(timeTicks)

pw6 = pg.PlotWidget(name='Joint 1 Velocity')
pw6.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw6.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw6.showGrid(x=True, y=True)
pw6.setDownsampling(mode='peak')
ax6l = pw4.getAxis('left')
ax6l.setTicks(velocityTicks)
ax6b = pw4.getAxis('bottom')
ax6b.setTicks(timeTicks)

pw7 = pg.PlotWidget(name='Joint 2 Velocity')
pw7.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw7.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw7.showGrid(x=True, y=True)
pw7.setDownsampling(mode='peak')
ax7l = pw5.getAxis('left')
ax7l.setTicks(velocityTicks)
ax7b = pw5.getAxis('bottom')
ax7b.setTicks(timeTicks)

pw8 = pg.PlotWidget(name='Joint 3 Velocity')
pw8.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw8.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw8.showGrid(x=True, y=True)
pw8.setDownsampling(mode='peak')
ax8l = pw4.getAxis('left')
ax8l.setTicks(velocityTicks)
ax8b = pw4.getAxis('bottom')
ax8b.setTicks(timeTicks)

pw9 = pg.PlotWidget(name='Joint 4 Velocity')
pw9.setRange(xRange=[-1, samples],yRange=[-vmax,vmax])
pw9.setLimits(xMin=0,xMax=samples,yMin=-vmax, yMax=vmax)
pw9.showGrid(x=True, y=True)
pw9.setDownsampling(mode='peak')
ax9l = pw5.getAxis('left')
ax9l.setTicks(velocityTicks)
ax9b = pw5.getAxis('bottom')
ax9b.setTicks(timeTicks)


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
l3.setText('      Joint 3 Position [deg]:')
l3.setAlignment(QtCore.Qt.AlignLeft)
l3.setFont(labelFont)

l4 = QtGui.QLabel()
l4.setText('      Joint 4 Position [deg]:')
l4.setAlignment(QtCore.Qt.AlignLeft)
l4.setFont(labelFont)

l5 = QtGui.QLabel()
l5.setText('      Joint 0 Velocity [deg/s]:')
l5.setAlignment(QtCore.Qt.AlignLeft)
l5.setFont(labelFont)

l6 = QtGui.QLabel()
l6.setText('      Joint 1 Velocity [deg/s]:')
l6.setAlignment(QtCore.Qt.AlignLeft)
l6.setFont(labelFont)

l7 = QtGui.QLabel()
l7.setText('      Joint 2 Velocity [deg/s]:')
l7.setAlignment(QtCore.Qt.AlignLeft)
l7.setFont(labelFont)

l8 = QtGui.QLabel()
l8.setText('      Joint 3 Velocity [deg/s]:')
l8.setAlignment(QtCore.Qt.AlignLeft)
l8.setFont(labelFont)

l9 = QtGui.QLabel()
l9.setText('      Joint 4 Velocity [deg/s]:')
l9.setAlignment(QtCore.Qt.AlignLeft)
l9.setFont(labelFont)

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

v6 = QtGui.QLabel()
v6.setText('+0.00')
v6.setAlignment(QtCore.Qt.AlignRight)

v7 = QtGui.QLabel()
v7.setText('+0.00')
v7.setAlignment(QtCore.Qt.AlignRight)

v8 = QtGui.QLabel()
v8.setText('+0.00')
v8.setAlignment(QtCore.Qt.AlignRight)

v9 = QtGui.QLabel()
v9.setText('+0.00')
v9.setAlignment(QtCore.Qt.AlignRight)

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
grid.addWidget(l3, 0, 9, 1, 2)
grid.addWidget(v3, 0, 11, 1, 1)
grid.addWidget(l4, 0, 12, 1, 2)
grid.addWidget(v4, 0, 14, 1, 1)

grid.addWidget(pw0, 1, 0, 1, 3)
grid.addWidget(pw1, 1, 3, 1, 3)
grid.addWidget(pw2, 1, 6, 1, 3)
grid.addWidget(pw3, 1, 9, 1, 3)
grid.addWidget(pw4, 1, 12, 1, 3)



grid.addWidget(l5, 2, 0, 1, 2)
grid.addWidget(v5, 2, 2, 1, 1)
grid.addWidget(l6, 2, 3, 1, 2)
grid.addWidget(v6, 2, 5, 1, 1)
grid.addWidget(l7, 2, 6, 1, 2)
grid.addWidget(v7, 2, 8, 1, 1)
grid.addWidget(l8, 2, 9, 1, 2)
grid.addWidget(v8, 2, 11, 1, 1)
grid.addWidget(l9, 2, 12, 1, 2)
grid.addWidget(v9, 2, 14, 1, 1)


grid.addWidget(pw5, 3, 0, 1, 3)
grid.addWidget(pw6, 3, 3, 1, 3)
grid.addWidget(pw7, 3, 6, 1, 3)
grid.addWidget(pw8, 3, 9, 1, 3)
grid.addWidget(pw9, 3, 12, 1, 3)


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

curve6 = pw6.plot(pen=pg.mkPen((196, 2, 51),width=curveSize))
data6 = np.empty(samples)

curve7 = pw7.plot(pen=pg.mkPen((0, 159, 107),width=curveSize))
data7 = np.empty(samples)

curve8 = pw8.plot(pen=pg.mkPen((0, 135, 189),width=curveSize))
data8 = np.empty(samples)

curve9 = pw9.plot(pen=pg.mkPen((211, 30, 0),width=curveSize))
data9 = np.empty(samples)


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
    result = mel_share.read_double_map("cpp2py", ctypes.byref(state), 10)
    updateScope( data0, curve0, v0, state[0] * RAD2DEG )
    updateScope( data1, curve1, v1, state[1] * RAD2DEG )
    updateScope( data2, curve2, v2, state[2] * RAD2DEG )
    updateScope( data3, curve3, v3, state[3] * RAD2DEG )
    updateScope( data4, curve4, v4, state[4] * RAD2DEG )
    updateScope( data5, curve5, v5, state[5] * RAD2DEG )
    updateScope( data6, curve6, v6, state[6] * RAD2DEG )
    updateScope( data7, curve7, v7, state[7] * RAD2DEG )
    updateScope( data8, curve8, v8, state[8] * RAD2DEG )
    updateScope( data9, curve9, v9, state[9] * RAD2DEG )
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

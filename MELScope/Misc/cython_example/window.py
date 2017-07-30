# -*- coding: utf-8 -*-

from pyqtgraph.Qt import QtGui, QtCore
from PyQt4.QtCore import pyqtSlot
import pyqtgraph as pg

class Button(QtGui.QPushButton):
    def __init__(self, *args):
        super(self.__class__, self).__init__(*args)

        self.destroyed.connect(self.doSomeDestruction)
        self.clicked.connect(self.onClicked)

        # old style signal: old-style slots are always disconnected
        # self.connect(self, QtCore.SIGNAL("clicked()"), self.onClicked)

    @pyqtSlot()
    def onClicked(self):
        print 'clicked!'

    @pyqtSlot()
    def doSomeDestruction(self):
        print 'destroy button!'
        self.clicked.disconnect()
        self.destroyed.disconnect()

class CustomWindow(pg.GraphicsWindow):
    def __init__(self, *args, **kwargs):
        super(self.__class__, self).__init__(*args, **kwargs)

        # create a push button that says OK.
        self.btn = Button('OK')

        # add QPushButton to the GraphicsWindow:
        self.btn.setParent(self)
        self.btn.show()

    def add(self, plot):
        self.plot = plot
        self.addItem(plot)

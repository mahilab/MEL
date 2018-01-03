from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg

## Start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
    import sys
    from window import CustomWindow


    app = QtGui.QApplication([])

    win = CustomWindow(title="Basic plotting examples")
    win.resize(1000,600)
    win.setWindowTitle('plot')

    # adding a PlotItem to a GraphicsWindow through a constructor, and addItem
    title = "Basic array plotting"
    p1 = pg.PlotItem(title=title)
    win.add(p1)

    # adding a data set through PlotDataItem constructor:
    d = pg.PlotDataItem(name='data set 1', y=np.random.normal(size=100))
    p1.addItem(d)

    p1.showAxis('bottom')

    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

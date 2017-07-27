################################################
# MEL Scope
# Evan Pezent (epezent@rice.edu, evanpezent.com)
# Mechatronics and Haptic Interfaces (MAHI) Lab
# Rice University
# 06/2017 -
################################################

from pyqtgraph.Qt import QtGui, QtCore
from pyqtgraph.ptime import time
import numpy as np
import pyqtgraph
import ctypes
import webbrowser
import math
import os
import subprocess
import sys
import yaml

#######################
# MEL SCOPE ABOUT INFO
#######################
ver = '1.2'

########################################
# PYQT APP / MAIN WINDOW / LAYOUT SETUP
########################################

app = QtGui.QApplication([])

main_window = QtGui.QMainWindow()
main_window.setWindowTitle('MEL Scope v'+ver)

screen_resolution = app.desktop().screenGeometry()
resolution_scale = screen_resolution.width() / 1920.0
main_window.setFixedSize(400*resolution_scale, 300*resolution_scale)

pyqtgraph.setConfigOption('background', (240,240,240))
pyqtgraph.setConfigOption('foreground', 'k')

widg = QtGui.QWidget()
main_window.setCentralWidget(widg)
grid = QtGui.QGridLayout() # http://zetcode.com/gui/pyqt4/layoutmanagement/
grid.setContentsMargins(0,0,0,0)
widg.setLayout(grid)

##################
# STATUS BAR SETUP
##################

status_bar = QtGui.QStatusBar()
status_bar.setSizeGripEnabled(False)
main_window.setStatusBar(status_bar)

fps_label = QtGui.QLabel()
fps_label.setText('0')
fps_label.setAlignment(QtCore.Qt.AlignRight)

status_bar.addPermanentWidget(fps_label)

###############################
# TIMING VARIABLES / FUNCTIONS
###############################

time_start   = time()
time_last    = time()
time_now     = time()
delta_time   = 0
elapsed_time = 0

fps_target = 60
fps_actual = None

def update_time():
    global time_last, time_now, delta_time, elapsed_time
    time_now = time()
    delta_time = time_now - time_last
    elapsed_time += delta_time
    time_last = time_now

def update_FPS():
    global delta_time, fps_actual
    if fps_actual is None:
        fps_actual = 1.0/delta_time
    else:
        s = np.clip(delta_time*3., 0, 1)
        fps_actual = fps_actual * (1-s) + (1.0/delta_time) * s
    fps_label.setText('FPS: %0.0f ' %fps_actual)

#################################
# MELSHARE VARIABLES / FUNCTIONS
#################################

melshare = ctypes.WinDLL("MELShare.dll")
melshare_name = ''
melshare_size = 0
double_array = ctypes.c_double * melshare_size
melshare_buffer = double_array()
connected = False

def connect_melshare():
    global melshare_name, melshare_size, melshare_buffer, double_array, connected
    input, ok = QtGui.QInputDialog.getText(widg,'MELShare', 'Enter the MELShare Name:')
    if ok:
        melshare_name = str(input)
        result = melshare.get_map_size(melshare_name);
        if result >= 0:
            melshare_size = result
            connected = True
            double_array = ctypes.c_double * melshare_size
            melshare_buffer = double_array()
            for scope in scope_modules:
                scope.clear()
            status_bar.showMessage('Connected to MELShare <'+input+'>')
        else:
            connected = False
            status_bar.showMessage('Failed to connect to MELShare  <'+input+'>')

######################################
# SCOPE CLASS / VARIABLES / FUNCTIONS
######################################

curve_size = 3

curve_colors = [ (31,  120, 180) , (227, 26,  28 ) , (51,  160, 44 ) ,
                 (255, 127, 0  ) , (106, 61,  154) , (177, 89,  40 ) ,
                 (166, 206, 227) , (251, 154, 153) , (178, 223, 138) ,
                 (253, 191, 111) , (202, 178, 214) , (255, 255, 153) ]
# http://colorbrewer2.org

label_padding = ' ' * 7
label_font = QtGui.QFont()
label_font.setBold(True)

class ScopeModule(QtGui.QWidget):

    def __init__(self):

        super(ScopeModule,self).__init__()

        # scope duration and refresh rate
        self.duration = 10 # each scope will span 10 seconds of data
        self.samples = self.duration * fps_target # the number of data points in each scope at any given time
        self.time_ticks = [[(0,10),(60,9),(120,8),(180,7),(240,6),(300,5),
                            (360,4),(420,3),(480,2),(540,1),(600,'0   ')]]

        # scope module plot widget
        self.plot = pyqtgraph.PlotWidget(name='MELScope')
        self.plot.setRange(xRange=[-1, self.samples],yRange=[-1,1])
        self.plot.setLimits(xMin=0,xMax=self.samples)
        self.plot.showGrid(x=True, y=True)
        self.plot.setDownsampling(mode='peak')
        self.axis_left = self.plot.getAxis('left')
        self.axis_bottom = self.plot.getAxis('bottom')
        self.axis_bottom.setTicks(self.time_ticks)

        # scope module curves and data
        self.all_curves = []
        self.all_data  = []

        self.title = 'Double-click to rename scope'

        # scope title label
        self.label = QtGui.QLabel()
        self.label.setText(self.title)
        self.label.mouseDoubleClickEvent = self.initiate_rename
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setFont(label_font)

        # scope module layout
        self.layout = QtGui.QGridLayout() # http://zetcode.com/gui/pyqt4/layoutmanagement/
        self.setLayout(self.layout)

        self.layout.addWidget(self.label, 0, 0, 1, 1)
        self.layout.addWidget(self.plot, 1, 0, 1, 1)

    def initiate_rename(self, event=None):
        self.line_edit = QtGui.QLineEdit()
        self.line_edit.setText(self.title)
        self.line_edit.setAlignment(QtCore.Qt.AlignCenter)
        self.line_edit.setFont(label_font)
        self.line_edit.editingFinished.connect(self.confirm_rename)
        self.line_edit.focusOutEvent = self.confirm_rename
        self.layout.addWidget(self.line_edit, 0, 0, 1, 1)
        self.line_edit.selectAll()
        self.line_edit.setFocus()

    def confirm_rename(self, event=None):
        self.title = self.line_edit.text()
        self.label.setText(self.title)
        self.layout.removeWidget(self.line_edit)
        self.line_edit.deleteLater()
        self.line_edit = None

    def clear(self):
        self.plot.clear()
        self.all_curves = []
        self.all_data  = []
        for i in range(melshare_size):
            self.all_curves.append(self.plot.plot(pen=pyqtgraph.mkPen(curve_colors[i],width=curve_size)))
            self.all_data.append(np.zeros(self.samples))

    def update_curve(self,data,curve,value):
        data[:-1] = data[1:]
        data[-1] = value
        curve.setData(data)

    def update(self):
        for (curve, data, i) in zip(self.all_curves, self.all_data, range(melshare_size)):
            self.update_curve(data, curve, melshare_buffer[i])

scope_modules = [ScopeModule()]

###############################
# GRID VARIABLES / FUNCTIONS
##############################

grid_rows = 1
grid_cols = 1

options = [ (1,1), (1,2), (1,3),
            (2,1), (2,2), (2,3),
            (3,1), (3,2), (3,3) ]

options_display = [ str(x[0]) + ' x ' + str(x[1]) for x in options ]

options_dict = dict(zip(options_display, options))

scope_count = grid_rows * grid_cols

def resize_grid():
    global grid_rows, grid_cols, scope_count
    selection, ok = QtGui.QInputDialog.getItem(widg, 'Grid Size', 'Select Grid Size:', options_display, 0, False)
    if ok:
        grid_rows, grid_cols = options_dict[str(selection)]
        new_count = grid_rows * grid_cols
        # remove scopes from grid
        for i in range(scope_count):
            grid.removeWidget(scope_modules[i])
        # add new scopes
        if new_count > scope_count:
            add = new_count - scope_count
            for i in range(add):
                scope_modules.append(ScopeModule())
        # or remove old scopes
        elif new_count < scope_count:
            remove = scope_count - new_count
            for i in range(remove):
                scope_modules.pop().deleteLater()
        # readd scopes to grid
        positions = [(r,c) for r in range(grid_rows) for c in range(grid_cols)]
        for i in range(new_count):
            grid.addWidget(scope_modules[i], positions[i][0], positions[i][1], 1, 1)
        # update scope count
        scope_count = new_count
        # resize window
        main_window.setFixedSize(400*grid_cols*resolution_scale, 300*grid_rows*resolution_scale)









def load_grid():
    pass

grid.addWidget(scope_modules[0], 0, 0, 1, 1)

##################################
# OPEN/SAVE VARIABLES / FUNCTIONS
##################################

config = {}
filepath = None
up_to_date = False

def open_new_instance():
    CREATE_NO_WINDOW = 0x08000000
    subprocess.Popen([sys.executable, 'MELScope.pyw'], creationflags=CREATE_NO_WINDOW)
# https://stackoverflow.com/questions/89228/calling-an-external-command-in-python#2251026
# https://msdn.microsoft.com/en-us/library/windows/desktop/ms684863(v=vs.85).aspx

def open_scope():
    global config, filepath
    filepath = QtGui.QFileDialog.getOpenFileName(widg,'Open MELScope',"",'Scope Files (*.scope *.yaml)')
    if filepath:
        stream = file(filepath,'r')
        config = yaml.load(stream)
        filename = filepath[str(filepath).rfind('/')+1:]
        status_bar.showMessage('Opened scope from <' + filename + '>')

def save_scope():
    global config, filepath
    if filepath:
        stream = file(filepath, 'w')
        yaml.dump(config,stream)
        filename = filepath[str(filepath).rfind('/')+1:]
        status_bar.showMessage('Saved scope to <' + filename + '>')
    else:
        save_scope_as()


def save_scope_as():
    global config, filepath
    new_filepath = QtGui.QFileDialog.getSaveFileName(widg,'Save MELScope',"",'Scope Files (*.scope *.yaml)')
    if new_filepath:
        filepath = new_filepath
        save_scope()


def load_config():
    global config
    pass

def save_config():
    global config
    pass


def open_github():
    webbrowser.open('https://github.com/epezent/MEL')



#################
# MENU BAR SETUP
#################

menu_bar = main_window.menuBar()

file_menu = menu_bar.addMenu('File')
edit_menu = menu_bar.addMenu('Edit')
help_menu = menu_bar.addMenu('Help')

new_action = QtGui.QAction('New',main_window)
new_action.setShortcut('Ctrl+N')
new_action.triggered.connect(open_new_instance)
file_menu.addAction(new_action)

open_action = QtGui.QAction('Open Scope...',main_window)
open_action.setShortcut('Ctrl+O')
open_action.triggered.connect(open_scope)
file_menu.addAction(open_action)

save_action = QtGui.QAction('Save Scope',main_window)
save_action.setShortcut('Ctrl+S')
save_action.triggered.connect(save_scope)
file_menu.addAction(save_action)

save_as_action = QtGui.QAction('Save Scope As...',main_window)
save_as_action.setShortcut('Ctrl+Shift+S')
save_as_action.triggered.connect(save_scope_as)
file_menu.addAction(save_as_action)

connect_action = QtGui.QAction('Connect MELShare...',main_window)
connect_action.setShortcut('Ctrl+C')
connect_action.triggered.connect(connect_melshare)
edit_menu.addAction(connect_action)

grid_action = QtGui.QAction('Change Grid Size...',main_window)
grid_action.setShortcut('Ctrl+G')
grid_action.triggered.connect(resize_grid)
edit_menu.addAction(grid_action)

about_action = QtGui.QAction('About', main_window)
help_menu.addAction(about_action)

github_action = QtGui.QAction('GitHub', main_window)
github_action.triggered.connect(open_github)
help_menu.addAction(github_action)




#############################
# MAIN APPLICATION EXECUTION
#############################

# define main update function to be passed to Qt
def update():
    global connected
    update_time()
    update_FPS()
    if connected:
        melshare.read_double_map(melshare_name, ctypes.byref(melshare_buffer), melshare_size)
        for scope in scope_modules:
            scope.update()

# start the plot
timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(1000/fps_target)

# show the main window
main_window.show()

# start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
	if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
		QtGui.QApplication.instance().exec_()

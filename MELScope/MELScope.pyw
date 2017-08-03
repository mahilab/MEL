################################################
# MEL Scope
# Evan Pezent (epezent@rice.edu, evanpezent.com)
# Mechatronics and Haptic Interfaces (MAHI) Lab
# Rice University
# 06/2017 -
################################################

from pyqtgraph.Qt import QtGui, QtCore
from pyqtgraph.ptime import time
import pyqtgraph.widgets.RemoteGraphicsView
import numpy as np
import pyqtgraph
import ctypes
import webbrowser
import subprocess
import sys
import yaml
import collections
import qdarkstyle

# TO DO:
# splitter widget/
# add cbrewer colors to color picker
# turn scopes on off
# thread melshare
# save window size
# curve scaling (eg deg2rad)
# scope modules to generic modules, add interactive gain modules with sliders
# open from .scope
# add scope mode
# change grid item class structure to be less bulky and titles separate
# change grid dialog that uses checkboxes
# each scope has own melshare?
# optimize calls to sample time and data
# manage MELShare(s) dialog

#######################                integral_ += (t - last_t_) * (0.5 * (last_x_ + x));

# MEL SCOPE ABOUT INFO
#######################
ver = '1.3'

########################################
# PYQT APP / MAIN WINDOW / LAYOUT SETUP
#########################################

app = QtGui.QApplication([])

main_window = QtGui.QMainWindow()
main_window.setWindowTitle('MELScope (untitled)')

screen_resolution = app.desktop().screenGeometry()
resolution_scale = screen_resolution.width() / 1920.0

widg = QtGui.QWidget()
main_window.setCentralWidget(widg)
main_window.resize(640, 640)
widg.setContentsMargins(12, 12, 12, 12)
grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
grid.setContentsMargins(0, 0, 0, 0)
widg.setLayout(grid)

bold_font = QtGui.QFont()
bold_font.setBold(True)

#==============================================================================
# THEME SETUP
#==============================================================================

THEME_OPTIONS = ['Classic', 'Dark']
THEME_STYLESHEETS = {'Classic': '',
                     'Dark': qdarkstyle.load_stylesheet(pyside=False)}
THEME_SCOPE_BG_COLORS = {"Classic": [240, 240, 240], "Dark": [49, 54, 59]}
THEME_SCOPE_FB_COLORS = {"Classic": [0, 0, 0], "Dark": [240, 240, 240]}
THEME_SCOPE_VB_COLORS = {"Classic": [240, 240, 240], "Dark": [35, 38, 41]}
THEME_SCOPE_RW_COLORS = {"Classic": [204, 232, 255], "Dark": [24, 70, 93]}

theme = 'Dark'

def set_theme():
    global app
    app.setStyleSheet(THEME_STYLESHEETS[theme])
    pyqtgraph.setConfigOption('background', THEME_SCOPE_BG_COLORS[theme])
    pyqtgraph.setConfigOption('foreground', THEME_SCOPE_FB_COLORS[theme])
    refresh_scopes()


#==============================================================================
# STATUS BAR SETUP
#==============================================================================

status_bar = QtGui.QStatusBar()
status_bar.setSizeGripEnabled(False)
main_window.setStatusBar(status_bar)

sample_rate_label = QtGui.QLabel()
sample_rate_label.setText('0')
sample_rate_label.setAlignment(QtCore.Qt.AlignRight)
sample_rate_label.setFixedWidth(50)


fps_label = QtGui.QLabel()
fps_label.setText('0')
fps_label.setAlignment(QtCore.Qt.AlignRight)
fps_label.setFixedWidth(50)

pause_label = QtGui.QLabel()
pause_label.setText('')
pause_label.setFont(bold_font)
pause_label.setAlignment(QtCore.Qt.AlignLeft)

status_bar.addPermanentWidget(pause_label)
status_bar.addPermanentWidget(fps_label)
status_bar.addPermanentWidget(sample_rate_label)

#==============================================================================
# TIMING VARIABLES / FUNCTIONS
#==============================================================================

TIME_LAST_FPS = time()
TIME_NOW_FPS = time()
DELTA_TIME_FPS = 0

TIME_LAST_SR = time()
TIME_NOW_SR = time()
DELTA_TIME_SR = 0

FPS_TARGET = 60
FPS_ACTUAL = None
SAMPLE_TARGET = 100
SAMPLE_RATE = None

def update_render_time():
    global TIME_LAST_FPS, TIME_NOW_FPS, DELTA_TIME_FPS
    TIME_NOW_FPS = time()
    DELTA_TIME_FPS = TIME_NOW_FPS - TIME_LAST_FPS
    TIME_LAST_FPS = TIME_NOW_FPS

def update_sample_time():
    global TIME_LAST_SR, TIME_NOW_SR, DELTA_TIME_SR
    TIME_NOW_SR = time()
    DELTA_TIME_SR = TIME_NOW_SR - TIME_LAST_SR
    TIME_LAST_SR = TIME_NOW_SR

def update_FPS():
    global DELTA_TIME_FPS, FPS_ACTUAL
    if FPS_ACTUAL is None:
        FPS_ACTUAL = 1.0 / DELTA_TIME_FPS
    else:
        s = np.clip(DELTA_TIME_FPS * 3., 0, 1)
        FPS_ACTUAL = FPS_ACTUAL * (1 - s) + (1.0 / DELTA_TIME_FPS) * s
    fps_label.setText('%0.0f FPS ' % FPS_ACTUAL)

def update_sample_rate():
    global DELTA_TIME_SR, SAMPLE_RATE
    if SAMPLE_RATE is None:
        SAMPLE_RATE = 1.0 / DELTA_TIME_SR
    else:
        s = np.clip(DELTA_TIME_SR * 3., 0, 1)
        SAMPLE_RATE = SAMPLE_RATE * (1 - s) + (1.0 / DELTA_TIME_SR) * s
    sample_rate_label.setText('%0.0f Hz ' % SAMPLE_RATE)

#==============================================================================
# MELSHARE VARIABLES / FUNCTIONS
#==============================================================================

MELSHARE_DLL = ctypes.WinDLL("MELShare.dll")

MELSHARE_MODES_OPTIONS = ['Read Only', 'Write Only']

MELSHARE_NAMES = []
MELSHARE_MODES = {}
MELSHARE_SIZES = {}
MELSHARE_CONNECTIONS = {}
MELSHARE_DOUBLE_ARRAYS  = {}
MELSHARE_BUFFERS =  {}
MELSHARE_SAMPLED_TIMES = {}
MELSHARE_SAMPLED_DATA = {}
MELSHARE_SAMPLED_DATA_TEXT = {}

def add_melshare(name, mode):
    if name not in MELSHARE_NAMES:
        MELSHARE_NAMES.append(name)
        MELSHARE_MODES[name] = mode
        result = MELSHARE_DLL.get_map_size(name)
        if result >= 0:
            MELSHARE_SIZES[name] = result
            MELSHARE_CONNECTIONS[name] = True
            MELSHARE_DOUBLE_ARRAYS[name] = ctypes.c_double * result
            MELSHARE_BUFFERS[name] = MELSHARE_DOUBLE_ARRAYS[name]()
        else:
            MELSHARE_SIZES[name] = 0
            MELSHARE_CONNECTIONS[name] = False
            MELSHARE_DOUBLE_ARRAYS[name] = None
            MELSHARE_BUFFERS[name] = None
        MELSHARE_SAMPLED_TIMES[name] = None
        MELSHARE_SAMPLED_DATA[name] = None
        MELSHARE_SAMPLED_DATA_TEXT[name] = None
        DATA_NAMES[name] = []
        CURVE_SIZES[name] = []
        CURVE_COLORS[name] = []
        CURVE_LINES[name] = []

def reconnect_melshare(name):
    # print 'CALL: reconnect_melshare()'
    if name in MELSHARE_NAMES:
        result = MELSHARE_DLL.get_map_size(name)
        if result >= 0:
            MELSHARE_SIZES[name] = result
            MELSHARE_CONNECTIONS[name] = True
            MELSHARE_DOUBLE_ARRAYS[name] = ctypes.c_double * result
            MELSHARE_BUFFERS[name] = MELSHARE_DOUBLE_ARRAYS[name]()
        else:
            MELSHARE_CONNECTIONS[name] = False
            MELSHARE_DOUBLE_ARRAYS[name] = None
            MELSHARE_BUFFERS[name] = None
        MELSHARE_SAMPLED_TIMES[name] = None
        MELSHARE_SAMPLED_DATA[name] = None
        MELSHARE_SAMPLED_DATA_TEXT[name] = None
    # print_melshare_info()

def purge_melshare_dicts(name):
    del MELSHARE_MODES[name]
    del MELSHARE_SIZES[name]
    del MELSHARE_CONNECTIONS[name]
    del MELSHARE_DOUBLE_ARRAYS[name]
    del MELSHARE_BUFFERS[name]
    del MELSHARE_SAMPLED_TIMES[name]
    del MELSHARE_SAMPLED_DATA[name]
    del MELSHARE_SAMPLED_DATA_TEXT[name]
    del DATA_NAMES[name]
    del CURVE_SIZES[name]
    del CURVE_COLORS[name]
    del CURVE_LINES[name]

def remove_melshare(name):
    # print "CALL: remove_melshare()"
    if name in MELSHARE_NAMES:
        purge_melshare_dicts(name)
        MELSHARE_NAMES.remove(name)
    # print_melshare_info()

def remove_all_melshares():
    # print "CALL: remove_all_melshares()"
    global MELSHARE_NAMES
    for name in MELSHARE_NAMES:
        purge_melshare_dicts(name)
    MELSHARE_NAMES = []

def read_melshare(name):
    if MELSHARE_CONNECTIONS[name]:
        return MELSHARE_DLL.read_double_map(name,
            ctypes.byref(MELSHARE_BUFFERS[name]), MELSHARE_SIZES[name])

def write_melshare(name):
    if MELSHARE_CONNECTIONS[name]:
        return MELSHARE_DLL.write_double_map(name,
        ctypes.byref(MELSHARE_BUFFERS[name]), MELSHARE_SIZES[name])

def melshares_connected():
    if len(MELSHARE_CONNECTIONS) == 0:
        return False
    for connection in MELSHARE_CONNECTIONS:
        if connection is False:
            return False
    return True

def print_melshare_info():
    print '    MELSHARE_NAMES:        ', MELSHARE_NAMES
    print '    MELSHARE_MODES:        ', MELSHARE_MODES
    print '    MELSHARE_SIZES:        ', MELSHARE_SIZES
    print '    MELSHARE_CONNECTIONS:  ', MELSHARE_CONNECTIONS
    print '    MELSHARE_DOUBLE_ARRAYS:', MELSHARE_DOUBLE_ARRAYS
    print '    MELSHARE_BUFFERS:      ', MELSHARE_BUFFERS

SAMPLE_DURATION = 10
NUM_SAMPLES = int(SAMPLE_DURATION * SAMPLE_TARGET * 1.1)


def save_melshare_data(name):
    global MELSHARE_BUFFERS, MELSHARE_SAMPLED_DATA_TEXT, MELSHARE_SAMPLED_TIMES, MELSHARE_SAMPLED_DATA, MELSHARE_SIZES, DELTA_TIME_SR
    MELSHARE_SAMPLED_DATA_TEXT[name] = ['%0.4f' % value for value in MELSHARE_BUFFERS[name]]
    MELSHARE_SAMPLED_TIMES[name][:-1] = MELSHARE_SAMPLED_TIMES[name][1:] - DELTA_TIME_SR
    MELSHARE_SAMPLED_TIMES[name][-1] = 0
    for (data, i) in zip(MELSHARE_SAMPLED_DATA[name], range(MELSHARE_SIZES[name])):
        data[:-1] = data[1:]
        data[-1] = MELSHARE_BUFFERS[name][i]

def reset_melshare_data(name):
    # print "CALL: reset_melshare_data()"
    global NUM_SAMPLES, MELSHARE_SAMPLED_TIMES, MELSHARE_SAMPLED_DATA
    MELSHARE_SAMPLED_TIMES[name] = None
    MELSHARE_SAMPLED_DATA[name] = []
    MELSHARE_SAMPLED_TIMES[name] = np.zeros(NUM_SAMPLES)
    read_melshare(name)
    for i in range(MELSHARE_SIZES[name]):
        MELSHARE_SAMPLED_DATA[name].append(np.ones(NUM_SAMPLES) * MELSHARE_BUFFERS[name][i])
    # print_melshare_info()

#==============================================================================
# DATA / CURVE PROPERTIES SETUP
#==============================================================================

DEFAULT_CURVE_SIZE = 1.5 * resolution_scale
DEFAULT_CURVE_COLORS = [[31, 120, 180], [227, 26, 28], [51, 160, 44],
                        [255, 127, 0], [106, 61, 154], [177, 89, 40],
                        [166, 206, 227], [251, 154, 153], [178, 223, 138],
                        [253, 191, 111], [202, 178, 214], [255, 255, 153],
                        [141,211,199], [255,255,179], [190,186,218],
                        [251,128,114], [128,177,211], [253,180,98],
                        [179,222,105], [252,205,229], [217,217,217],
                        [188,128,189], [204,235,197], [255,237,111]]
                        # http://colorbrewer2.org/

DEFAULT_CURVE_LINE = 'Solid'
CURVE_LINE_OPTIONS = collections.OrderedDict([
    ('Solid', QtCore.Qt.SolidLine),
    ('Dash', QtCore.Qt.DashLine),
    ('Dot', QtCore.Qt.DotLine),
    ('Dash Dot', QtCore.Qt.DashDotLine),
    ('Dash Dot Dot', QtCore.Qt.DashDotDotLine)])

DATA_NAMES = {}
CURVE_SIZES = {}
CURVE_COLORS = {}
CURVE_LINES = {}

def validate_curve_properties(name):
    global DATA_NAMES, CURVE_SIZES, CURVE_COLORS, CURVE_LINES
    if MELSHARE_SIZES[name] > len(DATA_NAMES[name]):
        for i in range(len(DATA_NAMES[name]), MELSHARE_SIZES[name]):
            DATA_NAMES[name].append(name + '_data_' + str(i))
    else:
        DATA_NAMES[name] = DATA_NAMES[name][:MELSHARE_SIZES[name]]
    if MELSHARE_SIZES[name] > len(CURVE_SIZES[name]):
        for i in range(len(CURVE_SIZES[name]), MELSHARE_SIZES[name]):
            CURVE_SIZES[name].append(DEFAULT_CURVE_SIZE)
    else:
        CURVE_SIZES[name] = CURVE_SIZES[name][:MELSHARE_SIZES[name]]
    if MELSHARE_SIZES[name] > len(CURVE_COLORS[name]):
        for i in range(len(CURVE_COLORS[name]), MELSHARE_SIZES[name]):
            next_color = DEFAULT_CURVE_COLORS.pop(0)
            DEFAULT_CURVE_COLORS.append(next_color);
            CURVE_COLORS[name].append(next_color)
    else:
        CURVE_COLORS[name] = CURVE_COLORS[name][:MELSHARE_SIZES[name]]
    if MELSHARE_SIZES[name] > len(CURVE_LINES[name]):
        for i in range(len(CURVE_LINES[name]), MELSHARE_SIZES[name]):
            CURVE_LINES[name].append(DEFAULT_CURVE_LINE)
    else:
        CURVE_LINES[name] = CURVE_LINES[name][:MELSHARE_SIZES[name]]

#==============================================================================
# SCOPE CLASS / VARIABLES / FUNCTIONS
#==============================================================================

SCOPE_COUNT = 0
SCOPE_MODULES = []

DEFAULT_SCOPE_MODE = 'Scope'
SCOPE_MODE_OPTIONS = ['Scope', 'I/O']

# scope properties
SCOPE_TITLES = []
SCOPE_MODES = []
SCOPE_FILTERS = []
SCOPE_LEGENDS = []
SCOPE_RANGES = []


def validate_scope_properties():
    global SCOPE_TITLES, SCOPE_MODES, SCOPE_FILTERS, SCOPE_LEGENDS, SCOPE_RANGES
    if SCOPE_COUNT > len(SCOPE_TITLES):
        for i in range(len(SCOPE_TITLES), SCOPE_COUNT):
            SCOPE_TITLES.append('scope_' + str(i))
    else:
        SCOPE_TITLES = SCOPE_TITLES[:SCOPE_COUNT]
    if SCOPE_COUNT > len(SCOPE_MODES):
        for i in range(len(SCOPE_MODES), SCOPE_COUNT):
            SCOPE_MODES.append(DEFAULT_SCOPE_MODE)
    else:
        SCOPE_MODES = SCOPE_MODES[:SCOPE_COUNT]

    if SCOPE_COUNT > len(SCOPE_FILTERS):
        for i in range(len(SCOPE_FILTERS), SCOPE_COUNT):
            new_filter = {}
            for name in MELSHARE_NAMES:
                new_melshare_filter = [True for j in range(MELSHARE_SIZES[name])]
                new_filter[name] = new_melshare_filter
            SCOPE_FILTERS.append(new_filter)
    else:
        SCOPE_FILTERS = SCOPE_FILTERS[:SCOPE_COUNT]
    for filter in SCOPE_FILTERS:
        for name in MELSHARE_NAMES:
            if name not in filter:
                filter[name] = []
            if MELSHARE_SIZES[name] > len(filter[name]):
                for i in range(len(filter[name]), MELSHARE_SIZES[name]):
                    filter[name].append(True)
            else:
                filter[name] = filter[name][:MELSHARE_SIZES[name]]
        for key, value in filter.items():
            if key not in MELSHARE_NAMES:
                del filter[key]

    if SCOPE_COUNT > len(SCOPE_LEGENDS):
        for i in range(len(SCOPE_LEGENDS), SCOPE_COUNT):
            SCOPE_LEGENDS.append(False)
    else:
        SCOPE_LEGENDS = SCOPE_LEGENDS[:SCOPE_COUNT]
    if SCOPE_COUNT > len(SCOPE_RANGES):
        for i in range(len(SCOPE_RANGES), SCOPE_COUNT):
            SCOPE_RANGES.append([-1, 1])
    else:
        SCOPE_RANGES = SCOPE_RANGES[:SCOPE_COUNT]


def refresh_scopes():
    for scope in SCOPE_MODULES:
        scope.refresh()


class ScopeModule(QtGui.QTabWidget):  # or QtGui.QGroupBox or QTabWidget
    def __init__(self):
        super(ScopeModule, self).__init__()
        # scope title
        self.index = None
        self.title = None
        # bottom axis ticks
        self.time_ticks = [[(-t, t) for t in range(SAMPLE_DURATION + 1)]]
        self.time_ticks[0][0] = (0, '0  ')
        # various object handles
        self.plot_widget = pyqtgraph.PlotWidget(name='MELScope')
        # http://www.pyqtgraph.org/documentation/graphicsItems/axisitem.html
        self.axis_left = self.plot_widget.getAxis('left')
        # http://www.pyqtgraph.org/documentation/graphicsItems/axisitem.html
        self.axis_bottom = self.plot_widget.getAxis('bottom')
        # http://www.pyqtgraph.org/documentation/graphicsItems/plotitem.html?highlight=plot%20item#pyqtgraph.PlotItem.plot
        self.plot_item = self.plot_widget.getPlotItem()
        # http://www.pyqtgraph.org/documentation/graphicsItems/viewbox.html#pyqtgraph.ViewBox
        self.view_box = self.plot_item.getViewBox()
        # configure plot settings
        self.plot_widget.setRange(xRange=[-SAMPLE_DURATION, 0], yRange=[-1, 1])
        self.plot_widget.setLimits(xMin=-SAMPLE_DURATION, xMax=0)
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setDownsampling(mode='peak')
        self.axis_bottom.setTicks(self.time_ticks)
        self.axis_bottom.setStyle(tickTextWidth=100)
        # self.plot_item.setMenuEnabled(False)
        self.axis_bottom.setZValue(0)
        self.axis_left.setZValue(0)
        # scope module curves and data
        self.curves = {}
        # scope title label
        self.label = QtGui.QLabel()
        self.label.mouseDoubleClickEvent = self.rename_scope
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setFont(bold_font)
        # scope module layout
        self.setTabPosition(2)
        self.scope_tab = QtGui.QWidget()
        self.layout = QtGui.QGridLayout()
        self.scope_tab.setLayout(self.layout)
        self.layout.addWidget(self.label, 0, 0, 1, 1)
        self.layout.addWidget(self.plot_widget, 1, 0, 1, 1)
        self.addTab(self.scope_tab, ' Scope ')
        # IO Tab Setup
        self.io_tab = QtGui.QWidget()
        self.io_layout = QtGui.QGridLayout()
        self.io_layout.setAlignment(QtCore.Qt.AlignTop)
        self.io_label = QtGui.QLabel()
        self.io_label.mouseDoubleClickEvent = self.rename_io
        self.io_label.setAlignment(QtCore.Qt.AlignCenter)
        self.io_label.setFont(bold_font)
        self.io_layout.addWidget(self.io_label, 0, 0, 1, 3)
        self.io_tab.setLayout(self.io_layout)
        self.addTab(self.io_tab, ' I/O ')

        self.io_names = {}
        self.io_values = {}
        self.io_sliders = {}
        # filter
        self.filter = {}
        # on tab changed
        self.currentChanged.connect(self.on_tab_changed)

    def on_tab_changed(self):
        global SCOPE_MODES
        SCOPE_MODES[self.index] = SCOPE_MODE_OPTIONS[self.currentIndex()]

    def refresh(self):
        # set current tab
        self.setCurrentIndex(SCOPE_MODE_OPTIONS.index(SCOPE_MODES[self.index]))
        # set scope range
        self.plot_widget.setRange(yRange=SCOPE_RANGES[self.index])
        # add/remove/refresh legend
        if self.plot_item.legend is not None:
            if self.plot_item.legend.scene() is not None:
                self.plot_item.legend.scene().removeItem(self.plot_item.legend)
        if SCOPE_LEGENDS[self.index]:
            self.plot_item.addLegend()
        # set title
        self.title = SCOPE_TITLES[self.index]
        self.label.setText(self.title)
        self.io_label.setText(self.title)
        # set filter
        self.filter = SCOPE_FILTERS[self.index]
        # clear plot
        self.plot_widget.clear()
        # apply theme settings
        self.view_box.setBackgroundColor(THEME_SCOPE_VB_COLORS[theme])
        self.plot_widget.setBackground(THEME_SCOPE_BG_COLORS[theme])
        self.axis_left.setPen(THEME_SCOPE_FB_COLORS[theme])
        self.axis_bottom.setPen(THEME_SCOPE_FB_COLORS[theme])
        if theme == 'Classic':
            color = QtGui.QColor(240, 240, 240)
            self.setStyleSheet(' background-color: %s' % color.name())
        else:
            self.setStyleSheet('')
        # remove IO items
        for key in self.io_names:
            for name in self.io_names[key]:
                if name is not None:
                    self.io_layout.removeWidget(name)
                    name.deleteLater()
        for key in self.io_values:
            for spinbox in self.io_values[key]:
                if spinbox is not None:
                    self.io_layout.removeWidget(spinbox)
                    spinbox.deleteLater()
        for key in self.io_sliders:
            for slider in self.io_sliders[key]:
                if slider is not None:
                    self.io_layout.removeWidget(slider)
                    slider.deleteLater()
        # rebuild
        self.curves = {}
        self.io_names, self.io_values, self.io_sliders = {}, {}, {}
        row = 1
        for name in MELSHARE_NAMES:
            self.curves[name] = []
            self.io_names[name] = []
            self.io_values[name] = []
            self.io_sliders[name] = []
            for i in range(MELSHARE_SIZES[name]):
                if self.filter[name][i]:
                    # update curve
                    new_pen = pyqtgraph.mkPen(CURVE_COLORS[name][i], style=CURVE_LINE_OPTIONS[
                                              CURVE_LINES[name][i]], width=CURVE_SIZES[name][i])
                    new_curve = pyqtgraph.PlotCurveItem(
                        pen=new_pen, name=DATA_NAMES[name][i])
                    self.plot_widget.addItem(new_curve)
                    self.curves[name].append(new_curve)
                    # update IO
                    new_name = QtGui.QLabel(self)
                    new_name.setText(DATA_NAMES[name][i])
                    new_name.mouseDoubleClickEvent = self.rename_data_factory(name, i)
                    new_value = QtGui.QLineEdit(self)
                    new_value.setText('0.00')
                    new_value.setValidator(QtGui.QDoubleValidator())
                    if MELSHARE_MODES[name] == 'Read Only':
                        new_value.setReadOnly(True)
                    elif MELSHARE_MODES[name] == 'Write Only':
                        rw_color = QtGui.QColor(THEME_SCOPE_RW_COLORS[theme][0], THEME_SCOPE_RW_COLORS[
                                                theme][1], THEME_SCOPE_RW_COLORS[theme][2])
                        new_value.setStyleSheet(
                            ' background-color: %s' % rw_color.name())
                        new_value.editingFinished.connect(
                            self.write_data_factory(name, i, new_value))
                    new_value.setAlignment(QtCore.Qt.AlignCenter)
                    new_slider = QtGui.QSlider(self)
                    new_slider.setOrientation(QtCore.Qt.Horizontal)
                    self.io_layout.addWidget(new_name, row, 0)
                    self.io_layout.addWidget(new_value, row, 1)
                    self.io_layout.addWidget(new_slider, row, 2)
                    for i in range(3):
                        self.io_layout.setColumnStretch(i, 1)
                    self.io_names[name].append(new_name)
                    self.io_values[name].append(new_value)
                    self.io_sliders[name].append(new_slider)
                    row += 1
                else:
                    self.curves[name].append(None)
                    self.io_names[name].append(None)
                    self.io_values[name].append(None)
                    self.io_sliders[name].append(None)

    def update(self):
        for name in MELSHARE_NAMES:
            if MELSHARE_CONNECTIONS[name]:
                for curve, io_value, filter, data, value_text in zip(self.curves[name], self.io_values[name], self.filter[name], MELSHARE_SAMPLED_DATA[name], MELSHARE_SAMPLED_DATA_TEXT[name]):
                    if filter:
                        if SCOPE_MODES[self.index] == 'Scope':
                            curve.setData(MELSHARE_SAMPLED_TIMES[name], data)
                        elif SCOPE_MODES[self.index] == 'I/O':
                            io_value.setText(value_text)


    def write_data(self, name, index, line_edit):
        MELSHARE_BUFFERS[name][index] = ctypes.c_double(float(line_edit.text()))

    def write_data_factory(self, name, index, line_edit):
        return lambda: self.write_data(name, index, line_edit)

    def rename_scope(self, event=None):
        self.temp_line_edit = QtGui.QLineEdit()
        self.temp_line_edit.setText(self.title)
        self.temp_line_edit.setAlignment(QtCore.Qt.AlignCenter)
        self.temp_line_edit.setFont(bold_font)
        self.temp_line_edit.editingFinished.connect(self.confirm_scope_rename)
        self.temp_line_edit.focusOutEvent = self.confirm_scope_rename
        self.layout.addWidget(self.temp_line_edit, 0, 0, 1, 1)
        self.temp_line_edit.selectAll()
        self.temp_line_edit.setFocus()

    def confirm_scope_rename(self, event=None):
        self.title = str(self.temp_line_edit.text())
        SCOPE_TITLES[self.index] = self.title
        self.label.setText(self.title)
        self.io_label.setText(self.title)
        self.layout.removeWidget(self.temp_line_edit)
        self.temp_line_edit.deleteLater()
        self.temp_line_edit = None

    def rename_io(self, event=None):
        self.temp_line_edit = QtGui.QLineEdit()
        self.temp_line_edit.setText(self.title)
        self.temp_line_edit.setAlignment(QtCore.Qt.AlignCenter)
        self.temp_line_edit.setFont(bold_font)
        self.temp_line_edit.editingFinished.connect(self.confirm_io_rename)
        self.temp_line_edit.focusOutEvent = self.confirm_io_rename
        self.io_layout.addWidget(self.temp_line_edit, 0, 0, 1, 3)
        self.temp_line_edit.selectAll()
        self.temp_line_edit.setFocus()

    def confirm_io_rename(self, event=None):
        self.title = str(self.temp_line_edit.text())
        SCOPE_TITLES[self.index] = self.title
        self.io_label.setText(self.title)
        self.label.setText(self.title)
        self.layout.removeWidget(self.temp_line_edit)
        self.temp_line_edit.deleteLater()
        self.temp_line_edit = None

    def rename_data_factory(self, name, index):
        return lambda event: self.rename_data(name, index, event)

    def rename_data(self, name, index, event=None):
        self.temp_line_edit1 = QtGui.QLineEdit()
        self.temp_line_edit1.setText(DATA_NAMES[name][index])
        self.temp_line_edit1.editingFinished.connect(
            lambda: self.confirm_data_rename(name, index))
        self.temp_line_edit1.focusOutEvent = lambda event: self.confirm_data_rename(
            name, index, event)
        self.io_layout.addWidget(self.temp_line_edit1, index + 1, 0)
        self.temp_line_edit1.selectAll()
        self.temp_line_edit1.setFocus()

    def confirm_data_rename(self, name, index, event=None):
        DATA_NAMES[name][index] = str(self.temp_line_edit1.text())
        self.io_names[name][index].setText(DATA_NAMES[name][index])
        self.io_layout.removeWidget(self.temp_line_edit1)
        self.temp_line_edit1.deleteLater()
        self.temp_line_edit1 = None
        refresh_scopes()


#==============================================================================
# GRID VARIABLES / FUNCTIONS
#==============================================================================

MAX_GRID_ROWS = 5
MAX_GRID_COLS = 5

GRID_SIZE_OPTIONS = [(r + 1, c + 1) for r in range(MAX_GRID_ROWS)
                for c in range(MAX_GRID_COLS)]
GRID_SIZE_OPTIONS_DISPLAY = [str(x[0]) + ' x ' + str(x[1]) for x in GRID_SIZE_OPTIONS]
GRID_SIZE_OPTIONS_DICT = dict(zip(GRID_SIZE_OPTIONS_DISPLAY, GRID_SIZE_OPTIONS))

GRID_ROWS = 1
GRID_COLS = 1

def resize_grid():
    global main_window, widg, grid, GRID_ROWS, GRID_COLS, SCOPE_COUNT, splitter_widgets
    new_count = GRID_ROWS * GRID_COLS
    # remove scopes from grid
    for i in range(SCOPE_COUNT):
        grid.removeWidget(SCOPE_MODULES[i])
    # reset the grid (???)
    widg.deleteLater()
    grid.deleteLater()
    widg, grid = None, None
    widg = QtGui.QWidget()
    main_window.setCentralWidget(widg)
    main_window.resize(640, 640)
    widg.setContentsMargins(12, 12, 12, 12)
    grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
    grid.setContentsMargins(0, 0, 0, 0)
    widg.setLayout(grid)
    # add new scopes
    if new_count > SCOPE_COUNT:
        add = new_count - SCOPE_COUNT
        for i in range(add):
            SCOPE_MODULES.append(ScopeModule())
    # or remove old scopes
    elif new_count < SCOPE_COUNT:
        remove = SCOPE_COUNT - new_count
        for i in range(remove):
            SCOPE_MODULES.pop().deleteLater()
    # readd scopes to grid
    positions = [(r, c) for r in range(GRID_ROWS)
                 for c in range(GRID_COLS)]
    for i in range(new_count):
        SCOPE_MODULES[i].index = i
        grid.addWidget(SCOPE_MODULES[i], positions[
                       i][0], positions[i][1], 1, 1)
    # set row/column stretch to ensure same sizes (!!! THIS HAS ISSUES)
    for i in range(GRID_ROWS):
        grid.setRowStretch(i, 1)
    for i in range(GRID_COLS):
        grid.setColumnStretch(i, 1)
    # update scope count
    SCOPE_COUNT = new_count


#==============================================================================
# OPEN/SAVE VARIABLES / FUNCTIONS
#==============================================================================

FILEPATH = None

def open_new_instance():
    CREATE_NO_WINDOW = 0x08000000
    subprocess.Popen([sys.executable, 'MELScope.pyw'],
                     creationflags=CREATE_NO_WINDOW)
# https://stackoverflow.com/questions/89228/calling-an-external-command-in-python#2251026
# https://msdn.microsoft.com/en-us/library/windows/desktop/ms684863(v=vs.85).aspx


def open():
    global FILEPATH
    FILEPATH = QtGui.QFileDialog.getOpenFileName(
        widg, 'Open MELScope', "", 'Scope Files (*.scope *.yaml)')
    if FILEPATH:
        stream = file(FILEPATH, 'r')
        config = yaml.load(stream)
        remove_all_melshares()
        deploy_config(config)
        resize_grid()
        set_theme()
        for name in MELSHARE_NAMES:
            reconnect_melshare(name)
            if MELSHARE_CONNECTIONS[name]:
                reset_melshare_data(name)
        filename = str(FILEPATH[str(FILEPATH).rfind('/') + 1:])
        filename = filename[0: filename.rfind('.')]
        status_bar.showMessage('Opened <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')


def save():
    global FILEPATH
    if FILEPATH:
        config = generate_config()
        stream = file(FILEPATH, 'w')
        yaml.dump(config, stream)
        filename = str(FILEPATH[str(FILEPATH).rfind('/') + 1:])
        filename = filename[0: filename.rfind('.')]
        status_bar.showMessage('Saved <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')
    else:
        save_as()


def save_as():
    global FILEPATH
    new_filepath = QtGui.QFileDialog.getSaveFileName(
        widg, 'Save MELScope', "", 'Scope Files (*.scope *.yaml)')
    if new_filepath:
        FILEPATH = new_filepath
        save()


def deploy_config(config):
    global theme, MELSHARE_NAMES, MELSHARE_MODES, MELSHARE_SIZES
    global DATA_NAMES, CURVE_SIZES, CURVE_COLORS, CURVE_LINES
    global GRID_ROWS, GRID_COLS
    global SCOPE_TITLES, SCOPE_MODES, SCOPE_FILTERS, SCOPE_LEGENDS, SCOPE_RANGES
    theme = config['theme']
    MELSHARE_NAMES = config['MELSHARE_NAMES']
    MELSHARE_MODES = config['MELSHARE_MODES']
    MELSHARE_SIZES = config['MELSHARE_SIZES']
    DATA_NAMES = config['DATA_NAMES']
    CURVE_SIZES = config['CURVE_SIZES']
    CURVE_COLORS = config['CURVE_COLORS']
    CURVE_LINES = config['CURVE_LINES']
    GRID_ROWS = config['GRID_ROWS']
    GRID_COLS = config['GRID_COLS']
    SCOPE_TITLES = config['SCOPE_TITLES']
    SCOPE_MODES = config['SCOPE_MODES']
    SCOPE_FILTERS = config['SCOPE_FILTERS']
    SCOPE_LEGENDS = config['SCOPE_LEGENDS']
    SCOPE_RANGES = config['SCOPE_RANGES']


def generate_config():
    config = {
        'theme': theme,
        'MELSHARE_NAMES': MELSHARE_NAMES,
        'MELSHARE_MODES': MELSHARE_MODES,
        'MELSHARE_SIZES': MELSHARE_SIZES,
        'DATA_NAMES': DATA_NAMES,
        'CURVE_SIZES': CURVE_SIZES,
        'CURVE_COLORS': CURVE_COLORS,
        'CURVE_LINES': CURVE_LINES,
        'GRID_ROWS': GRID_ROWS,
        'GRID_COLS': GRID_COLS,
        'SCOPE_TITLES': SCOPE_TITLES,
        'SCOPE_MODES': SCOPE_MODES,
        'SCOPE_FILTERS': SCOPE_FILTERS,
        'SCOPE_LEGENDS': SCOPE_LEGENDS,
        'SCOPE_RANGES': SCOPE_RANGES}
    return config


#==============================================================================
# DIALOG CLASSES
#==============================================================================

class AddMelShareDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(AddMelShareDialog, self).__init__(parent)
        self.setWindowTitle('Add MELShare')
        self.layout = QtGui.QGridLayout(self)

        self.name_label = QtGui.QLabel(self)
        self.name_label.setText("MELShare Name")
        self.name_label.setFont(bold_font)
        self.layout.addWidget(self.name_label, 0, 0)

        self.mode_label = QtGui.QLabel(self)
        self.mode_label.setText("Mode")
        self.mode_label.setFont(bold_font)
        self.layout.addWidget(self.mode_label, 0, 1)

        self.name_line_edit = QtGui.QLineEdit(self)
        self.layout.addWidget(self.name_line_edit, 1, 0)

        self.mode_combo_box = QtGui.QComboBox(self)
        self.mode_combo_box.addItems(MELSHARE_MODES_OPTIONS)
        self.layout.addWidget(self.mode_combo_box)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.layout.addWidget(self.buttons, 2, 0, 1, 2)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    @staticmethod
    def get_input(parent=None):
        dialog = AddMelShareDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            melshare_name = str(dialog.name_line_edit.text())
            melshare_mode = str(dialog.mode_combo_box.currentText())
            return (melshare_name, melshare_mode, True)
        else:
            return (None, None, False)




class ConfigureDataDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigureDataDialog, self).__init__(parent)

        # title and layout
        self.setWindowTitle('Configure Data')
        layout = QtGui.QGridLayout(self)

        # top row
        index_label = QtGui.QLabel(self)
        index_label.setText('Data')
        index_label.setFont(bold_font)
        layout.addWidget(index_label, 0, 0)

        name_label = QtGui.QLabel(self)
        name_label.setText('Name')
        name_label.setFont(bold_font)
        layout.addWidget(name_label, 0, 1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(bold_font)
        layout.addWidget(mode_label, 0, 2)

        size_label = QtGui.QLabel(self)
        size_label.setText('Weight')
        size_label.setFont(bold_font)
        layout.addWidget(size_label, 0, 3)

        color_label = QtGui.QLabel(self)
        color_label.setText('Color')
        color_label.setFont(bold_font)
        color_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(color_label, 0, 4)

        line_label = QtGui.QLabel(self)
        line_label.setText('Line Style')
        line_label.setFont(bold_font)
        layout.addWidget(line_label, 0, 5)

        # storage containers for GUI elements
        self.name_line_edits = {}
        self.mode_labels = {}
        self.size_spin_boxes = {}
        self.color_buttons = {}
        self.line_combo_boxes = {}

        row = 1
        for name in MELSHARE_NAMES:
            self.name_line_edits[name] = []
            self.mode_labels[name] = []
            self.size_spin_boxes[name] = []
            self.color_buttons[name] = []
            self.line_combo_boxes[name] = []
            for i in range(MELSHARE_SIZES[name]):
                row += 1
                new_index_label = QtGui.QLabel(self)
                new_index_label.setText(name + '[' + str(i) + ']')
                new_index_label.setFont(bold_font)
                layout.addWidget(new_index_label, row, 0)

                new_line_edit = QtGui.QLineEdit(self)
                new_line_edit.setText(DATA_NAMES[name][i])
                self.name_line_edits[name].append(new_line_edit)
                layout.addWidget(new_line_edit, row, 1)

                new_mode_label = QtGui.QLabel(self)
                new_mode_label.setText(MELSHARE_MODES[name])
                self.mode_labels[name].append(new_mode_label)
                layout.addWidget(new_mode_label, row, 2)

                new_spin_box = QtGui.QSpinBox(self)
                new_spin_box.setValue(int(CURVE_SIZES[name][i]))
                new_spin_box.setAlignment(QtCore.Qt.AlignCenter)
                new_spin_box.setRange(1, 10)
                self.size_spin_boxes[name].append(new_spin_box)
                layout.addWidget(new_spin_box, row, 3)

                c = CURVE_COLORS[name][i]
                curve_color = QtGui.QColor(c[0], c[1], c[2])

                new_color_button = QtGui.QPushButton(self)
                new_color_button.setMaximumHeight(15 * resolution_scale)
                new_color_button.setMaximumWidth(30 * resolution_scale)
                new_color_button.setStyleSheet(
                    ' background-color: %s' % curve_color.name())
                new_color_button.clicked.connect(
                    self.prompt_color_dialog_factory(name, i))
                self.color_buttons[name].append(new_color_button)
                layout.addWidget(new_color_button, row, 4)

                new_line_combo_box = QtGui.QComboBox(self)
                new_line_combo_box.addItems(CURVE_LINE_OPTIONS.keys())
                new_line_combo_box.setCurrentIndex(
                    CURVE_LINE_OPTIONS.keys().index(CURVE_LINES[name][i]))
                self.line_combo_boxes[name].append(new_line_combo_box)
                layout.addWidget(new_line_combo_box, row, 5)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)
        self.buttons.button(QtGui.QDialogButtonBox.Apply).clicked.connect(self.apply)

        layout.addWidget(self.buttons, row+1, 0, 1, 6)


    def prompt_color_dialog_factory(self, name, index):
        return lambda: self.prompt_color_dialog(name, index)

    def prompt_color_dialog(self, name, index):
        current_color = self.color_buttons[name][
            index].palette().color(QtGui.QPalette.Background)
        selected_color = QtGui.QColorDialog.getColor(current_color, self)
        if selected_color.isValid():
            self.color_buttons[name][index].setStyleSheet(
                "background-color: %s" % selected_color.name())

    def apply(self):
        for name in MELSHARE_NAMES:
            for i in range(MELSHARE_SIZES[name]):
                DATA_NAMES[name][i] = str(self.name_line_edits[name][i].text())
                CURVE_SIZES[name][i] = int(self.size_spin_boxes[name][i].value())
                color = self.color_buttons[name][i].palette().color(
                    QtGui.QPalette.Background)
                CURVE_COLORS[name][i] = [color.red(), color.green(), color.blue()]
                CURVE_LINES[name][i] = str(self.line_combo_boxes[name][i].currentText())
            refresh_scopes()

    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def open_dialog(parent=None):
        dialog = ConfigureDataDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            dialog.apply()


class ConfigureModulesDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigureModulesDialog, self).__init__(parent)
        self.setWindowTitle('Configure Module(s)')
        layout = QtGui.QGridLayout(self)

        c = THEME_SCOPE_FB_COLORS[theme]
        border_color = QtGui.QColor(c[0], c[1], c[2])

        x = 0
        for name in MELSHARE_NAMES:
            filter_label = QtGui.QLabel(self)
            filter_label.setText(name)
            filter_label.setFont(bold_font)
            filter_label.setAlignment(QtCore.Qt.AlignCenter)
            filter_label.setStyleSheet("border: 1px solid %s" % border_color.name());
            span = 1
            if MELSHARE_SIZES[name] > 0:
                span = MELSHARE_SIZES[name]
            layout.addWidget(filter_label, 0, 6 + x, 1, span)
            x += span

        scope_label = QtGui.QLabel(self)
        scope_label.setText('Scope Options')
        scope_label.setFont(bold_font)
        scope_label.setAlignment(QtCore.Qt.AlignCenter)
        scope_label.setStyleSheet("border: 1px solid %s" % border_color.name());
        layout.addWidget(scope_label, 0, 3, 1, 3)

        index_label = QtGui.QLabel(self)
        index_label.setText('i')
        index_label.setFont(bold_font)
        index_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(index_label, 1, 0)

        title_label = QtGui.QLabel(self)
        title_label.setText('Module Name')
        title_label.setFont(bold_font)
        layout.addWidget(title_label, 1, 1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(bold_font)
        layout.addWidget(mode_label, 1, 2)

        legend_label = QtGui.QLabel(self)
        legend_label.setText('Legend')
        legend_label.setFont(bold_font)
        layout.addWidget(legend_label, 1, 3)

        min_label = QtGui.QLabel(self)
        min_label.setText('yMin')
        min_label.setFont(bold_font)
        min_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(min_label, 1, 4)

        max_label = QtGui.QLabel(self)
        max_label.setText('yMax')
        max_label.setFont(bold_font)
        max_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(max_label, 1, 5)

        self.title_line_edits = []
        self.mode_combo_boxes = []
        self.legend_checkboxes = []
        self.min_line_edits = []
        self.max_line_edits = []
        self.filter_checkboxes = []

        x = 0
        for name in MELSHARE_NAMES:
            for i in range(MELSHARE_SIZES[name]):
                c = CURVE_COLORS[name][i]
                curve_color = QtGui.QColor(c[0], c[1], c[2])
                new_curve_label = QtGui.QLabel(self)

                curve_label_font = QtGui.QFont()
                curve_label_font.setBold(True)
                curve_label_font.setPointSize(8)

                new_curve_label.setText(str(i))
                new_curve_label.setFont(curve_label_font)
                new_curve_label.setStyleSheet(
                    ' color: white; background-color: %s' % curve_color.name())

                new_curve_label.setAlignment(QtCore.Qt.AlignCenter)
                layout.addWidget(new_curve_label, 1, x + i + 6)
            x += MELSHARE_SIZES[name]

        for i in range(SCOPE_COUNT):

            new_index_label = QtGui.QLabel(self)
            new_index_label.setText(str(i))
            new_index_label.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_index_label, i + 2, 0)

            new_line_edit = QtGui.QLineEdit(self)
            new_line_edit.setText(SCOPE_TITLES[i])
            new_line_edit.setMinimumWidth(150 * resolution_scale)
            self.title_line_edits.append(new_line_edit)
            layout.addWidget(new_line_edit, i + 2, 1)

            new_mode_combo_box = QtGui.QComboBox(self)
            new_mode_combo_box.addItems(SCOPE_MODE_OPTIONS)
            new_mode_combo_box.setCurrentIndex(
                SCOPE_MODE_OPTIONS.index(SCOPE_MODES[i]))
            self.mode_combo_boxes.append(new_mode_combo_box)
            layout.addWidget(new_mode_combo_box, i + 2, 2)

            new_legend_checkbox = QtGui.QCheckBox(self)
            new_legend_checkbox.setChecked(SCOPE_LEGENDS[i])
            new_legend_checkbox.setFixedWidth(25 * resolution_scale)
            self.legend_checkboxes.append(new_legend_checkbox)
            layout.addWidget(new_legend_checkbox, i + 2, 3)

            new_min_line_edit = QtGui.QLineEdit(self)
            new_min_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_min_line_edit.setText(str(SCOPE_RANGES[i][0]))
            new_min_line_edit.setFixedWidth(50 * resolution_scale)
            self.min_line_edits.append(new_min_line_edit)
            layout.addWidget(new_min_line_edit, i + 2, 4)

            new_max_line_edit = QtGui.QLineEdit(self)
            new_max_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_max_line_edit.setText(str(SCOPE_RANGES[i][1]))
            new_max_line_edit.setFixedWidth(50 * resolution_scale)
            self.max_line_edits.append(new_max_line_edit)
            layout.addWidget(new_max_line_edit, i + 2, 5)


            self.filter_checkboxes.append({})

            x = 0
            for name in MELSHARE_NAMES:
                self.filter_checkboxes[i][name] = []
                for j in range(MELSHARE_SIZES[name]):
                    new_check_box = QtGui.QCheckBox(self)
                    new_check_box.setChecked(SCOPE_FILTERS[i][name][j])
                    self.filter_checkboxes[i][name].append(new_check_box)
                    layout.addWidget(new_check_box, i + 2, x + j + 6)
                x += MELSHARE_SIZES[name]

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)
        self.buttons.button(QtGui.QDialogButtonBox.Apply).clicked.connect(self.apply)
        layout.addWidget(self.buttons, len(SCOPE_MODULES) + 2, 0, 1, layout.columnCount())

    def apply(self):
        global SCOPE_TITLES, SCOPE_MODES, SCOPE_FILTERS, SCOPE_LEGENDS, SCOPE_RANGES
        for i in range(SCOPE_COUNT):
            for name in MELSHARE_NAMES:
                for j in range(MELSHARE_SIZES[name]):
                    SCOPE_FILTERS[i][name][j] = self.filter_checkboxes[i][name][j].isChecked()
        SCOPE_TITLES = [str(i.text()) for i in self.title_line_edits]
        SCOPE_MODES = [str(i.currentText())
                       for i in self.mode_combo_boxes]
        SCOPE_LEGENDS = [i.isChecked() for i in self.legend_checkboxes]
        SCOPE_RANGES = [list(pair) for pair in
                        zip([float(i.text()) for i in self.min_line_edits],
                            [float(i.text()) for i in self.max_line_edits])]
        refresh_scopes()

    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def open_dialog(parent=None):
        dialog = ConfigureModulesDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            dialog.apply()

class AboutDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(AboutDialog, self).__init__(parent)
        self.setWindowTitle('About')
        layout = QtGui.QGridLayout(self)

        title = QtGui.QLabel(self)
        title.setText("MELScope v" + ver)
        title_font = QtGui.QFont()
        title_font.setBold(True)
        title_font.setPointSize(12)
        title.setFont(title_font)
        title.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(title, 0,0,1,2)

        lab = QtGui.QLabel(self)
        lab.setText("MAHI Lab | Rice University")
        lab.setFont(bold_font)
        lab.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(lab, 1,0,1,2)

        author_lh = QtGui.QLabel(self)
        author_lh.setText("Author:")
        author_lh.setFont(bold_font)
        layout.addWidget(author_lh, 2, 0)

        author_rh = QtGui.QLabel(self)
        author_rh.setText("Evan Pezent")
        layout.addWidget(author_rh, 2, 1)

        email_lh = QtGui.QLabel(self)
        email_lh.setText("Email:")
        email_lh.setFont(bold_font)
        layout.addWidget(email_lh, 3, 0)

        email_rh = QtGui.QLabel(self)
        email_rh.setText("<a href='mailto:someone@somewhere.com'>epezent@rice.edu</a>")
        email_rh.setOpenExternalLinks(True)
        layout.addWidget(email_rh, 3, 1)

        website_lh = QtGui.QLabel(self)
        website_lh.setText("Website:")
        website_lh.setFont(bold_font)
        layout.addWidget(website_lh, 4, 0)

        website_rh = QtGui.QLabel(self)
        website_rh.setText("www.evanpezent.com")
        website_rh.setText("<a href=\"http://evanpezent.com/\">evanpezent.com</a>");
        website_rh.setOpenExternalLinks(True)
        layout.addWidget(website_rh, 4, 1)

    @staticmethod
    def open_dialog(parent=None):
        dialog = AboutDialog(parent)
        dialog.exec_()



#==============================================================================
# ACTION FUNCTIONS
#==============================================================================

def prompt_configure_data():
    if melshares_connected():
        ConfigureDataDialog.open_dialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a MELShare before configuring!')
        msgBox.setWindowTitle('Configure Data')
        msgBox.exec_()


def prompt_configure_modules():
    ConfigureModulesDialog.open_dialog()



def prompt_add_melshare():
    melshare_name, melshare_mode, ok = AddMelShareDialog.get_input()
    if ok:
        add_melshare(melshare_name, melshare_mode)
        reload_all()

def prompt_remove_melshare():
    selection, ok = QtGui.QInputDialog.getItem(widg, 'Remove MELShare', 'Select MELShare:', MELSHARE_NAMES,0,False)
    if ok:
        remove_melshare(str(selection))
        reload_all()

def prompt_resize_grid():
    global GRID_ROWS, GRID_COLS, SCOPE_COUNT
    i = GRID_SIZE_OPTIONS.index((GRID_ROWS, GRID_COLS))
    selection, ok = QtGui.QInputDialog.getItem(
        widg, 'Grid Size', 'Select Grid Size:', GRID_SIZE_OPTIONS_DISPLAY, i, False)
    if ok:
        GRID_ROWS, GRID_COLS = GRID_SIZE_OPTIONS_DICT[str(selection)]
        reload_grid()
        status_bar.showMessage('Scope grid resized to ' + str(GRID_ROWS) + ' x ' + str(GRID_COLS))


def about():
    AboutDialog.open_dialog()


def open_github():
    webbrowser.open('https://github.com/epezent/MEL')


def prompt_change_theme():
    global theme, THEME_OPTIONS
    selection, ok = QtGui.QInputDialog.getItem(
        widg, 'Theme', 'Select Theme:', THEME_OPTIONS, THEME_OPTIONS.index(theme), False)
    if ok:
        theme = str(selection)
        set_theme()


def reload_all():
    for name in MELSHARE_NAMES:
        reconnect_melshare(name)
        reset_melshare_data(name)
        validate_curve_properties(name)
    resize_grid()
    validate_scope_properties()
    refresh_scopes()


def reload_grid():
    resize_grid()
    validate_scope_properties()
    refresh_scopes()

PAUSED = False

def switch_pause():
    global PAUSED
    PAUSED =  not PAUSED
    if PAUSED:
        pause_label.setText("Paused")
        pause_action.setText('R&esume')
    else:
        pause_label.setText("")
        pause_action.setText("&Pause")


#==============================================================================
# MENU BAR SETUP
#==============================================================================

menu_bar = main_window.menuBar()

file_menu = menu_bar.addMenu('&File')
edit_menu = menu_bar.addMenu('&Edit')
action_menu = menu_bar.addMenu('&Actions')
pref_menu = menu_bar.addMenu('&Preferences')
help_menu = menu_bar.addMenu('&Help')

new_action = QtGui.QAction('&New', main_window,
                           shortcut='Ctrl+N', statusTip='Create a MELScope', triggered=open_new_instance)
file_menu.addAction(new_action)

open_action = QtGui.QAction('&Open...', main_window,
                            shortcut='Ctrl+O', statusTip='Open an existing MELScope', triggered=open)
file_menu.addAction(open_action)

save_action = QtGui.QAction('&Save', main_window,
                            shortcut='Ctrl+S', statusTip='Save this MELScope', triggered=save)
file_menu.addAction(save_action)

save_as_action = QtGui.QAction('Save &As...', main_window,
                               shortcut='Ctrl+Shift+S', statusTip='Save this MELScope under a new name', triggered=save_as)
file_menu.addAction(save_as_action)

reload_action = QtGui.QAction('&Reload', main_window,
                              shortcut='R', statusTip='Reload the MELScope', triggered=reload_all)
action_menu.addAction(reload_action)

pause_action = QtGui.QAction('&Pause', main_window,
    shortcut='P',statusTip='Pause the MELScope', triggered=switch_pause)
action_menu.addAction(pause_action)

add_action = QtGui.QAction('&Add MELShare...', main_window,
                               shortcut='Ctrl+A', statusTip='Add a new MELShare map', triggered=prompt_add_melshare)
edit_menu.addAction(add_action)

remove_action = QtGui.QAction('&Remove MELShare...', main_window,
                               shortcut='Ctrl+X', statusTip='Remove an existing MELShare map', triggered=prompt_remove_melshare)
edit_menu.addAction(remove_action)

grid_action = QtGui.QAction('Resize &Grid...', main_window,
                            shortcut='Ctrl+G', statusTip='Resize the scope grid', triggered=prompt_resize_grid)
edit_menu.addAction(grid_action)

config_curves_action = QtGui.QAction('Configure &Data...', main_window,
                                     shortcut='Ctrl+D', statusTip='Configure data and curve properties', triggered=prompt_configure_data)
edit_menu.addAction(config_curves_action)

configure_modules_action = QtGui.QAction('&Configure Modules(s)...', main_window,
                                         shortcut='Ctrl+C', statusTip='Configure scope module(s) properties', triggered=prompt_configure_modules)
edit_menu.addAction(configure_modules_action)

theme_action = QtGui.QAction('&Theme...', main_window,
                             shortcut='F10', statusTip='Change the current theme', triggered=prompt_change_theme)
pref_menu.addAction(theme_action)

about_action = QtGui.QAction('&About', main_window,
                             shortcut='F11', statusTip='About MELScope', triggered=about)
help_menu.addAction(about_action)

github_action = QtGui.QAction('&GitHub...', main_window,
                              shortcut='F12', statusTip='Open MEL GitHub webpage', triggered=open_github)
help_menu.addAction(github_action)


def update_menu():
    global config_curves_action, configure_modules_action
    if len(MELSHARE_NAMES) == 0:
        remove_action.setDisabled(True)
    else:
        remove_action.setDisabled(False)
    if melshares_connected():
        config_curves_action.setDisabled(False)
        #configure_modules_action.setDisabled(False)
    else:
        config_curves_action.setDisabled(True)
        #configure_modules_action.setDisabled(True)

#==============================================================================
# MAIN APPLICATION EXECUTION / LOOP
#==============================================================================

set_theme()
reload_grid()
status_bar.showMessage('Welcome to MELScope!')

def sample_loop():
    update_sample_time()
    update_sample_rate()
    lost = []
    for name in MELSHARE_NAMES:
        if MELSHARE_CONNECTIONS[name]:
            if read_melshare(name) > 0:
                save_melshare_data(name)
            else:
                lost.append(name)
                MELSHARE_CONNECTIONS[name] = False;
    if lost:
        msg = 'Lost connection to MELShare(s): '
        for name in lost:
            msg += '<' + name + '>, '
        msg = msg[:-2]
        status_bar.showMessage(msg)

def render_loop():
    update_render_time()
    update_FPS()
    update_menu()
    if not PAUSED:
        for scope in SCOPE_MODULES:
            scope.update()

# start the sample_loop_timer
sample_loop_timer  = QtCore .QTimer()
sample_loop_timer.timeout.connect(sample_loop)
sample_loop_timer.start(1000 / SAMPLE_TARGET)

# start the render_loop_timer
render_loop_timer = QtCore.QTimer()
render_loop_timer.timeout.connect(render_loop)
render_loop_timer.start(1000 / FPS_TARGET)

# show the main window
main_window.show()

# start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
    # multiprocessing.freeze_support()
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

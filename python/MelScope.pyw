#==============================================================================
# MEL Scope
# Evan Pezent | epezent@rice.edu | evanpezent.com
# Mechatronics and Haptic Interfaces (MAHI) Lab
# Rice University
# 06/2017 -
#==============================================================================

from MelShare import MelShare
from pyqtgraph.Qt import QtGui, QtCore
from pyqtgraph.ptime import time
import pyqtgraph.widgets.RemoteGraphicsView
import numpy as np
import pyqtgraph
import webbrowser
import subprocess
import sys
import yaml
import collections
import qdarkstyle
import ctypes

# TO DO:
# turn scopes on off
# thread melshare
# save window size
# curve scaling (eg deg2rad)
# scope modules to generic modules, add interactive gain modules with sliders
# open from .scope
# changing colros when paused deletes data
# add MelNet
# write only shouldn't plot
# write curves do weird things on plot -- should be hidden or fixed
# settigns tab on each scope

#==============================================================================
# DEFAULT SETTINGS
#==============================================================================

# The default theme ('Classic', 'Dark')
DEFAULT_THEME = 'Dark'
# The default width of each new grid box
DEFAULT_WIDTH = 400
# The default height of each new grid box
DEFAULT_HEIGHT = 400
# The maximum allowable grid rows
MAX_GRID_ROWS = 4
# The maximum allowable grid columns
MAX_GRID_COLS = 4
# The sample history length in seconds
SAMPLE_DURATION = 10
# The target plot rendering rate (FPS)
FPS_TARGET = 60
# The target sampling rate (Hz)
SAMPLE_TARGET = 100
# The default time axis mode ('Fixed', 'Rolling')
DEFAULT_TIME_MODE = 'Fixed'
# The default curve style ('Solid', 'Dash', 'Dot', 'Dash Dot', 'Dash Dot Dot')
DEFAULT_CURVE_STYLE = 'Solid'
# The default curve width
DEFAULT_CURVE_WIDTH = 2

#==============================================================================
# INITIALIZATION
#==============================================================================

myappid = 'MELScope'
ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
application = QtGui.QApplication([])
application.setWindowIcon(QtGui.QIcon("melscope_icon.png"))

main_window = QtGui.QMainWindow()
main_window.setWindowTitle('MEL Scope (untitled)')

main_widget = QtGui.QWidget()
main_window.setCentralWidget(main_widget)
main_widget.setContentsMargins(12, 12, 12, 12)

grid = QtGui.QGridLayout()
grid.setContentsMargins(0, 0, 0, 0)
main_widget.setLayout(grid)

theme = DEFAULT_THEME
time_mode = DEFAULT_TIME_MODE

#==============================================================================
# GLOBAL CONSTANTS
#==============================================================================

VER = '0.2.1'

SCREEN_RESOLUTION = application.desktop().screenGeometry()
RESOLUTION_SCALE = SCREEN_RESOLUTION.width() / 1920.0

DEFAULT_WIDTH  *= RESOLUTION_SCALE
DEFAULT_HEIGHT *= RESOLUTION_SCALE

BOLD_FONT = QtGui.QFont()
BOLD_FONT.setBold(True)

SCROLL_OPTIONS = ['Fixed', 'Rolling']

NUM_SAMPLES = SAMPLE_DURATION * SAMPLE_TARGET

THEME_OPTIONS = ['Classic', 'Dark']
THEME_STYLESHEETS = {'Classic': '', 'Dark': qdarkstyle.load_stylesheet(pyside=False)}
THEME_SCOPE_BG_COLORS = {"Classic": [240, 240, 240], "Dark": [49, 54, 59]}
THEME_SCOPE_FB_COLORS = {"Classic": [0, 0, 0], "Dark": [240, 240, 240]}
THEME_SCOPE_VB_COLORS  = {"Classic": [240, 240, 240], "Dark": [35, 38, 41]}
THEME_SCOPE_IO_CONFIRMED_COLORS = {"Classic": [204, 232, 255], "Dark": [24, 70, 93]}
THEME_SCOPE_IO_CHANGING_COLORS  = {'Classic': [144, 200, 246], 'Dark': [191, 54, 12]}

CURVE_COLOR_OPTIONS = [[31, 120, 180], [227, 26, 28], [51, 160, 44],
                      [255, 127, 0], [106, 61, 154], [177, 89, 40],
                      [166, 206, 227], [251, 154, 153], [178, 223, 138],
                      [253, 191, 111], [202, 178, 214], [255, 255, 153],
                      [141,211,199], [255,255,179], [190,186,218],
                      [251,128,114], [128,177,211], [253,180,98],
                      [179,222,105], [252,205,229], [217,217,217],
                      [188,128,189], [204,235,197], [255,237,111]]
                      # http://colorbrewer2.org/

CURVE_STYLE_OPTIONS = collections.OrderedDict([
    ('Solid', QtCore.Qt.SolidLine),
    ('Dash', QtCore.Qt.DashLine),
    ('Dot', QtCore.Qt.DotLine),
    ('Dash Dot', QtCore.Qt.DashDotLine),
    ('Dash Dot Dot', QtCore.Qt.DashDotDotLine)])

DEFAULT_CURVE_WIDTH *= RESOLUTION_SCALE

GRID_SIZE_OPTIONS = [(r + 1, c + 1) for r in range(MAX_GRID_ROWS) for c in range(MAX_GRID_COLS)]
GRID_SIZE_OPTIONS_DISPLAY = [str(x[0]) + ' x ' + str(x[1]) for x in GRID_SIZE_OPTIONS]
GRID_SIZE_OPTIONS_DICT = dict(zip(GRID_SIZE_OPTIONS_DISPLAY, GRID_SIZE_OPTIONS))

DATA_TYPE_OPTIONS = ['MEL Share', 'MEL Net']
DATA_MODE_OPTIONS = ['Read Only', 'Write Only']

SCOPE_MODE_OPTIONS = ['Plot', 'Numeric']

#==============================================================================
# TIMING
#==============================================================================

last_time_fps = time()
time_now_fps = time()
delta_time_fps = 0

last_time_sr = time()
time_now_sr = time()
delta_time_sr = 0
elapsed_time_sr = 0

sampled_times = np.zeros(NUM_SAMPLES)

fps_actual = None
sample_rate = None

def update_render_time():
    global last_time_fps, time_now_fps, delta_time_fps
    time_now_fps = time()
    delta_time_fps = time_now_fps - last_time_fps
    last_time_fps = time_now_fps

def update_FPS():
    global delta_time_fps, fps_actual
    if fps_actual is None:
        fps_actual = 1.0 / delta_time_fps
    else:
        s = np.clip(delta_time_fps * 3., 0, 1)
        fps_actual = fps_actual * (1 - s) + (1.0 / delta_time_fps) * s
    fps_label.setText('%0.0f FPS ' % fps_actual)

def update_sample_rate():
    global delta_time_sr, sample_rate
    if sample_rate is None:
        sample_rate = 1.0 / delta_time_sr
    else:
        s = np.clip(delta_time_sr * 3., 0, 1)
        sample_rate = sample_rate * (1 - s) + (1.0 / delta_time_sr) * s
    rate_label.setText('%0.0f Hz ' % sample_rate)

def update_sample_times():
    global last_time_sr, time_now_sr, delta_time_sr, elapsed_time_sr, sampled_times
    time_now_sr = time()
    delta_time_sr = time_now_sr - last_time_sr
    elapsed_time_sr += delta_time_sr
    last_time_sr = time_now_sr
    if time_mode == 'Fixed':
        sampled_times[:-1] = sampled_times[1:] - delta_time_sr
        sampled_times[-1] = 0
    elif time_mode == 'Rolling':
        sampled_times[:-1] = sampled_times[1:]
        sampled_times[-1] = elapsed_time_sr

def reset_sample_times():
    global NUM_SAMPLES, sampled_times
    sampled_times = np.zeros(NUM_SAMPLES)
    # reset each data source

#==============================================================================
# DATA SOURCES
#==============================================================================

write_flag = False

class DataSource(object):

    def __init__(self, name, mode, curve_names, curve_colors, curve_styles, curve_widths):
        # properties
        self.name = name
        self.mode = mode
        self.curve_names  = curve_names
        self.curve_colors = curve_colors
        self.curve_styles = curve_styles
        self.curve_widths = curve_widths
        self.ms = MelShare(name)
        self.data = self.ms.read_data()
        self.size = len(self.data)
        self.text = ['%0.4f' % value for value in self.data]
        self.samples = None
        self.reset_samples()
        self.validate_curves()

    def reload(self):
        self.reset_samples()
        self.validate_curves()

    def sample(self):
        # read new data
        self.data = self.ms.read_data()
        self.size = len(self.data)
        # save valus to as text
        self.text = ['%0.4f' % value for value in self.data]
        # add new data to samples
        for (samples, i) in zip(self.samples, range(self.size)):
            samples[:-1] = samples[1:]
            samples[-1] = self.data[i]

    def reset_samples(self):
        self.samples = []
        self.data = self.ms.read_data()
        self.size = len(self.data)
        for i in range(self.size):
            self.samples.append(np.ones(NUM_SAMPLES) * self.data[i])

    def write(self):
        self.ms.write_data(self.data)

    def validate_curves(self):
        self.data = self.ms.read_data()
        self.size = len(self.data)
        # validate curve names
        if self.size > len(self.curve_names):
            for i in range(len(self.curve_names), self.size):
                self.curve_names.append(self.name + '[' + str(i) + ']')
        else:
            self.curve_names = self.curve_names[:self.size]
        # validate curve colors
        if self.size > len(self.curve_colors):
            for i in range(len(self.curve_colors), self.size):
                next_color = CURVE_COLOR_OPTIONS.pop(0)
                CURVE_COLOR_OPTIONS.append(next_color)
                self.curve_colors.append(next_color)
        else:
            self.curve_colors = self.curve_colors[:self.size]
        # validate curve styles
        if self.size > len(self.curve_styles):
            for i in range(len(self.curve_styles), self.size):
                self.curve_styles.append(DEFAULT_CURVE_STYLE)
        else:
            self.curve_styles = self.curve_styles[:self.size]
        # validate curve widths
        if self.size > len(self.curve_widths):
            for i in range(len(self.curve_widths), self.size):
                self.curve_widths.append(DEFAULT_CURVE_WIDTH)
        else:
            self.curve_widths = self.curve_widths[:self.size]

data_sources = collections.OrderedDict()

def reload_data_sources():
    reset_sample_times()
    for name in data_sources:
        data_sources[name].reload()
    refresh_scopes()

def add_data_source(name, mode, curve_names, curve_colors, curve_styles, curve_widths):
    global data_sources
    if name not in data_sources:
        data_sources[name] = DataSource(name, mode, curve_names, curve_colors, curve_styles, curve_widths)

def remove_data_source(name):
    global data_sources
    if name in data_sources:
        del data_sources[name]

def remove_all_data_sources():
    global data_sources
    data_sources.clear()

#==============================================================================
# SCOPE MODULE
#==============================================================================

class ScopeModule(QtGui.QTabWidget):  # or QtGui.QGroupBox or QTabWidget

    def __init__(self, index, title, mode='Scope', yrange=[-1,1], show_legend=False):
        super(ScopeModule, self).__init__()
        # scope properties
        self.index = index
        self.title = title
        self.mode = mode
        self.yrange = yrange
        self.show_legend = show_legend
        # bottom axis ticks
        self.time_ticks = [[(-t, t) for t in range(SAMPLE_DURATION + 1)]]
        self.time_ticks[0][0] = (0, '0  ')
        # various object handles
        self.plot_widget = pyqtgraph.PlotWidget(name='MEL Scope')
        self.axis_left = self.plot_widget.getAxis('left')
        self.axis_bottom = self.plot_widget.getAxis('bottom')
        self.plot_item = self.plot_widget.getPlotItem()
        self.view_box = self.plot_item.getViewBox()
        # configure plot settings
        self.plot_widget.setRange(xRange=[-SAMPLE_DURATION, 0], yRange=[-1, 1])
        if time_mode == 'Fixed':
            self.plot_widget.setLimits(xMin=-SAMPLE_DURATION, xMax=0)
            self.axis_bottom.setTicks(self.time_ticks)
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setDownsampling(mode='peak')
        self.axis_bottom.setStyle(tickTextWidth=100)
        # self.plot_item.setMenuEnabled(False)
        self.axis_bottom.setZValue(0)
        self.axis_left.setZValue(0)
        # scope module curves and data
        self.curves = {}
        # scope title label
        self.label = QtGui.QLabel()
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setFont(BOLD_FONT)
        # scope module layout
        self.setTabPosition(2)
        self.scope_tab = QtGui.QWidget()
        self.layout = QtGui.QGridLayout()
        self.scope_tab.setLayout(self.layout)
        self.layout.addWidget(self.label, 0, 0, 1, 1)
        self.layout.addWidget(self.plot_widget, 1, 0, 1, 1)
        self.addTab(self.scope_tab, '    Plot    ')
        # Numeric Tab
        self.io_tab = QtGui.QWidget()
        self.io_layout = QtGui.QGridLayout()
        self.io_layout.setAlignment(QtCore.Qt.AlignTop)
        self.io_label = QtGui.QLabel()
        self.io_label.setAlignment(QtCore.Qt.AlignCenter)
        self.io_label.setFont(BOLD_FONT)
        self.io_layout.addWidget(self.io_label, 0, 0, 1, 2)
        self.io_tab.setLayout(self.io_layout)
        self.addTab(self.io_tab, ' Numeric ')
        self.io_names = {}
        self.io_values = {}
        # Settings Tab
        # self.settings_tab = QtGui.QWidget()
        # self.settings_layout = QtGui.QGridLayout()
        # self.settings_layout.setAlignment(QtCore.Qt.AlignTop)
        # self.settings_label = QtGui.QLabel()
        # self.settings_label.setAlignment(QtCore.Qt.AlignCenter)
        # self.settings_label.setFont(BOLD_FONT)
        # self.settings_layout.addWidget(self.settings_label, 0, 0, 1, 2)
        # self.addTab(self.settings_tab, ' Settings ')
        # make filter
        self.filter = {}
        for name in data_sources:
            data_source_filter = [True if len(data_sources) < 1 else False for j in range(data_sources[name].size)]
            self.filter[name] = data_source_filter
        # on tab changed
        self.currentChanged.connect(self.on_tab_changed)
        self.reset_write_only = True
        # self.axis_left.range = [1.1 * x for x in self.yrange]

    def on_tab_changed(self):
        self.mode = SCOPE_MODE_OPTIONS[self.currentIndex()]

    def refresh(self):
        # set current tab
        self.setCurrentIndex(SCOPE_MODE_OPTIONS.index(self.mode))
        # set scope range
        # self.axis_left.range = [1.1 * x for x in self.yrange]
        self.plot_widget.setRange(yRange=self.yrange)
        # add/remove/refresh legend
        if self.plot_item.legend is not None:
            if self.plot_item.legend.scene() is not None:
                self.plot_item.legend.scene().removeItem(self.plot_item.legend)
        if self.show_legend:
            self.plot_item.addLegend()
        # set title
        self.label.setText(self.title)
        self.io_label.setText(self.title)
        # self.settings_label.setText(self.title)
        # validate filter
        self.validate_filter()
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
        # reset write only flag
        self.reset_write_only = True
        # rebuild
        self.curves = {}
        self.io_names, self.io_values = {}, {}
        row = 1
        for name in data_sources:
            data_souce = data_sources[name]
            self.curves[name] = []
            self.io_names[name] = []
            self.io_values[name] = []
            for i in range(data_sources[name].size):
                if self.filter[name][i] is True:
                    # update curve
                    new_pen = pyqtgraph.mkPen(data_souce.curve_colors[i], style=CURVE_STYLE_OPTIONS[data_souce.curve_styles[i]], width=data_souce.curve_widths[i])
                    new_curve = pyqtgraph.PlotCurveItem(
                        pen=new_pen, name=data_souce.curve_names[i])
                    self.plot_widget.addItem(new_curve)
                    self.curves[name].append(new_curve)
                    # update IO
                    new_name = QtGui.QLabel(self)
                    new_name.setText("  " + data_souce.curve_names[i])
                    # set fill color
                    c = data_souce.curve_colors[i]
                    curve_color = QtGui.QColor(c[0], c[1], c[2])
                    curve_label_font = QtGui.QFont()
                    curve_label_font.setBold(True)
                    curve_label_font.setPointSize(8)
                    new_name.setFont(curve_label_font)
                    new_name.setStyleSheet('color: white; background-color: %s' % curve_color.name())
                    new_value = QtGui.QLineEdit(self)
                    new_value.setText('0.0000')
                    new_value.setValidator(QtGui.QDoubleValidator())
                    if data_souce.mode == 'Read Only':
                        new_value.setReadOnly(True)
                    elif data_souce.mode == 'Write Only':
                        io_color = QtGui.QColor(THEME_SCOPE_IO_CONFIRMED_COLORS[theme][0], THEME_SCOPE_IO_CONFIRMED_COLORS[
                                                theme][1], THEME_SCOPE_IO_CONFIRMED_COLORS[theme][2])
                        new_value.setStyleSheet('background-color: %s' % io_color.name())
                        new_value.returnPressed.connect(self.confirm_write_factory(name, i, new_value))
                        new_value.textEdited.connect(self.change_io_factory(new_value))
                    new_value.setAlignment(QtCore.Qt.AlignCenter)
                    self.io_layout.addWidget(new_name, row, 0)
                    self.io_layout.addWidget(new_value, row, 1)
                    for i in range(2):
                        self.io_layout.setColumnStretch(i, 1)
                    self.io_names[name].append(new_name)
                    self.io_values[name].append(new_value)
                    row += 1
                else:
                    self.curves[name].append(None)
                    self.io_names[name].append(None)
                    self.io_values[name].append(None)

    def update(self):
        if time_mode == 'Rolling':
            self.plot_widget.setLimits(xMin=elapsed_time_sr-SAMPLE_DURATION, xMax=elapsed_time_sr)
        for name in data_sources:
            for curve, io_value, filter, data, value_text in zip(self.curves[name], self.io_values[name], self.filter[name], data_sources[name].samples, data_sources[name].text):
                if filter:
                    if self.mode == 'Plot':
                        curve.setData(sampled_times, data)
                    elif self.mode == 'Numeric':
                        if data_sources[name].mode == 'Read Only' or self.reset_write_only:
                            io_value.setText(value_text)
        self.reset_write_only = False

    def confirm_write(self, name, index, line_edit):
        global write_flag
        value = float(line_edit.text())
        data_sources[name].data[index] = value
        io_color = QtGui.QColor(THEME_SCOPE_IO_CONFIRMED_COLORS[theme][0], THEME_SCOPE_IO_CONFIRMED_COLORS[theme][1], THEME_SCOPE_IO_CONFIRMED_COLORS[theme][2])
        line_edit.setText('%0.4f' % data_sources[name].data[index])
        line_edit.setStyleSheet('background-color: %s' % io_color.name())
        status_bar.showMessage("Wrote " + str(value) + " to " + name + '[' + str(index) + '] "' + data_sources[name].curve_names[index] + '"')
        write_flag = True

    def confirm_write_factory(self, name, index, line_edit):
        return lambda: self.confirm_write(name, index, line_edit)

    def change_io(self, line_edit):
        io_color = QtGui.QColor(THEME_SCOPE_IO_CHANGING_COLORS[theme][0], THEME_SCOPE_IO_CHANGING_COLORS[theme][1], THEME_SCOPE_IO_CHANGING_COLORS[theme][2])
        line_edit.setStyleSheet('background-color: %s' % io_color.name())

    def change_io_factory(self, line_edit):
        return lambda: self.change_io(line_edit)

    def validate_filter(self):
        for name in data_sources:
            if name not in self.filter:
                self.filter[name] = []
            if data_sources[name].size > len(self.filter[name]):
                for i in range(len(self.filter[name]), data_sources[name].size):
                    self.filter[name].append(True if len(data_sources) <= 1 else False)
            else:
                self.filter[name] = self.filter[name][:data_sources[name].size]
        for key, value in self.filter.items():
            if key not in data_sources:
                del self.filter[key]

scope_count = 0

scope_modules = []

def refresh_scopes():
    for scope in scope_modules:
        scope.refresh()

def add_scope_module(title, mode, yrange, show_legend):
    global scope_count
    scope_modules.append(ScopeModule(scope_count, title, mode, yrange, show_legend))
    scope_count += 1

def remove_scope_module():
    global scope_count
    scope_modules.pop().deleteLater()
    scope_count -= 1

def remove_all_scope_modulels():
    global scope_count
    del scope_modules[:]
    scope_count = 0;


#==============================================================================
# GRID & WINDOW
#==============================================================================

grid_rows = 1
grid_cols = 1

def set_theme():
    global application
    application.setStyleSheet(THEME_STYLESHEETS[theme])
    pyqtgraph.setConfigOption('background', THEME_SCOPE_BG_COLORS[theme])
    pyqtgraph.setConfigOption('foreground', THEME_SCOPE_FB_COLORS[theme])
    refresh_scopes()

def center_window():
    frameGm = main_window.frameGeometry()
    screen = QtGui.QApplication.desktop().screenNumber(QtGui.QApplication.desktop().cursor().pos())
    centerPoint = QtGui.QApplication.desktop().screenGeometry(screen).center()
    frameGm.moveCenter(centerPoint)
    main_window.move(frameGm.topLeft())

def resize_grid():
    global main_window, main_widget, grid, grid_rows, grid_cols, scope_count
    new_count = grid_rows * grid_cols
    # remove scopes from GRID
    for i in range(scope_count):
        grid.removeWidget(scope_modules[i])
    # reset the GRID (???)
    main_widget.deleteLater()
    grid.deleteLater()
    main_widget, grid = None, None
    main_widget = QtGui.QWidget()
    main_window.setCentralWidget(main_widget)
    main_window.resize(640, 640)
    main_widget.setContentsMargins(12, 12, 12, 12)
    grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
    grid.setContentsMargins(0, 0, 0, 0)
    main_widget.setLayout(grid)
    # add new scopes
    if new_count > scope_count:
        add = new_count - scope_count
        for i in range(add):
            add_scope_module('Scope' + str(scope_count), 'Plot', [-1,1], False)
    # or remove old scopes
    elif new_count < scope_count:
        remove = scope_count - new_count
        for i in range(remove):
            remove_scope_module()
    # readd scopes to GRID
    positions = [(r, c) for r in range(grid_rows)
                 for c in range(grid_cols)]
    for i in range(new_count):
        scope_modules[i].index = i
        grid.addWidget(scope_modules[i], positions[i][0], positions[i][1], 1, 1)
    # set row/column stretch to ensure same sizes (!!! THIS HAS ISSUES)
    for i in range(grid_rows):
        grid.setRowStretch(i, 1)
    for i in range(grid_cols):
        grid.setColumnStretch(i, 1)
    # update scope count
    # scope_count = new_count
    main_window.resize(grid_cols * DEFAULT_WIDTH, grid_rows * DEFAULT_HEIGHT)
    center_window()


def reload_grid():
    resize_grid()
    refresh_scopes()

#==============================================================================
# OPEN/SAVE
#==============================================================================

filepath = None

def open_new_instance():
    CREATE_NO_WINDOW = 0x08000000
    subprocess.Popen([sys.executable, 'MelScope.pyw'],
                     creationflags=CREATE_NO_WINDOW)

def open():
    global filepath
    filepath = QtGui.QFileDialog.getOpenFileName(
        main_widget, 'Open MEL Scope', "", 'Scope Files (*.scope *.yaml)')
    if filepath:
        stream = file(filepath, 'r')
        config = yaml.load(stream)
        remove_all_data_sources()
        remove_all_scope_modulels()
        deploy_config(config)
        reload_data_sources()
        resize_grid()
        set_theme()
        reload_all()
        filename = str(filepath[str(filepath).rfind('/') + 1:])
        filename = filename[0: filename.rfind('.')]
        status_bar.showMessage('Opened <' + filename + '>')
        main_window.setWindowTitle('MEL Scope (' + filename + ')')


def save():
    global filepath
    if filepath:
        config = generate_config()
        stream = file(filepath, 'w')
        yaml.dump(config, stream)
        filename = str(filepath[str(filepath).rfind('/') + 1:])
        filename = filename[0: filename.rfind('.')]
        status_bar.showMessage('Saved <' + filename + '>')
        main_window.setWindowTitle('MEL Scope (' + filename + ')')
    else:
        save_as()


def save_as():
    global filepath
    new_filepath = QtGui.QFileDialog.getSaveFileName(main_widget, 'Save MEL Scope', "", 'Scope Files (*.scope *.yaml)')
    if new_filepath:
        filepath = new_filepath
        save()


def deploy_config(config):
    global theme, grid_rows, grid_cols
    theme = config['THEME']
    grid_rows = config['GRID_ROWS']
    grid_cols = config['GRID_COLS']
    for x in config['DATA_SOURCES']:
        add_data_source(
            x['name'],
            x['mode'],
            x['curve_names'],
            x['curve_colors'],
            x['curve_styles'],
            x['curve_widths'])
    for x in config['SCOPES']:
        add_scope_module(
            x['title'],
            x['mode'],
            x['yrange'],
            x['legend'])
        scope_modules[-1].filter = x['filter']

def generate_config():
    config = {}
    config['THEME'] = theme
    config['GRID_ROWS'] = grid_rows
    config['GRID_COLS'] = grid_cols
    config['DATA_SOURCES'] = []
    for name in data_sources:
        x = {
            'name': data_sources[name].name,
            'mode': data_sources[name].mode,
            'size': data_sources[name].size,
            'curve_names': data_sources[name].curve_names,
            'curve_colors': data_sources[name].curve_colors,
            'curve_styles': data_sources[name].curve_styles,
            'curve_widths': data_sources[name].curve_widths
        }
        config['DATA_SOURCES'].append(x)
    config['SCOPES'] = []
    for scope in scope_modules:
        x = {
            'index': scope.index,
            'title': scope.title,
            'mode': scope.mode,
            'yrange': scope.yrange,
            'legend': scope.show_legend,
            'filter': scope.filter
            }
        config['SCOPES'].append(x)
    return config

#==============================================================================
# DIALOG CLASSES
#==============================================================================

class AddDataSourceDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(AddDataSourceDialog, self).__init__(parent)
        self.setWindowTitle('Add Data Source')
        self.layout = QtGui.QGridLayout(self)

        self.name_label = QtGui.QLabel(self)
        self.name_label.setText("MEL Share / MEL Net   ")
        self.name_label.setFont(BOLD_FONT)
        self.layout.addWidget(self.name_label, 0, 0)

        self.mode_label = QtGui.QLabel(self)
        self.mode_label.setText("Mode")
        self.mode_label.setFont(BOLD_FONT)
        self.layout.addWidget(self.mode_label, 0, 1)

        self.name_line_edit = QtGui.QLineEdit(self)
        self.layout.addWidget(self.name_line_edit, 1, 0)

        self.mode_combo_box = QtGui.QComboBox(self)
        self.mode_combo_box.addItems(DATA_MODE_OPTIONS)
        self.layout.addWidget(self.mode_combo_box, 1, 1)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.layout.addWidget(self.buttons, 2, 0, 1, 2)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    @staticmethod
    def get_input(parent=None):
        dialog = AddDataSourceDialog(parent)
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
        index_label.setFont(BOLD_FONT)
        layout.addWidget(index_label, 0, 0)

        name_label = QtGui.QLabel(self)
        name_label.setText('Name')
        name_label.setFont(BOLD_FONT)
        layout.addWidget(name_label, 0, 1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(BOLD_FONT)
        layout.addWidget(mode_label, 0, 2)

        size_label = QtGui.QLabel(self)
        size_label.setText('Weight')
        size_label.setFont(BOLD_FONT)
        layout.addWidget(size_label, 0, 3)

        color_label = QtGui.QLabel(self)
        color_label.setText('Color')
        color_label.setFont(BOLD_FONT)
        color_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(color_label, 0, 4)

        line_label = QtGui.QLabel(self)
        line_label.setText('Line Style')
        line_label.setFont(BOLD_FONT)
        layout.addWidget(line_label, 0, 5)

        # storage containers for GUI elements
        self.name_line_edits = {}
        self.mode_labels = {}
        self.size_spin_boxes = {}
        self.color_buttons = {}
        self.line_combo_boxes = {}

        row = 1
        for name in data_sources:
            self.name_line_edits[name] = []
            self.mode_labels[name] = []
            self.size_spin_boxes[name] = []
            self.color_buttons[name] = []
            self.line_combo_boxes[name] = []

            for i in range(data_sources[name].size):
                row += 1
                new_index_label = QtGui.QLabel(self)
                new_index_label.setText(name + '[' + str(i) + ']')
                new_index_label.setFont(BOLD_FONT)
                layout.addWidget(new_index_label, row, 0)

                new_line_edit = QtGui.QLineEdit(self)
                new_line_edit.setText(data_sources[name].curve_names[i])
                self.name_line_edits[name].append(new_line_edit)
                layout.addWidget(new_line_edit, row, 1)

                new_mode_label = QtGui.QLabel(self)
                new_mode_label.setText(data_sources[name].mode)
                self.mode_labels[name].append(new_mode_label)
                layout.addWidget(new_mode_label, row, 2)

                new_spin_box = QtGui.QSpinBox(self)
                new_spin_box.setValue(int(data_sources[name].curve_widths[i]))
                new_spin_box.setAlignment(QtCore.Qt.AlignCenter)
                new_spin_box.setRange(1, 10)
                self.size_spin_boxes[name].append(new_spin_box)
                layout.addWidget(new_spin_box, row, 3)

                c = data_sources[name].curve_colors[i]
                curve_color = QtGui.QColor(c[0], c[1], c[2])

                new_color_button = QtGui.QPushButton(self)
                new_color_button.setMaximumHeight(15 * RESOLUTION_SCALE)
                new_color_button.setMaximumWidth(30 * RESOLUTION_SCALE)
                new_color_button.setStyleSheet(
                    ' background-color: %s' % curve_color.name())
                new_color_button.clicked.connect(
                    self.prompt_color_dialog_factory(name, i))
                self.color_buttons[name].append(new_color_button)
                layout.addWidget(new_color_button, row, 4)

                new_line_combo_box = QtGui.QComboBox(self)
                new_line_combo_box.addItems(CURVE_STYLE_OPTIONS.keys())
                new_line_combo_box.setCurrentIndex(
                    CURVE_STYLE_OPTIONS.keys().index(data_sources[name].curve_styles[i]))
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
        for name in data_sources:
            for i in range(data_sources[name].size):
                data_sources[name].curve_names[i] = str(self.name_line_edits[name][i].text())
                data_sources[name].curve_widths[i] = int(self.size_spin_boxes[name][i].value())
                color = self.color_buttons[name][i].palette().color(
                    QtGui.QPalette.Background)
                data_sources[name].curve_colors[i] = [color.red(), color.green(), color.blue()]
                data_sources[name].curve_styles[i] = str(self.line_combo_boxes[name][i].currentText())
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
        self.setWindowTitle('Configure Scope(s)')
        layout = QtGui.QGridLayout(self)

        c = THEME_SCOPE_FB_COLORS[theme]
        border_color = QtGui.QColor(c[0], c[1], c[2])

        x = 0
        for name in data_sources:
            filter_label = QtGui.QLabel(self)
            filter_label.setText("  " + name + "  ")
            filter_label.setFont(BOLD_FONT)
            filter_label.setAlignment(QtCore.Qt.AlignCenter)
            filter_label.setStyleSheet("border: 1px solid %s" % border_color.name());
            span = 1
            if data_sources[name].size > 0:
                span = data_sources[name].size
            layout.addWidget(filter_label, 0, 6 + x, 1, span)
            x += span

        scope_label = QtGui.QLabel(self)
        scope_label.setText('Scope Options')
        scope_label.setFont(BOLD_FONT)
        scope_label.setAlignment(QtCore.Qt.AlignCenter)
        scope_label.setStyleSheet("border: 1px solid %s" % border_color.name());
        layout.addWidget(scope_label, 0, 3, 1, 3)

        index_label = QtGui.QLabel(self)
        index_label.setText('')
        index_label.setFont(BOLD_FONT)
        index_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(index_label, 1, 0)

        title_label = QtGui.QLabel(self)
        title_label.setText('Scope Name')
        title_label.setFont(BOLD_FONT)
        layout.addWidget(title_label, 1, 1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(BOLD_FONT)
        layout.addWidget(mode_label, 1, 2)

        legend_label = QtGui.QLabel(self)
        legend_label.setText('Legend')
        legend_label.setFont(BOLD_FONT)
        layout.addWidget(legend_label, 1, 3)

        min_label = QtGui.QLabel(self)
        min_label.setText('yMin')
        min_label.setFont(BOLD_FONT)
        min_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(min_label, 1, 4)

        max_label = QtGui.QLabel(self)
        max_label.setText('yMax')
        max_label.setFont(BOLD_FONT)
        max_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(max_label, 1, 5)

        self.title_line_edits = []
        self.mode_combo_boxes = []
        self.legend_checkboxes = []
        self.min_line_edits = []
        self.max_line_edits = []
        self.filter_checkboxes = []

        x = 0
        for name in data_sources:
            for i in range(data_sources[name].size):
                c = data_sources[name].curve_colors[i]
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
            x += data_sources[name].size

        for i in range(scope_count):

            new_index_label = QtGui.QLabel(self)
            new_index_label.setText(str(i))
            new_index_label.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_index_label, i + 2, 0)

            new_line_edit = QtGui.QLineEdit(self)
            new_line_edit.setText(scope_modules[i].title)
            new_line_edit.setMinimumWidth(150 * RESOLUTION_SCALE)
            self.title_line_edits.append(new_line_edit)
            layout.addWidget(new_line_edit, i + 2, 1)

            new_mode_combo_box = QtGui.QComboBox(self)
            new_mode_combo_box.addItems(SCOPE_MODE_OPTIONS)
            new_mode_combo_box.setCurrentIndex(
                SCOPE_MODE_OPTIONS.index(scope_modules[i].mode))
            self.mode_combo_boxes.append(new_mode_combo_box)
            layout.addWidget(new_mode_combo_box, i + 2, 2)

            new_legend_checkbox = QtGui.QCheckBox(self)
            new_legend_checkbox.setChecked(scope_modules[i].show_legend)
            new_legend_checkbox.setFixedWidth(25 * RESOLUTION_SCALE)
            self.legend_checkboxes.append(new_legend_checkbox)
            layout.addWidget(new_legend_checkbox, i + 2, 3)

            new_min_line_edit = QtGui.QLineEdit(self)
            new_min_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_min_line_edit.setText(str(scope_modules[i].yrange[0]))
            new_min_line_edit.setFixedWidth(50 * RESOLUTION_SCALE)
            self.min_line_edits.append(new_min_line_edit)
            layout.addWidget(new_min_line_edit, i + 2, 4)

            new_max_line_edit = QtGui.QLineEdit(self)
            new_max_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_max_line_edit.setText(str(scope_modules[i].yrange[1]))
            new_max_line_edit.setFixedWidth(50 * RESOLUTION_SCALE)
            self.max_line_edits.append(new_max_line_edit)
            layout.addWidget(new_max_line_edit, i + 2, 5)

            self.filter_checkboxes.append({})

            x = 0
            for name in data_sources:
                self.filter_checkboxes[i][name] = []
                for j in range(data_sources[name].size):
                    new_check_box = QtGui.QCheckBox(self)
                    new_check_box.setChecked(scope_modules[i].filter[name][j])
                    self.filter_checkboxes[i][name].append(new_check_box)
                    layout.addWidget(new_check_box, i + 2, x + j + 6)
                x += data_sources[name].size

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)
        self.buttons.button(QtGui.QDialogButtonBox.Apply).clicked.connect(self.apply)
        layout.addWidget(self.buttons, len(scope_modules) + 2, 0, 1, layout.columnCount())

    def apply(self):
        for i in range(scope_count):
            for name in data_sources:
                for j in range(data_sources[name].size):
                    scope_modules[i].filter[name][j] = self.filter_checkboxes[i][name][j].isChecked()
            scope_modules[i].title = str(self.title_line_edits[i].text())
            scope_modules[i].mode = str(self.mode_combo_boxes[i].currentText())
            scope_modules[i].show_legend = self.legend_checkboxes[i].isChecked()
            scope_modules[i].yrange[0] = float(self.min_line_edits[i].text())
            scope_modules[i].yrange[1] = float(self.max_line_edits[i].text())
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
        title.setText("MEL Scope v" + VER)
        title_font = QtGui.QFont()
        title_font.setBold(True)
        title_font.setPointSize(12)
        title.setFont(title_font)
        title.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(title, 0,0,1,2)

        lab = QtGui.QLabel(self)
        lab.setText("MAHI Lab | Rice University")
        lab.setFont(BOLD_FONT)
        lab.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(lab, 1,0,1,2)

        author_lh = QtGui.QLabel(self)
        author_lh.setText("Author:")
        author_lh.setFont(BOLD_FONT)
        layout.addWidget(author_lh, 2, 0)

        author_rh = QtGui.QLabel(self)
        author_rh.setText("Evan Pezent")
        layout.addWidget(author_rh, 2, 1)

        email_lh = QtGui.QLabel(self)
        email_lh.setText("Email:")
        email_lh.setFont(BOLD_FONT)
        layout.addWidget(email_lh, 3, 0)

        email_rh = QtGui.QLabel(self)
        email_rh.setText("epezent@rice.edu")
        layout.addWidget(email_rh, 3, 1)

        website_lh = QtGui.QLabel(self)
        website_lh.setText("Website:")
        website_lh.setFont(BOLD_FONT)
        layout.addWidget(website_lh, 4, 0)

        website_rh = QtGui.QLabel(self)
        website_rh.setText("www.evanpezent.com")
        layout.addWidget(website_rh, 4, 1)

    @staticmethod
    def open_dialog(parent=None):
        dialog = AboutDialog(parent)
        dialog.exec_()


#==============================================================================
# ACTION FUNCTIONS
#==============================================================================

def prompt_configure_data():
    if len(data_sources) > 0:
        ConfigureDataDialog.open_dialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a Data Source before configuring!')
        msgBox.setWindowTitle('Configure Data')
        msgBox.exec_()

def prompt_configure_modules():
    ConfigureModulesDialog.open_dialog()

def prompt_add_data_source():
    name, mode, ok = AddDataSourceDialog.get_input()
    if ok:
        if name in data_sources:
            status_bar.showMessage("Data Source <" + name + "> already exists.")
        else:
            add_data_source(name, mode, [], [], [], [])
            reload_data_sources()
            status_bar.showMessage("Added Data Source <" + name + ">")

def prompt_remove_melshare():
    selection, ok = QtGui.QInputDialog.getItem(main_widget, 'Remove Data Source', 'Select Data Source:', list(data_sources.keys()), 0, False)
    if ok:
        name = str(selection)
        remove_data_source(name)
        reload_data_sources()
        status_bar.showMessage("Removed Data Source <" + name + ">")


def prompt_resize_grid():
    global grid_rows, grid_cols, scope_count
    i = GRID_SIZE_OPTIONS.index((grid_rows, grid_cols))
    selection, ok = QtGui.QInputDialog.getItem(
        main_widget, 'Grid Size', 'Select Grid Size:', GRID_SIZE_OPTIONS_DISPLAY, i, False)
    if ok:
        grid_rows, grid_cols = GRID_SIZE_OPTIONS_DICT[str(selection)]
        reload_grid()
        status_bar.showMessage('Grid resized to ' + str(grid_rows) + ' x ' + str(grid_cols))

def about():
    AboutDialog.open_dialog()

def open_github():
    webbrowser.open('https://github.com/epezent/MEL')

def prompt_scroll_mode():
    global time_mode
    selection, ok = QtGui.QInputDialog.getItem(
        main_widget, 'Scroll Mode', 'Select Mode:', SCROLL_OPTIONS, SCROLL_OPTIONS.index(time_mode), False)
    if ok:
        time_mode = str(selection)

def prompt_change_theme():
    global theme, THEME_OPTIONS
    selection, ok = QtGui.QInputDialog.getItem(
        main_widget, 'Theme', 'Select Theme:', THEME_OPTIONS, THEME_OPTIONS.index(theme), False)
    if ok:
        theme = str(selection)
        set_theme()
        status_bar.showMessage("Theme changed to " + theme)

def reload_all():
    reload_data_sources()
    resize_grid()
    refresh_scopes()

paused = False

def switch_pause():
    global paused
    paused =  not paused
    if paused:
        pause_label.setText("Paused")
        pause_action.setText('R&esume')
    else:
        pause_label.setText("")
        pause_action.setText("&Pause")


#==============================================================================
# MENU BAR
#==============================================================================

menu_bar = main_window.menuBar()

file_menu = menu_bar.addMenu('&File')
edit_menu = menu_bar.addMenu('&Edit')
action_menu = menu_bar.addMenu('&Actions')
pref_menu = menu_bar.addMenu('&Preferences')
help_menu = menu_bar.addMenu('&Help')

new_action = QtGui.QAction('&New', main_window,
                           shortcut='Ctrl+N', statusTip='Create a MEL Scope', triggered=open_new_instance)
file_menu.addAction(new_action)

open_action = QtGui.QAction('&Open...', main_window,
                            shortcut='Ctrl+O', statusTip='Open an existing MEL Scope', triggered=open)
file_menu.addAction(open_action)

save_action = QtGui.QAction('&Save', main_window,
                            shortcut='Ctrl+S', statusTip='Save this MEL Scope', triggered=save)
file_menu.addAction(save_action)

save_as_action = QtGui.QAction('Save &As...', main_window,
                               shortcut='Ctrl+Shift+S', statusTip='Save this MEL Scope under a new name', triggered=save_as)
file_menu.addAction(save_as_action)

reload_action = QtGui.QAction('&Reload', main_window,
                              shortcut='R', statusTip='Reload the MEL Scope', triggered=reload_data_sources)
action_menu.addAction(reload_action)

pause_action = QtGui.QAction('&Pause', main_window,
    shortcut='P',statusTip='Pause the MEL Scope', triggered=switch_pause)
action_menu.addAction(pause_action)

add_action = QtGui.QAction('&Add Data Source...', main_window,
                               shortcut='Ctrl+A', statusTip='Add a data source', triggered=prompt_add_data_source)
edit_menu.addAction(add_action)

remove_action = QtGui.QAction('&Remove Data Source...', main_window,
                               shortcut='Ctrl+X', statusTip='Remove an existing data source', triggered=prompt_remove_melshare)
edit_menu.addAction(remove_action)

grid_action = QtGui.QAction('Resize &Grid...', main_window,
                            shortcut='Ctrl+G', statusTip='Resize the scope grid', triggered=prompt_resize_grid)
edit_menu.addAction(grid_action)

config_curves_action = QtGui.QAction('Configure &Data...', main_window,
                                     shortcut='Ctrl+D', statusTip='Configure data and curve properties', triggered=prompt_configure_data)
edit_menu.addAction(config_curves_action)

configure_modules_action = QtGui.QAction('&Configure Scope(s)...', main_window,
                                         shortcut='Ctrl+C', statusTip='Configure scope properties', triggered=prompt_configure_modules)
edit_menu.addAction(configure_modules_action)

theme_action = QtGui.QAction('&Theme...', main_window,
                             shortcut='F10', statusTip='Change the current theme', triggered=prompt_change_theme)
pref_menu.addAction(theme_action)

about_action = QtGui.QAction('&About', main_window,
                             shortcut='F11', statusTip='About MEL Scope', triggered=about)
help_menu.addAction(about_action)

github_action = QtGui.QAction('&GitHub...', main_window,
                              shortcut='F12', statusTip='Open MEL GitHub webpage', triggered=open_github)
help_menu.addAction(github_action)


def update_menu():
    global config_curves_action, configure_modules_action
    if len(data_sources) == 0:
        remove_action.setDisabled(True)
    else:
        remove_action.setDisabled(False)

#==============================================================================
# STATUS BAR
#==============================================================================

status_bar = QtGui.QStatusBar()
status_bar.setSizeGripEnabled(False)
main_window.setStatusBar(status_bar)

rate_label = QtGui.QLabel()
rate_label.setText('0')
rate_label.setAlignment(QtCore.Qt.AlignRight)
rate_label.setFixedWidth(50 * RESOLUTION_SCALE)

fps_label = QtGui.QLabel()
fps_label.setText('0')
fps_label.setAlignment(QtCore.Qt.AlignRight)
fps_label.setFixedWidth(50 * RESOLUTION_SCALE)

pause_label = QtGui.QLabel()
pause_label.setText('')
pause_label.setFont(BOLD_FONT)
pause_label.setAlignment(QtCore.Qt.AlignLeft)

status_bar.addPermanentWidget(pause_label)
status_bar.addPermanentWidget(fps_label)
status_bar.addPermanentWidget(rate_label)

#==============================================================================
# MAIN APPLICATION EXECUTION / LOOP
#==============================================================================

# set custom colors in color widget
for i in range (QtGui.QColorDialog.customCount()):
    rgb = CURVE_COLOR_OPTIONS[i]
    color = QtGui.QColor(rgb[0], rgb[1], rgb[2])
    QtGui.QColorDialog.setCustomColor(i, color.rgb())

set_theme()
reload_grid()
status_bar.showMessage('Welcome to MEL Scope!')

def sample_loop():
    global write_flag
    update_sample_times()
    update_sample_rate()
    for name in data_sources:
        if data_sources[name].mode == 'Read Only':
            data_sources[name].sample()
        elif data_sources[name].mode == 'Write Only':
            if write_flag is True:
                data_sources[name].write()
    write_flag = False


def render_loop():
    update_render_time()
    update_FPS()
    update_menu()
    if not paused:
        for scope in scope_modules:
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

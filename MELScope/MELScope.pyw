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
# curve sizes
# splitter widget
# add cbrewer colors to color picker
# turn scopes on off
# thread melshare
# save window size
# curve scaling (deg2rad)
# Dark theme
# scope modules to generic modules, add interactive gain modules with sliders
# open from .scope
# add scope mode
# change grid item class structure to be less bulky and titles separate

#######################
# MEL SCOPE ABOUT INFO
#######################
ver = '1.2'

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
main_window.resize(640,640)
widg.setContentsMargins(12, 12, 12, 12)
grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
grid.setContentsMargins(0, 0, 0, 0)
widg.setLayout(grid)

bold_font = QtGui.QFont()
bold_font.setBold(True)


##############
# THEME SETUP
##############

theme_options = ['Classic', 'Dark']
theme_stylesheets = {'Classic': '', 'Dark': qdarkstyle.load_stylesheet(pyside=False)}
theme_scope_bg_colors = {"Classic": [240, 240, 240], "Dark": [49,  54,  59 ]}
theme_scope_fg_colors = {"Classic": [0,   0,   0  ], "Dark": [240, 240, 240]}
theme_scope_vb_colors = {"Classic": [240, 240, 240], "Dark": [35,  38,  41 ]}
theme_io_rw_colors = {"Classic": [204, 232, 255], "Dark": [24, 70, 93]}

theme = 'Dark'

def set_theme():
    global app
    app.setStyleSheet(theme_stylesheets[theme])
    pyqtgraph.setConfigOption('background', theme_scope_bg_colors[theme])
    pyqtgraph.setConfigOption('foreground', theme_scope_fg_colors[theme])
    refresh_scopes()


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

time_start = time()
time_last = time()
time_now = time()
delta_time = 0
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
        fps_actual = 1.0 / delta_time
    else:
        s = np.clip(delta_time * 3., 0, 1)
        fps_actual = fps_actual * (1 - s) + (1.0 / delta_time) * s
    fps_label.setText('FPS: %0.0f ' % fps_actual)

#################################
# MELSHARE VARIABLES / FUNCTIONS
#################################

melshare = ctypes.WinDLL("MELShare.dll")
melshare_name = ''
melshare_size = 0
double_array = ctypes.c_double * melshare_size
melshare_buffer = double_array()
melshare_connected = False

def connect_melshare():
    global melshare_name, melshare_size, melshare_buffer, double_array, melshare_connected
    result = melshare.get_map_size(melshare_name)
    if result >= 0:
        melshare_size = result
        melshare_connected = True
        double_array = ctypes.c_double * melshare_size
        melshare_buffer = double_array()
        status_bar.showMessage('Connected to MELShare <' + melshare_name + '>')
    else:
        melshare_size = 0
        melshare_connected = False
        status_bar.showMessage(
            'Failed to connect to MELShare <' + melshare_name + '>')

######################################
# DATA SAMPLING VARIABLES / FUNCTIONS
######################################

sample_duration = 10
sampled_times = None
sampled_data = []
sampled_values_text = []

def sample_data():
    global sampled_times, sampled_data, sampled_values_text, delta_time
    result = melshare.read_double_map(melshare_name, ctypes.byref(melshare_buffer), melshare_size)
    sampled_values_text = ['%0.2f' % value for value in melshare_buffer]
    if result >= 0:
        sampled_times[:-1] = sampled_times[1:] - delta_time
        sampled_times[-1] = 0
        for (data, i) in zip(sampled_data, range(melshare_size)):
            data[:-1] = data[1:]
            data[-1] = melshare_buffer[i]
        return True
    else:
        return False

def reset_samples():
    global num_samples, sampled_times, sampled_data
    num_samples = int(sample_duration * fps_target * 1.1)
    sampled_times = None
    sampled_data = []
    sampled_times = np.zeros(num_samples)
    melshare.read_double_map(melshare_name, ctypes.byref(melshare_buffer), melshare_size)
    for i in range(melshare_size):
        sampled_data.append(np.ones(num_samples) * melshare_buffer[i])



########################
# DATA PROPERTIES SETUP
########################

default_curve_mode = 'Read Only'
curve_mode_options = ['Read Only', 'Read Write']

default_curve_size = 3
default_curve_colors = [[31, 120, 180], [227, 26, 28], [51, 160, 44],
                        [255, 127, 0], [106, 61, 154], [177, 89, 40],
                        [166, 206, 227], [251, 154, 153], [178, 223, 138],
                        [253, 191, 111], [202, 178, 214], [255, 255, 153]]

default_curve_line = 'Solid'
curve_line_options = collections.OrderedDict( [
    ('Solid'       , QtCore.Qt.SolidLine),
    ('Dash'        , QtCore.Qt.DashLine),
    ('Dot'         , QtCore.Qt.DotLine),
    ('Dash Dot'    , QtCore.Qt.DashDotLine),
    ('Dash Dot Dot', QtCore.Qt.DashDotDotLine) ] )

# curve properties
curve_names = []
curve_modes =[]
curve_sizes = []
curve_colors = []
curve_lines = []

def validate_curve_properties():
    global curve_names, curve_modes, curve_sizes, curve_colors, curve_lines
    # print 'CALL: validate_curve_properties()'
    if melshare_size > len(curve_names):
        for i in range(len(curve_names), melshare_size):
            curve_names.append('Curve'+str(i))
    else:
        curve_names = curve_names[:melshare_size]
    if melshare_size > len(curve_modes):
        for i in range(len(curve_modes), melshare_size):
            curve_modes.append(default_curve_mode)
    else:
        curve_modes = curve_modes[:melshare_size]
    if melshare_size > len(curve_sizes):
        for i in range(len(curve_sizes), melshare_size):
            curve_sizes.append(default_curve_size)
    else:
        curve_sizes = curve_sizes[:melshare_size]
    if melshare_size > len(curve_colors):
        for i in range(len(curve_colors), melshare_size):
            curve_colors.append(default_curve_colors[i % len(default_curve_colors)])
    else:
        curve_colors = curve_colors[:melshare_size]
    if melshare_size > len(curve_lines):
        for i in range(len(curve_lines), melshare_size):
            curve_lines.append(default_curve_line)
    else:
        curve_lines = curve_lines[:melshare_size]



######################################
# SCOPE CLASS / VARIABLES / FUNCTIONS
######################################

scope_count = 0
scope_modules = []

default_scope_mode = 'Scope'
scope_mode_options = ['Scope', 'I/O']

# scope properties
scope_titles = []
scope_modes = []
scope_filters = []
scope_legends = []
scope_ranges = []

def validate_scope_properties():
    global scope_titles, scope_modes, scope_filters, scope_legends, scope_ranges
    if scope_count > len(scope_titles):
        for i in range(len(scope_titles), scope_count):
            scope_titles.append('Scope'+str(i))
    else:
        scope_titles = scope_titles[:scope_count]
    if scope_count > len(scope_modes):
        for i in range(len(scope_modes), scope_count):
            scope_modes.append(default_scope_mode)
    else:
        scope_modes = scope_modes[:scope_count]
    if scope_count > len(scope_filters):
        for i in range(len(scope_filters), scope_count):
            scope_filters.append([True for j in range(melshare_size)])
    else:
        scope_filters = scope_filters[:scope_count]
    for filter in scope_filters:
        if melshare_size > len(filter):
            for i in range(len(filter), melshare_size):
                filter.append(True)
        else:
            filter = filter[:melshare_size]
    if scope_count > len(scope_legends):
        for i in range(len(scope_legends), scope_count):
            scope_legends.append(False)
    else:
        scope_legends = scope_legends[:scope_count]
    if scope_count > len(scope_ranges):
        for i in range(len(scope_ranges), scope_count):
            scope_ranges.append([-1,1])
    else:
        scope_ranges = scope_ranges[:scope_count]



def refresh_scopes():
    for scope in scope_modules:
        scope.refresh()

class ScopeModule(QtGui.QTabWidget): # or QtGui.QGroupBox or QTabWidget
    def __init__(self):
        super(ScopeModule, self).__init__()
        # scope title
        self.index = None
        self.title = None
        # bottom axis ticks
        self.time_ticks = [[(-t, t) for t in range(sample_duration + 1)]]
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
        self.plot_widget.setRange(xRange=[-sample_duration, 0], yRange=[-1, 1])
        self.plot_widget.setLimits(xMin=-sample_duration, xMax=0)
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setDownsampling(mode='peak')
        self.axis_bottom.setTicks(self.time_ticks)
        self.axis_bottom.setStyle(tickTextWidth=100)
        #self.plot_item.setMenuEnabled(False)
        self.axis_bottom.setZValue(0)
        self.axis_left.setZValue(0)
        # scope module curves and data
        self.curves = []
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
        self.addTab(self.scope_tab,' Scope ')
        # IO Tab Setup
        self.io_tab = QtGui.QWidget()
        self.io_layout = QtGui.QGridLayout()
        self.io_layout.setAlignment(QtCore.Qt.AlignTop)
        self.io_label = QtGui.QLabel()
        self.io_label.mouseDoubleClickEvent = self.rename_io
        self.io_label.setAlignment(QtCore.Qt.AlignCenter)
        self.io_label.setFont(bold_font)
        self.io_layout.addWidget(self.io_label,0,0,1,3)
        self.io_tab.setLayout(self.io_layout)
        self.addTab(self.io_tab,' I/O ')
        self.io_names = []
        self.io_values = []
        self.io_sliders = []
        # filter
        self.filter = []
        # on tab changed
        self.currentChanged.connect(self.on_tab_changed)

    def on_tab_changed(self):
        global scope_modes
        scope_modes[self.index] = scope_mode_options[self.currentIndex()]

    def refresh(self):
        # set current tab
        self.setCurrentIndex(scope_mode_options.index(scope_modes[self.index]))
        self.plot_widget.setRange(yRange=scope_ranges[self.index])
        if self.plot_item.legend is not None:
            if self.plot_item.legend.scene() is not None:
                self.plot_item.legend.scene().removeItem(self.plot_item.legend)
        if scope_legends[self.index]:
            self.plot_item.addLegend()
        self.title = scope_titles[self.index]
        self.label.setText(self.title)
        self.io_label.setText(self.title)
        self.filter = scope_filters[self.index]
        self.plot_widget.clear()
        # apply theme settings
        self.view_box.setBackgroundColor(theme_scope_vb_colors[theme])
        self.plot_widget.setBackground(theme_scope_bg_colors[theme])
        self.axis_left.setPen(theme_scope_fg_colors[theme])
        self.axis_bottom.setPen(theme_scope_fg_colors[theme])
        if theme == 'Classic':
            color = QtGui.QColor(240,240,240)
            self.setStyleSheet(' background-color: %s' % color.name() )
        else:
            self.setStyleSheet('')
         # remove IO items
        for name in self.io_names:
            if name is not None:
                self.io_layout.removeWidget(name)
                name.deleteLater()
        for spinbox in self.io_values:
            if spinbox is not None:
                self.io_layout.removeWidget(spinbox)
                spinbox.deleteLater()
        for slider in self.io_sliders:
            if slider is not None:
                self.io_layout.removeWidget(slider)
                slider.deleteLater()
        # rebuild
        self.curves = []
        self.io_names, self.io_values, self.io_sliders = [], [], []
        row = 1
        for i in range(melshare_size):
            if self.filter[i]:
                # update curve
                new_pen = pyqtgraph.mkPen(curve_colors[i], style=curve_line_options[curve_lines[i]], width=curve_sizes[i])
                new_curve = pyqtgraph.PlotCurveItem(pen=new_pen,name=curve_names[i])
                self.plot_widget.addItem(new_curve)
                self.curves.append(new_curve)
                # update IO
                new_name = QtGui.QLabel(self)
                new_name.setText(curve_names[i])
                new_name.mouseDoubleClickEvent = self.rename_data_factory(i)
                new_value = QtGui.QLineEdit(self)
                new_value.setText('0.00')
                new_value.setValidator(QtGui.QDoubleValidator())
                if curve_modes[i] == 'Read Only':
                    new_value.setReadOnly(True)
                elif curve_modes[i] == 'Read Write':
                    rw_color = QtGui.QColor(theme_io_rw_colors[theme][0], theme_io_rw_colors[theme][1], theme_io_rw_colors[theme][2] )
                    new_value.setStyleSheet(' background-color: %s' % rw_color.name() )
                new_value.setAlignment(QtCore.Qt.AlignCenter)
                new_slider = QtGui.QSlider(self)
                new_slider.setOrientation(QtCore.Qt.Horizontal)
                self.io_layout.addWidget(new_name,row,0)
                self.io_layout.addWidget(new_value,row,1)
                self.io_layout.addWidget(new_slider,row,2)
                for i in range(3):
                    self.io_layout.setColumnStretch(i,1)
                self.io_names.append(new_name)
                self.io_values.append(new_value)
                self.io_sliders.append(new_slider)
                row += 1
            else:
                self.curves.append(None)
                self.io_names.append(None)
                self.io_values.append(None)
                self.io_sliders.append(None)


    def update(self):
        global curve_modes
        for curve, curve_mode, io_value, filter, data, value_text in zip(self.curves, curve_modes, self.io_values, self.filter, sampled_data, sampled_values_text):
            if filter:
                if scope_modes[self.index] == 'Scope':
                    curve.setData(sampled_times, data)
                elif scope_modes[self.index] == 'I/O':
                    if curve_mode == 'Read Only':
                        pass
                        io_value.setText(value_text)
                    elif curve_mode == 'Read Write':
                        pass

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
        scope_titles[self.index] = self.title
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
        scope_titles[self.index] = self.title
        self.io_label.setText(self.title)
        self.label.setText(self.title)
        self.layout.removeWidget(self.temp_line_edit)
        self.temp_line_edit.deleteLater()
        self.temp_line_edit = None

    def rename_data_factory(self, index):
        return lambda event: self.rename_data(index, event)

    def rename_data(self, index, event=None):
        print 'start', index
        self.temp_line_edit1 = QtGui.QLineEdit()
        self.temp_line_edit1.setText(curve_names[index])
        self.temp_line_edit1.editingFinished.connect(lambda : self.confirm_data_rename(index))
        self.temp_line_edit1.focusOutEvent = lambda event: self.confirm_data_rename(index, event)
        self.io_layout.addWidget(self.temp_line_edit1, index + 1, 0)
        self.temp_line_edit1.selectAll()
        self.temp_line_edit1.setFocus()

    def confirm_data_rename(self, index, event=None):
        print 'confirm', index
        curve_names[index] = str(self.temp_line_edit1.text())
        self.io_names[index].setText(curve_names[index])
        self.io_layout.removeWidget(self.temp_line_edit1)
        self.temp_line_edit1.deleteLater()
        self.temp_line_edit1 = None
        refresh_scopes()






###############################
# GRID VARIABLES / FUNCTIONS
##############################

max_grid_rows = 5
max_grid_cols = 5

grid_options = [(r+1,c+1) for r in range(max_grid_rows) for c in range(max_grid_cols)]
grid_options_display = [str(x[0]) + ' x ' + str(x[1]) for x in grid_options]
grid_options_dict = dict(zip(grid_options_display, grid_options))

grid_rows = 1
grid_cols = 1

def resize_grid():
    global main_window, widg, grid, grid_rows, grid_cols, scope_count, splitter_widgets
    new_count = grid_rows * grid_cols
    # remove scopes from grid
    for i in range(scope_count):
        grid.removeWidget(scope_modules[i])
    # reset the grid (???)
    widg.deleteLater()
    grid.deleteLater()
    widg, grid = None, None
    widg = QtGui.QWidget()
    main_window.setCentralWidget(widg)
    main_window.resize(640,640)
    widg.setContentsMargins(12, 12, 12, 12)
    grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
    grid.setContentsMargins(0, 0, 0, 0)
    widg.setLayout(grid)
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
    positions = [(r, c) for r in range(grid_rows)
                 for c in range(grid_cols)]
    for i in range(new_count):
        scope_modules[i].index = i;
        grid.addWidget(scope_modules[i], positions[
                       i][0], positions[i][1], 1, 1)
    # set row/column stretch to ensure same sizes (!!! THIS HAS ISSUES)
    for i in range(grid_rows):
        grid.setRowStretch(i,1)
    for i in range(grid_cols):
        grid.setColumnStretch(i,1)
    # update scope count
    scope_count = new_count
    status_bar.showMessage('Scope grid resized to ' +
                           str(grid_rows) + ' x ' + str(grid_cols))


##################################
# OPEN/SAVE VARIABLES / FUNCTIONS
##################################

filepath = None

def open_new_instance():
    CREATE_NO_WINDOW = 0x08000000
    subprocess.Popen([sys.executable, 'MELScope.pyw'],
                     creationflags=CREATE_NO_WINDOW)
# https://stackoverflow.com/questions/89228/calling-an-external-command-in-python#2251026
# https://msdn.microsoft.com/en-us/library/windows/desktop/ms684863(v=vs.85).aspx


def open():
    global filepath
    filepath = QtGui.QFileDialog.getOpenFileName(
        widg, 'Open MELScope', "", 'Scope Files (*.scope *.yaml)')
    if filepath:
        stream = file(filepath, 'r')
        config = yaml.load(stream)
        deploy_config(config)
        reload_all()
        set_theme()
        filename = str(filepath[str(filepath).rfind('/') + 1:])
        filename = filename[0 : filename.rfind('.')]
        status_bar.showMessage('Opened <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')


def save():
    global filepath
    if filepath:
        config = generate_config()
        stream = file(filepath, 'w')
        yaml.dump(config, stream)
        filename = str(filepath[str(filepath).rfind('/') + 1:])
        filename = filename[0 : filename.rfind('.')]
        status_bar.showMessage('Saved <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')
    else:
        save_as()


def save_as():
    global filepath
    new_filepath = QtGui.QFileDialog.getSaveFileName(
        widg, 'Save MELScope', "", 'Scope Files (*.scope *.yaml)')
    if new_filepath:
        filepath = new_filepath
        save()


def deploy_config(config):
    global theme, melshare_name, melshare_size
    global curve_modes, curve_names, curve_sizes, curve_colors, curve_lines
    global grid_rows, grid_cols
    global scope_titles, scope_modes, scope_filters, scope_legends, scope_ranges
    theme = config['theme']
    melshare_name = config['melshare_name']
    melshare_size = config['melshare_size']
    curve_names = config['curve_names']
    curve_modes = config['curve_modes']
    curve_sizes = config['curve_sizes']
    curve_colors = config['curve_colors']
    curve_lines = config['curve_lines']
    grid_rows = config['grid_rows']
    grid_cols = config['grid_cols']
    scope_titles = config['scope_titles']
    scope_modes = config['scope_modes']
    scope_filters = config['scope_filters']
    scope_legends = config['scope_legends']
    scope_ranges = config['scope_ranges']



def generate_config():
    config = {
        'theme': theme,
        'melshare_name': melshare_name,
        'melshare_size': melshare_size,
        'curve_names' : curve_names,
        'curve_modes': curve_modes,
        'curve_sizes' : curve_sizes,
        'curve_colors' : curve_colors,
        'curve_lines' : curve_lines,
        'grid_rows': grid_rows,
        'grid_cols': grid_cols,
        'scope_titles': scope_titles,
        'scope_modes': scope_modes,
        'scope_filters': scope_filters,
        'scope_legends': scope_legends,
        'scope_ranges': scope_ranges }
    return config




##################
# DIALOG CLASSES
##################

class ConfigureDataDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigureDataDialog, self).__init__(parent)
        self.setWindowTitle('Configure Data')
        layout = QtGui.QGridLayout(self)

        index_label = QtGui.QLabel(self)
        index_label.setText('i')
        index_label.setFont(bold_font)
        index_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(index_label,0,0)

        name_label = QtGui.QLabel(self)
        name_label.setText('Name')
        name_label.setFont(bold_font)
        layout.addWidget(name_label,0,1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(bold_font)
        layout.addWidget(mode_label,0,2)

        size_label = QtGui.QLabel(self)
        size_label.setText('Weight')
        size_label.setFont(bold_font)
        layout.addWidget(size_label,0,3)

        color_label = QtGui.QLabel(self)
        color_label.setText('Color')
        color_label.setFont(bold_font)
        color_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(color_label,0,4)

        line_label = QtGui.QLabel(self)
        line_label.setText('Line Style')
        line_label.setFont(bold_font)
        layout.addWidget(line_label,0,5)

        self.name_line_edits = []
        self.mode_combo_boxes = []
        self.size_spin_boxes = []
        self.color_buttons = []
        self.line_combo_boxes = []

        for i in range(melshare_size):
            new_index_label = QtGui.QLabel(self)
            new_index_label.setText(str(i))
            new_index_label.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_index_label,i+1,0)

            new_line_edit = QtGui.QLineEdit(self)
            new_line_edit.setText(curve_names[i])
            self.name_line_edits.append(new_line_edit)
            layout.addWidget(new_line_edit,i+1,1)

            new_mode_combo_box = QtGui.QComboBox(self)
            new_mode_combo_box.addItems(curve_mode_options)
            new_mode_combo_box.setCurrentIndex(curve_mode_options.index(curve_modes[i]))
            self.mode_combo_boxes.append(new_mode_combo_box)
            layout.addWidget(new_mode_combo_box,i+1,2)

            new_spin_box = QtGui.QSpinBox(self)
            new_spin_box.setValue(int(curve_sizes[i]))
            new_spin_box.setAlignment(QtCore.Qt.AlignCenter)
            new_spin_box.setRange(1,10)
            self.size_spin_boxes.append(new_spin_box)
            layout.addWidget(new_spin_box,i+1,3)

            c = curve_colors[i]
            curve_color = QtGui.QColor(c[0],c[1],c[2])

            new_color_button = QtGui.QPushButton(self)
            new_color_button.setMaximumHeight(15*resolution_scale)
            new_color_button.setMaximumWidth(30*resolution_scale)
            new_color_button.setStyleSheet(' background-color: %s' % curve_color.name() )
            new_color_button.clicked.connect(self.prompt_color_dialog_factory(i))
            self.color_buttons.append(new_color_button)
            layout.addWidget(new_color_button,i+1,4)

            new_line_combo_box = QtGui.QComboBox(self)
            new_line_combo_box.addItems(curve_line_options.keys())
            new_line_combo_box.setCurrentIndex(curve_line_options.keys().index(curve_lines[i]))
            self.line_combo_boxes.append(new_line_combo_box)
            layout.addWidget(new_line_combo_box,i+1,5)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        layout.addWidget(self.buttons, melshare_size + 1, 0, 1, 6)

        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    def prompt_color_dialog_factory(self, index):
        return lambda : self.prompt_color_dialog(index)

    def prompt_color_dialog(self, index):
        current_color = self.color_buttons[index].palette().color(QtGui.QPalette.Background)
        selected_color = QtGui.QColorDialog.getColor(current_color, self)
        if selected_color.isValid():
            self.color_buttons[index].setStyleSheet("background-color: %s" % selected_color.name())

    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def open_dialog(parent=None):
        global curve_names, curve_modes, curve_sizes, curve_colors, curve_lines
        dialog = ConfigureDataDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            for i in range(melshare_size):
                curve_names[i] = str(dialog.name_line_edits[i].text())
                curve_modes[i] = str(dialog.mode_combo_boxes[i].currentText())
                curve_sizes[i] = int(dialog.size_spin_boxes[i].value())
                color = dialog.color_buttons[i].palette().color(QtGui.QPalette.Background)
                curve_colors[i] = [color.red(),color.green(),color.blue()]
                curve_lines[i] = str(dialog.line_combo_boxes[i].currentText())
            refresh_scopes()

class ConfigureModulesDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigureModulesDialog, self).__init__(parent)
        self.setWindowTitle('Configure Module(s)')
        layout = QtGui.QGridLayout(self)

        filter_label = QtGui.QLabel(self)
        filter_label.setText(melshare_name)
        filter_label.setFont(bold_font)
        filter_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(filter_label,0,6,1,melshare_size)

        range_label = QtGui.QLabel(self)
        range_label.setText('Scope Options')
        range_label.setFont(bold_font)
        range_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(range_label, 0, 3, 1, 4)

        index_label = QtGui.QLabel(self)
        index_label.setText('i')
        index_label.setFont(bold_font)
        index_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(index_label,1,0)

        title_label = QtGui.QLabel(self)
        title_label.setText('Module Name')
        title_label.setFont(bold_font)
        layout.addWidget(title_label, 1, 1)

        mode_label = QtGui.QLabel(self)
        mode_label.setText('Mode')
        mode_label.setFont(bold_font)
        layout.addWidget(mode_label,1,2)

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

        for i in range(melshare_size):
            c = curve_colors[i]
            curve_color = QtGui.QColor(c[0],c[1],c[2])
            new_curve_label = QtGui.QLabel(self)

            curve_label_font = QtGui.QFont()
            curve_label_font.setBold(True)
            curve_label_font.setPointSize(8)

            new_curve_label.setText(str(i))
            new_curve_label.setFont(curve_label_font)
            new_curve_label.setStyleSheet(' color: white; background-color: %s' % curve_color.name() )

            new_curve_label.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_curve_label, 1, i + 6)

        for i in range(scope_count):

            new_index_label = QtGui.QLabel(self)
            new_index_label.setText(str(i))
            new_index_label.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_index_label,i+2,0)

            new_line_edit = QtGui.QLineEdit(self)
            new_line_edit.setText(scope_titles[i])
            new_line_edit.setMinimumWidth(200*resolution_scale)
            self.title_line_edits.append(new_line_edit)
            layout.addWidget(new_line_edit, i + 2, 1)

            new_mode_combo_box = QtGui.QComboBox(self)
            new_mode_combo_box.addItems(scope_mode_options)
            new_mode_combo_box.setCurrentIndex(scope_mode_options.index(scope_modes[i]))
            self.mode_combo_boxes.append(new_mode_combo_box)
            layout.addWidget(new_mode_combo_box,i+2,2)

            new_legend_checkbox =  QtGui.QCheckBox(self)
            new_legend_checkbox.setChecked(scope_legends[i])
            new_legend_checkbox.setFixedWidth(25*resolution_scale)
            self.legend_checkboxes.append(new_legend_checkbox)
            layout.addWidget(new_legend_checkbox, i + 2, 3)

            new_min_line_edit = QtGui.QLineEdit(self)
            new_min_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_min_line_edit.setText(str(scope_ranges[i][0]))
            new_min_line_edit.setFixedWidth(50*resolution_scale)
            self.min_line_edits.append(new_min_line_edit)
            layout.addWidget(new_min_line_edit,i+2,4)

            new_max_line_edit = QtGui.QLineEdit(self)
            new_max_line_edit.setValidator(QtGui.QDoubleValidator(self))
            new_max_line_edit.setText(str(scope_ranges[i][1]))
            new_max_line_edit.setFixedWidth(50*resolution_scale)
            self.max_line_edits.append(new_max_line_edit)
            layout.addWidget(new_max_line_edit,i+2,5)

            self.filter_checkboxes.append([])
            for j in range(melshare_size):
                new_check_box = QtGui.QCheckBox(self)
                new_check_box.setChecked(scope_filters[i][j])
                self.filter_checkboxes[i].append(new_check_box)
                layout.addWidget(new_check_box, i + 2, j + 6)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        layout.addWidget(self.buttons, len(scope_modules) + 2, 0, 1, melshare_size + 6)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def open_dialog(parent=None):
        global scope_titles, scope_modes, scope_filters, scope_legends, scope_ranges
        dialog = ConfigureModulesDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            scope_filters = [[i.isChecked() for i in row] for row in dialog.filter_checkboxes]
            scope_titles = [str(i.text()) for i in dialog.title_line_edits]
            scope_modes = [str(i.currentText()) for i in dialog.mode_combo_boxes]
            scope_legends = [i.isChecked() for i in dialog.legend_checkboxes]
            scope_ranges = [list(pair) for pair in
                            zip([float(i.text()) for i in dialog.min_line_edits],
                                [float(i.text()) for i in dialog.max_line_edits] ) ]
            refresh_scopes()

###################
# ACTION FUNCTIONS
###################

def prompt_configure_data():
    if melshare_connected:
        ConfigureDataDialog.open_dialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a MELShare before configuring!')
        msgBox.setWindowTitle('Configure Data')
        msgBox.exec_()

def prompt_configure_modules():
    if melshare_connected:
        ConfigureModulesDialog.open_dialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a MELShare before configuring!')
        msgBox.setWindowTitle('Configure Module(s)')
        msgBox.exec_()


def prompt_connect_melshare():
    global melshare_name
    input, ok = QtGui.QInputDialog.getText(
        widg, 'MELShare', 'Enter the MELShare Name:',QtGui.QLineEdit.Normal,melshare_name)
    if ok:
        melshare_name = str(input)
        reload_all()

def prompt_resize_grid():
    global grid_rows, grid_cols, scope_count
    i = grid_options.index((grid_rows, grid_cols))
    selection, ok = QtGui.QInputDialog.getItem(
        widg, 'Grid Size', 'Select Grid Size:', grid_options_display, i, False)
    if ok:
        grid_rows, grid_cols = grid_options_dict[str(selection)]
        reload_grid()

def about():
    msg = QtGui.QMessageBox()
    msg.setWindowTitle('About')
    msg.setText('MAHI Lab - Rice University\n'+
        'Author: Evan Pezent\n'
        'Email: epezent@rice.edu\n'+
        'Website: evanpezent.com')
    msg.exec_()

def open_github():
    webbrowser.open('https://github.com/epezent/MEL')

def prompt_change_theme():
    global theme, theme_options
    selection, ok = QtGui.QInputDialog.getItem(widg, 'Theme', 'Select Theme:', theme_options, theme_options.index(theme), False)
    if ok:
        theme = str(selection)
        set_theme()


def reload_all():
    connect_melshare()
    reset_samples()
    validate_curve_properties()
    resize_grid()
    validate_scope_properties()
    refresh_scopes()

def reload_grid():
    resize_grid()
    validate_scope_properties()
    refresh_scopes()

#################
# MENU BAR SETUP
#################

menu_bar = main_window.menuBar()

file_menu = menu_bar.addMenu('&File')
edit_menu = menu_bar.addMenu('&Edit')
pref_menu = menu_bar.addMenu('&Preferences')
help_menu = menu_bar.addMenu('&Help')

new_action = QtGui.QAction('&New', main_window,
    shortcut='Ctrl+N', statusTip='Create a new scope', triggered=open_new_instance)
file_menu.addAction(new_action)

open_action = QtGui.QAction('&Open...', main_window,
    shortcut='Ctrl+O', statusTip='Open an existing scope', triggered=open)
file_menu.addAction(open_action)

save_action = QtGui.QAction('&Save', main_window,
    shortcut='Ctrl+S', statusTip='Save the current scope', triggered=save)
file_menu.addAction(save_action)

save_as_action = QtGui.QAction('Save &As...', main_window,
    shortcut='Ctrl+Shift+S', statusTip='Save the current scope under a new name', triggered=save_as)
file_menu.addAction(save_as_action)

reload_action = QtGui.QAction('&Reload', main_window,
    shortcut='Ctrl+R', statusTip='Reload the current scope', triggered=reload_all)
file_menu.addAction(reload_action)

connect_action = QtGui.QAction('Connect &MELShare...', main_window,
    shortcut='Ctrl+M', statusTip='Connect to a MELShare map', triggered=prompt_connect_melshare)
edit_menu.addAction(connect_action)

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
    if melshare_connected:
        config_curves_action.setDisabled(False)
        configure_modules_action.setDisabled(False)
    else:
        config_curves_action.setDisabled(True)
        configure_modules_action.setDisabled(True)

####################################
# MAIN APPLICATION EXECUTION / LOOP
####################################

set_theme()
reload_grid()

def main_loop():
    global melshare_connected
    update_time()
    update_FPS()
    update_menu()
    if melshare_connected:
        if sample_data():
            for scope in scope_modules:
                scope.update()
        else:
            melshare_connected = False
            status_bar.showMessage('Lost connection to MELShare <' + melshare_name + '>')

# start the main_loop_timer
main_loop_timer = QtCore.QTimer()
main_loop_timer.timeout.connect(main_loop)
main_loop_timer.start(1000 / fps_target)

# show the main window
main_window.show()

# start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
    #multiprocessing.freeze_support()
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

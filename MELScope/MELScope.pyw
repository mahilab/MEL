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
import collections

# TO DO:
# Finish configure with Qt Line styles

#######################
# MEL SCOPE ABOUT INFO
#######################
ver = '1.2'

########################################
# PYQT APP / MAIN WINDOW / LAYOUT SETUP
########################################

app = QtGui.QApplication([])

main_window = QtGui.QMainWindow()
main_window.setWindowTitle('MELScope (untitled)')

screen_resolution = app.desktop().screenGeometry()
resolution_scale = screen_resolution.width() / 1920.0
#main_window.setFixedSize(400*resolution_scale, 300*resolution_scale)

pyqtgraph.setConfigOption('background', (240, 240, 240))
pyqtgraph.setConfigOption('foreground', 'k')

widg = QtGui.QWidget()
main_window.setCentralWidget(widg)
grid = QtGui.QGridLayout()  # http://zetcode.com/gui/pyqt4/layoutmanagement/
grid.setContentsMargins(0, 0, 0, 0)
widg.setLayout(grid)

bold_font = QtGui.QFont()
bold_font.setBold(True)

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
melshare_dataa_names = []
double_array = ctypes.c_double * melshare_size
melshare_buffer = double_array()
connected = False


def connect_melshare():
    global melshare_name, melshare_size, melshare_buffer, double_array, connected
    result = melshare.get_map_size(melshare_name)
    if result >= 0:
        melshare_size = result
        connected = True
        double_array = ctypes.c_double * melshare_size
        melshare_buffer = double_array()
        reset_samples()
        reset_curve_styles()
        for scope in scope_modules:
            scope.refresh()
        status_bar.showMessage('Connected to MELShare <' + melshare_name + '>')
    else:
        connected = False
        status_bar.showMessage(
            'Failed to connect to MELShare <' + melshare_name + '>')

######################################
# DATA SAMPLING VARIABLES / FUNCTIONS
######################################

sample_duration = 10
sampled_times = None
sampled_data = []

def reset_samples():
    global num_samples, sampled_times, sampled_data
    num_samples = int(sample_duration * fps_target * 1.1)
    sampled_times = None
    sampled_data = []
    sampled_times = np.zeros(num_samples)
    melshare.read_double_map(melshare_name, ctypes.byref(melshare_buffer), melshare_size)
    for i in range(melshare_size):
        sampled_data.append(np.ones(num_samples) * melshare_buffer[i])

def sample_data():
    global delta_time
    result = melshare.read_double_map(melshare_name, ctypes.byref(melshare_buffer), melshare_size)
    if result >= 0:
        sampled_times[:-1] = sampled_times[1:] - delta_time
        sampled_times[-1] = 0
        for (data, i) in zip(sampled_data, range(melshare_size)):
            data[:-1] = data[1:]
            data[-1] = melshare_buffer[i]
        return True
    else:
        return False


############################
# CURVE STYLING SETUP
###########################

default_curve_size = 3

default_curve_colors = [(31, 120, 180), (227, 26, 28), (51, 160, 44),
                        (255, 127, 0), (106, 61, 154), (177, 89, 40),
                        (166, 206, 227), (251, 154, 153), (178, 223, 138),
                        (253, 191, 111), (202, 178, 214), (255, 255, 153)]

curve_line_options = collections.OrderedDict( [
    ('Solid'       , QtCore.Qt.SolidLine),
    ('Dash'        , QtCore.Qt.DashLine),
    ('Dot'         , QtCore.Qt.DotLine),
    ('Dash Dot'    , QtCore.Qt.DashDotLine),
    ('Dash Dot Dot', QtCore.Qt.DashDotDotLine) ] )

curve_names = []
curve_sizes = []
curve_colors = []
curve_lines = []

def reset_curve_styles():
    global curve_sizes, curve_colors, curve_lines
    for i in range(melshare_size):
        curve_names.append('Curve'+str(i))
        curve_sizes.append(default_curve_size)
        curve_colors.append(default_curve_colors[i])
        curve_lines.append('Solid')

######################################
# SCOPE CLASS / VARIABLES / FUNCTIONS
######################################

scope_modules = []

class ScopeModule(QtGui.QWidget):

    def __init__(self):
        super(ScopeModule, self).__init__()
        # scope title
        self.title = 'Double-click to rename scope'
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
        self.plot_item.setMenuEnabled(False)
        # scope module curves and data
        self.curves = []
        # scope title label
        self.label = QtGui.QLabel()
        self.label.setText(self.title)
        self.label.mouseDoubleClickEvent = self.initiate_rename
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setFont(bold_font)
        # scope module layout
        self.layout = QtGui.QGridLayout()
        self.setLayout(self.layout)
        self.layout.addWidget(self.label, 0, 0, 1, 1)
        self.layout.addWidget(self.plot_widget, 1, 0, 1, 1)
        self.filters = []

    def initiate_rename(self, event=None):
        self.line_edit = QtGui.QLineEdit()
        self.line_edit.setText(self.title)
        self.line_edit.setAlignment(QtCore.Qt.AlignCenter)
        self.line_edit.setFont(bold_font)
        self.line_edit.editingFinished.connect(self.confirm_rename)
        self.line_edit.focusOutEvent = self.confirm_rename
        self.layout.addWidget(self.line_edit, 0, 0, 1, 1)
        self.line_edit.selectAll()
        self.line_edit.setFocus()

    def confirm_rename(self, event=None):
        self.title = str(self.line_edit.text())
        self.label.setText(self.title)
        self.layout.removeWidget(self.line_edit)
        self.line_edit.deleteLater()
        self.line_edit = None

    def configure(self, title, filter):
        self.title = str(title)
        self.label.setText(title)
        self.filters = filter

    def refresh(self):
        self.plot_widget.clear()
        self.curves = []
        for i in range(melshare_size):
            new_pen = pyqtgraph.mkPen(curve_colors[i], style=curve_line_options[curve_lines[i]], width=curve_sizes[i])
            new_curve = pyqtgraph.PlotCurveItem(pen=new_pen)
            self.plot_widget.addItem(new_curve)
            self.curves.append(new_curve)
        if self.filters == [] or len(self.filters) != melshare_size:
            self.reset_filter()

    def reset_filter(self):
        self.filters = [True for i in range(melshare_size)]

    def update(self):
        global delta_time, elapsed_time, sampled_data
        for curve, filter, data in zip(self.curves, self.filters, sampled_data):
            if filter:
                curve.setData(sampled_times, data)

###############################
# GRID VARIABLES / FUNCTIONS
##############################

options = [(1, 1), (1, 2), (1, 3),
           (2, 1), (2, 2), (2, 3),
           (3, 1), (3, 2), (3, 3)]

options_display = [str(x[0]) + ' x ' + str(x[1]) for x in options]

options_dict = dict(zip(options_display, options))

grid_rows = 1
grid_cols = 1
scope_count = 1


def resize_grid():
    global grid_rows, grid_cols, scope_count
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
    positions = [(r, c) for r in range(grid_rows)
                 for c in range(grid_cols)]
    for i in range(new_count):
        grid.addWidget(scope_modules[i], positions[
                       i][0], positions[i][1], 1, 1)
    # clear the scopes
    for scope in scope_modules:
        scope.refresh()
    # update scope count
    scope_count = new_count
    # resize window
    #main_window.setFixedSize(400 * grid_cols * resolution_scale, 300 * grid_rows * resolution_scale)
    status_bar.showMessage('')
    status_bar.showMessage('Scope grid resized to ' +
                           str(grid_rows) + ' x ' + str(grid_cols))

##################################
# OPEN/SAVE VARIABLES / FUNCTIONS
##################################

config = {}
filepath = None
up_to_date = False


def open_new_instance():
    CREATE_NO_WINDOW = 0x08000000
    subprocess.Popen([sys.executable, 'MELScope.pyw'],
                     creationflags=CREATE_NO_WINDOW)
# https://stackoverflow.com/questions/89228/calling-an-external-command-in-python#2251026
# https://msdn.microsoft.com/en-us/library/windows/desktop/ms684863(v=vs.85).aspx


def open():
    global config, filepath
    filepath = QtGui.QFileDialog.getOpenFileName(
        widg, 'Open MELScope', "", 'Scope Files (*.scope *.yaml)')
    if filepath:
        stream = file(filepath, 'r')
        config = yaml.load(stream)
        deploy_config()
        filename = str(filepath[str(filepath).rfind('/') + 1:])
        filename = filename[0 : filename.rfind('.')]
        status_bar.showMessage('Opened <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')


def save():
    global config, filepath
    if filepath:
        generate_config()
        stream = file(filepath, 'w')
        yaml.dump(config, stream)
        filename = filepath[str(filepath).rfind('/') + 1:]
        filename = filename[0 : filename.rfind('.') + 1]
        status_bar.showMessage('Saved <' + filename + '>')
        main_window.setWindowTitle('MELScope (' + filename + ')')
    else:
        save_as()


def save_as():
    global config, filepath
    new_filepath = QtGui.QFileDialog.getSaveFileName(
        widg, 'Save MELScope', "", 'Scope Files (*.scope *.yaml)')
    if new_filepath:
        filepath = new_filepath
        save()


def deploy_config():
    global config, melshare_name, melshare_size, grid_rows, grid_cols
    melshare_name = config['melshare_name']
    melshare_size = config['melshare_size']
    grid_rows = config['grid_rows']
    grid_cols = config['grid_cols']
    titles = config['scope_titles']
    filters = config['scope_filters']
    connect_melshare()
    resize_grid()

    for (scope, title, filter) in zip(scope_modules, titles, filters):
        scope.configure(title,filter)

def generate_config():
    global config
    config = {
        'melshare_name': melshare_name,
        'melshare_size': melshare_size,
        'grid_rows': grid_rows,
        'grid_cols': grid_cols,
        'scope_titles': [scope.title for scope in scope_modules],
        'scope_filters':[scope.filters for scope in scope_modules] }


def open_github():
    webbrowser.open('https://github.com/epezent/MEL')

##################
# DIALOG(S) SETUP
##################

class ConfigCurvesDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigCurvesDialog, self).__init__(parent)
        self.setWindowTitle('Configure Curve(s)')
        layout = QtGui.QGridLayout(self)

        index_label = QtGui.QLabel(self)
        index_label.setText('i')
        index_label.setFont(bold_font)
        index_label.setAlignment(QtCore.Qt.AlignCenter)

        layout.addWidget(index_label,0,0)

        name_label = QtGui.QLabel(self)
        name_label.setText('Curve Name')
        name_label.setFont(bold_font)
        layout.addWidget(name_label,0,1)

        color_label = QtGui.QLabel(self)
        color_label.setText('Color')
        color_label.setFont(bold_font)
        color_label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(color_label,0,2)

        line_label = QtGui.QLabel(self)
        line_label.setText('Style')
        line_label.setFont(bold_font)
        layout.addWidget(line_label,0,3)

        self.name_line_edits = []
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

            new_color_button = QtGui.QPushButton(self)
            new_color_button.setMaximumHeight(15*resolution_scale)
            new_color_button.setMaximumWidth(30*resolution_scale)
            new_color_button.setStyleSheet("background-color:rgb(" +
                str(curve_colors[i][0]) + ',' +
                str(curve_colors[i][1]) + ',' +
                str(curve_colors[i][2]) + ')' )
            new_color_button.clicked.connect(self.prompt_color_dialog_factory(i))
            self.color_buttons.append(new_color_button)
            layout.addWidget(new_color_button,i+1,2)

            new_combo_box = QtGui.QComboBox(self)
            new_combo_box.addItems(curve_line_options.keys())
            new_combo_box.setCurrentIndex(curve_line_options.keys().index(curve_lines[i]))
            self.line_combo_boxes.append(new_combo_box)
            layout.addWidget(new_combo_box,i+1,3)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        layout.addWidget(self.buttons, melshare_size + 1, 0, 1, 4)

        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    def prompt_color_dialog_factory(self, index):
        return lambda : self.prompt_color_dialog(index)

    def prompt_color_dialog(self, index):
        current_color = self.color_buttons[index].palette().color(QtGui.QPalette.Background)
        selected_color = QtGui.QColorDialog.getColor(current_color, self)
        self.color_buttons[index].setStyleSheet("QWidget { background-color: %s}" % selected_color.name())


    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def openDialog(parent=None):
        global curve_names, curve_sizes, curve_colors, curve_lines
        dialog = ConfigCurvesDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            for i in range(melshare_size):
                curve_names[i] = str(dialog.name_line_edits[i].text())
                color = dialog.color_buttons[i].palette().color(QtGui.QPalette.Background)
                curve_colors[i] = (color.red(),color.green(),color.blue())
                curve_lines[i] = str(dialog.line_combo_boxes[i].currentText())
            for scope in scope_modules:
                scope.refresh()

class ConfigScopesDialog(QtGui.QDialog):
    def __init__(self, parent=None):
        super(ConfigScopesDialog, self).__init__(parent)
        self.setWindowTitle('Configure Scope(s)')
        layout = QtGui.QGridLayout(self)

        header = QtGui.QLabel(self)
        header.setText('Scope Name')
        header.setFont(bold_font)
        layout.addWidget(header, 0, 0)

        self.checkbox_table = []

        for i in range(melshare_size):
            new_header = QtGui.QLabel(self)
            new_header.setText(str(i))
            new_header.setFont(bold_font)
            new_header.setAlignment(QtCore.Qt.AlignCenter)
            layout.addWidget(new_header, 0, i + 1)

        for scope, i in zip(scope_modules, range(len(scope_modules))):
            new_label = QtGui.QLabel(self)
            new_label.setText(scope.title)
            layout.addWidget(new_label, i + 1, 0)
            self.checkbox_table.append([])
            for j in range(melshare_size):
                new_check_box = QtGui.QCheckBox(self)
                new_check_box.setChecked(scope.filters[j])
                self.checkbox_table[i].append(new_check_box)
                layout.addWidget(new_check_box, i + 1, j + 1)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        layout.addWidget(self.buttons, len(scope_modules) +
                         1, 0, 1, melshare_size + 1)

        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    # get current date and time from the dialog
    def evaluate_checkbox_table(self):
        self.truth_table = []
        for row, i in zip(self.checkbox_table, range(len(self.checkbox_table))):
            self.truth_table.append([])
            for check_box in row:
                self.truth_table[i].append(check_box.isChecked())

    # static method to create the dialog and return (date, time, accepted)
    @staticmethod
    def openDialog(parent=None):
        dialog = ConfigScopesDialog(parent)
        result = dialog.exec_()
        if result == QtGui.QDialog.Accepted:
            dialog.evaluate_checkbox_table()
            for scope, row in zip(scope_modules, dialog.truth_table):
                if scope.filters !=  row:
                    scope.filters = row
                    scope.refresh()


def prompt_configure_curves():
    if connected:
        ConfigCurvesDialog.openDialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a MELShare before configuring!')
        msgBox.setWindowTitle('Configure Curve(s)')
        msgBox.exec_()

def prompt_configure_scopes():
    if connected:
        ConfigScopesDialog.openDialog()
    else:
        msgBox = QtGui.QMessageBox(main_window)
        msgBox.setText('Connect to a MELShare before configuring!')
        msgBox.setWindowTitle('Configure Scope(s)')
        msgBox.exec_()


def prompt_connect_melshare():
    global melshare_name
    input, ok = QtGui.QInputDialog.getText(
        widg, 'MELShare', 'Enter the MELShare Name:',QtGui.QLineEdit.Normal,melshare_name)
    if ok:
        melshare_name = str(input)
        connect_melshare()


def prompt_resize_grid():
    global grid_rows, grid_cols, scope_count
    i = options.index((grid_rows, grid_cols))
    selection, ok = QtGui.QInputDialog.getItem(
        widg, 'Grid Size', 'Select Grid Size:', options_display, i, False)
    if ok:
        grid_rows, grid_cols = options_dict[str(selection)]
        resize_grid()


#################
# MENU BAR SETUP
#################

menu_bar = main_window.menuBar()

file_menu = menu_bar.addMenu('File')
edit_menu = menu_bar.addMenu('Edit')
help_menu = menu_bar.addMenu('Help')

new_action = QtGui.QAction('New', main_window)
new_action.setShortcut('Ctrl+N')
new_action.triggered.connect(open_new_instance)
file_menu.addAction(new_action)

open_action = QtGui.QAction('Open...', main_window)
open_action.setShortcut('Ctrl+O')
open_action.triggered.connect(open)
file_menu.addAction(open_action)

save_action = QtGui.QAction('Save', main_window)
save_action.setShortcut('Ctrl+S')
save_action.triggered.connect(save)
file_menu.addAction(save_action)

save_as_action = QtGui.QAction('Save As...', main_window)
save_as_action.setShortcut('Ctrl+Shift+S')
save_as_action.triggered.connect(save_as)
file_menu.addAction(save_as_action)

connect_action = QtGui.QAction('Connect MELShare...', main_window)
connect_action.setShortcut('Ctrl+C')
connect_action.triggered.connect(prompt_connect_melshare)
edit_menu.addAction(connect_action)

grid_action = QtGui.QAction('Resize Scope Grid...', main_window)
grid_action.setShortcut('Ctrl+G')
grid_action.triggered.connect(prompt_resize_grid)
edit_menu.addAction(grid_action)

config_curves_action = QtGui.QAction('Configure Curve(s)...', main_window)
config_curves_action.setShortcut('Ctrl+D')
config_curves_action.triggered.connect(prompt_configure_curves)
config_curves_action.setDisabled(True)
edit_menu.addAction(config_curves_action)

configure_scopes_action = QtGui.QAction('Configure Scope(s)...', main_window)
configure_scopes_action.setShortcut('Ctrl+F')
configure_scopes_action.triggered.connect(prompt_configure_scopes)
configure_scopes_action.setDisabled(True)
edit_menu.addAction(configure_scopes_action)

about_action = QtGui.QAction('About...', main_window)
about_action.setShortcut('F11')
help_menu.addAction(about_action)

github_action = QtGui.QAction('GitHub...', main_window)
github_action.setShortcut('F12')
github_action.triggered.connect(open_github)
help_menu.addAction(github_action)

####################################
# MAIN APPLICATION EXECUTION / LOOP
####################################

scope_modules.append(ScopeModule())
grid.addWidget(scope_modules[0], 0, 0, 1, 1)

def main_loop():
    global connected, config_curves_action, configure_scopes_action
    update_time()
    update_FPS()
    if connected:
        config_curves_action.setDisabled(False)
        configure_scopes_action.setDisabled(False)
        if sample_data():
            for scope in scope_modules:
                scope.update()
        else:
            connected = False
            status_bar.showMessage('Lost connection to MELShare <' + melshare_name + '>')
    else:
        config_curves_action.setDisabled(True)
        configure_scopes_action.setDisabled(True)

# start the plot
timer = QtCore.QTimer()
timer.timeout.connect(main_loop)
timer.start(1000 / fps_target)

# show the main window
main_window.show()

# start Qt event loop unless running in interactive mode or using pyside.
if __name__ == '__main__':
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

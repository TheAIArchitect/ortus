import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QDialog, QProgressBar, QLabel, QHBoxLayout, QSlider
from PyQt5.QtCore import pyqtSlot, Qt, pyqtSignal

class ProgressBar_Dialog(QDialog):
    def __init__(self):
        super(ProgressBar_Dialog ,self).__init__()
        self.init_ui()

    def init_ui(self):
        # Creating a label
        self.progressLabel = QLabel('Progress Bar:', self)

        # Creating a progress bar and setting the value limits
        self.progressBar = QProgressBar(self)
        self.progressBar.setMaximum(100)
        self.progressBar.setMinimum(0)

        # Creating a Horizontal Layout to add all the widgets
        self.hboxLayout = QHBoxLayout(self)

        # Adding the widgets
        self.hboxLayout.addWidget(self.progressLabel)
        self.hboxLayout.addWidget(self.progressBar)

        # Setting the hBoxLayout as the main layout
        self.setLayout(self.hboxLayout)
        self.setWindowTitle('Dialog with Progressbar')

        self.show()

    def make_connection(self, slider_object):
        slider_object.changedValue.connect(self.get_slider_value)

    @pyqtSlot(int)
    def get_slider_value(self, val):
        self.progressBar.setValue(val)


class Slider_Dialog(QDialog):

    # Added a signal
    changedValue = pyqtSignal(int)
    ''' for a string: '''
    # changedValue = pyqtSignal('QString')

    def __init__(self):
        super(Slider_Dialog, self).__init__()
        self.init_ui()

    def init_ui(self):
        # Creating a label
        self.sliderLabel = QLabel('Slider:', self)

        # Creating a slider and setting its maximum and minimum value
        self.slider = QSlider(self)
        self.slider.setMinimum(0)
        self.slider.setMaximum(100)
        self.slider.setOrientation(Qt.Horizontal)

        # Creating a horizontalBoxLayout
        self.hboxLayout = QHBoxLayout(self)

        # Adding the widgets
        self.hboxLayout.addWidget(self.sliderLabel)
        self.hboxLayout.addWidget(self.slider)

        # Setting main layout
        self.setLayout(self.hboxLayout)

        # Setting a connection between slider position change and on_changed_value function
        self.slider.valueChanged.connect(self.on_changed_value)

        self.setWindowTitle("Dialog with a Slider")
        self.show()

    def on_changed_value(self, value):
        self.changedValue.emit(value)

'''
Progress bar:
    - function, "make_connection(self, slider_object)",
which gets called by 'main', with the slider object as a parameter.
        => function also calls slider_object.changedValue.connect(self.get_slider_value)
    - It also has a decorator, denoted with "@pyqtSlot(int)", 
function "get_slider_value(self, val),  that *sets* the value of the progress bar.

Slider:
    - has a signal 'changedValue = pyqtSignal(int)'
        => should by pyqtSignal('QString') for a string signal
    - sets slider.valueChanged.connect(self.on_changed_value)
    - function, 'on_changed_value(self, value)', which calls self. changedValue.emit(value)
'''
if __name__ == '__main__':
    app = QApplication(sys.argv)
    sd = Slider_Dialog()
    pb = ProgressBar_Dialog()
    # Making the connection
    pb.make_connection(sd)
    sys.exit(app.exec_())


import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import pyqtSlot, Qt, pyqtSignal

class NeuronTypeSelector(QWidget):

    neuronTypeSelected = pyqtSignal("QString", int)

    def __init__(me):
        super().__init__()
        me.grid = QGridLayout()
        me.setLayout(me.grid)
        me.neuronTypes = ["sensor", "inter", "motor"]
        ## add the buttons
        # sensor, checked, enabled (default)
        me.addButton(0, True)
        # inter, not checked (default), enabled (default)
        me.addButton(1)
        # motor, not checked, not enabled
        me.addButton(2, False, False)
       
    def addButton(me, index, isChecked = False, isEnabled = True):
        button = QRadioButton(me.neuronTypes[index])
        button.type = me.neuronTypes[index]
        button.id = index
        button.toggled.connect(me.onToggle)
        if isEnabled:
            button.setChecked(isChecked)
        else:
            button.setChecked(False)
            button.setEnabled(False)
        me.grid.addWidget(button, 0, index)

    @pyqtSlot()
    def onToggle(me):
        button = me.sender()
        if button.isChecked():
            me.neuronTypeSelected.emit(button.type, button.id)
            print("Selected %s -- %s" % (button.id, button.type))


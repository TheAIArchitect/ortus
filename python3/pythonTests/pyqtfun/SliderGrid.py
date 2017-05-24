#!/usr/bin/python3
# -*- coding: utf-8 -*-
from __future__ import division
"""
ZetCode PyQt5 tutorial 

In this example, we connect a signal
of a QSlider to a slot of a QLCDNumber. 

author: Jan Bodnar
website: zetcode.com 
last edited: January 2015
"""

import sys
from PyQt5.QtCore import Qt, pyqtSignal, QObject
from PyQt5.QtWidgets import (QWidget, QLCDNumber, QSlider, QLineEdit, QLabel, QGridLayout, QHBoxLayout, QVBoxLayout, QApplication)
from PyQt5.QtCore import pyqtSlot, Qt, pyqtSignal

from SliderUnit import SliderUnit


#class Communicate(QObject):
#    closeApp = pyqtSignal()

 
class Display(QWidget):
    
    def __init__(me):
        super().__init__()
        me.initUI()
    
    def updateLabel(me, value):
        me.sbl.setText(str(value))
        
    def initUI(me):
        
        grid = QGridLayout()
        me.setLayout(grid)

        
        sliderArray = []
        sliderArray.append(SliderUnit("a"))
        sliderBox = QVBoxLayout(me)
        for sliderUnit in sliderArray:
            sliderBox.addWidget(sliderUnit)
            me.sbl = QLabel(me)
            sliderBox.addWidget(me.sbl)
            sliderUnit.sliderValueLine
            #sliderUnit.sliderValueLine.textChanged.connect(me.onValueChanged)
            sliderUnit.valueChangeSignal.connect(me.updateLabel)
        sliderBox.addStretch(1)
        #me.addSlider(grid, [0,0])

        #me.addSlider(grid, [2,0])
        grid.addLayout(sliderBox,0,0) 
        
        me.setGeometry(500, 300, 400, 800)
        me.setWindowTitle('Signal & slot')
        me.show()




'''
class Example(QWidget):
    
    theSignal = pyqtSignal(int)

    def __init__(me):
        super().__init__()
        me.initUI()

    #def set_the_text(me, ):
    #    func
    
    @pyqtSlot(int)
    def get_slider_value(me, value):
        me.output.setText(str(value))

    def on_the_signal(me, value):
        me.theSignal.emit(value)

    def addSlider(me, grid, pos):
        me.slider = QSlider(Qt.Horizontal, me)
        #lcd = QLCDNumber(me)
        me.output = QLineEdit(me)
        me.output.setMaxLength(3)
        me.slider.valueChanged.connect(me.on_the_signal)
        me.theSignal.connect(me.get_slider_value)

        #slider.valueChanged.connect(output.setText)
        #slider.valueChanged.connect(
        grid.addWidget(me.slider, pos[0], pos[1])
        #grid.addWidget(me.output, pos[0], pos[1] + 1, 2, 1) # take up 2 'X' spaces, and 1 'Y' space.
        grid.addWidget(me.output, pos[0], pos[1] + 1)

        
    def initUI(me):
        
        #me.c = Communicate()
        #me.c.closeApp.connect(me.close)

        grid = QGridLayout()
        me.setLayout(grid)
        me.addSlider(grid, [0,0])
        #me.addSlider(grid, [2,0])

        
        me.setGeometry(300, 300, 250, 150)
        me.setWindowTitle('Signal & slot')
        me.show()
        
    def keyPressEvent(me, e):
        if e.key() == Qt.Key_Escape:
                me.close()

    def mousePressEvent(me, event):
        # mouse press causes the 'closeApp'signal to be emitted
        #me.c.closeApp.emit()
        pass
'''

if __name__ == '__main__':
    
    app = QApplication(sys.argv)
    #ex = Example()
    display = Display()
    sys.exit(app.exec_())

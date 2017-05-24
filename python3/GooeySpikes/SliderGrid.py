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
from PyQt5.QtWidgets import QSizePolicy, QMessageBox, QWidget, QPushButton
from PyQt5.QtGui import QIcon

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt


from SliderUnit import SliderUnit
from SimpleNeuronModel import SimpleNeuron


#class Communicate(QObject):
#    closeApp = pyqtSignal()


class PlotCanvas(FigureCanvas):
 
    def __init__(me, parent=None):
        inchesWidth = 25 
        inchesHeight = 17 
        me.fig = Figure(figsize=(inchesWidth,inchesHeight))
        FigureCanvas.__init__(me, me.fig)
        me.setParent(parent)
 
        FigureCanvas.setSizePolicy(me,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(me)
        me.topFirst = True
        me.bottomFirst = True
        #me.plot()

    def setLegend(me, topLineNames, bottomLineNames):
        me.topLineNames = topLineNames
        me.bottomLineNames = bottomLineNames

    ''' 
    xVals is the x data, dataArray is a list of y data arrays, which get looped through,
    all plotted against xVals
    
    plotTop creates the top subplot of a two subplot figure
    '''
    def plotTop(me, xVals, dataArray):
        lineNo = 0
        if me.topFirst:
            me.ax = me.fig.add_subplot(211)
            me.ax.set_ylabel("Potential (mV)")
            me.ax.grid(True)
            me.topLines = []
            for data in dataArray:
                print(lineNo)
                tempLine = me.ax.plot(xVals, data, label=me.topLineNames[lineNo])[0]
                me.topLines.append(tempLine)
                lineNo += 1
            me.topFirst = False
        else:
            for data in dataArray:
                me.topLines[lineNo].set_ydata(data)
                lineNo += 1
            me.draw()
            me.flush_events()
        me.ax.legend()



    ''' 
    xVals is the x data, dataArray is a list of y data arrays, which get looped through,
    all plotted against xVals
    
    plotBottom creates the bottom subplot of a two subplot figure
    '''
    def plotBottom(me, xVals, dataArray):
        lineNo = 0
        if me.bottomFirst:
            me.ax = me.fig.add_subplot(212)
            me.ax.set_xlabel("Time (ms)")
            me.ax.set_ylabel("Potential (mV)")
            me.ax.grid(True)
            me.bottomLines = []
            for data in dataArray:
                tempLine = me.ax.plot(xVals, data, label=me.bottomLineNames[lineNo])[0] # returns a tuple
                me.bottomLines.append(tempLine)
                lineNo += 1
            #me.ax.axhline(0, color='green', lw=2)
            me.bottomFirst = False
        else:
            lineNo = 0
            for data in dataArray:
                me.bottomLines[lineNo].set_ydata(data)
                lineNo += 1
            me.draw()
            me.flush_events()



class NetworkSimulator:

    def __init__(me, thePlotCanvas):
        me.numIterations = 1500
        #numIterations = 100
        me.touchStimulus = []
        touchDuration = 100
        touchOffDuration = 200
        touchCycle = touchDuration + touchOffDuration
        touchIntensity = 10 
        #touchIntensity = 5 
        touchOn = True 
        for i in range (0, me.numIterations):
            if (not touchOn and i % touchCycle == 0):
                touchOn = True
            elif (touchOn and i % touchDuration == 0):
                touchOn = False
            if touchOn:
                me.touchStimulus.append(touchIntensity)
            else:
                me.touchStimulus.append(0)
        me.sensors = []
        #numSensors = 3
        me.numSensors = 1
        me.numInters = 1
        for i in range(0, me.numSensors):
            me.sensors.append(SimpleNeuron("sensory"))
        me.inters = []
        for i in range(0, me.numInters):
            me.inters.append(SimpleNeuron("inter"))
        me.activationForInter = 0
        thePlotCanvas.setLegend(["sensory", "inter"], ["touch"])

    ''' sets same parameters to all types of neurons by default'''
    def updateParams(me, params, neuronType = 'all'):
        all = False
        if neuronType.__eq__("all"): 
            all = True
        if neuronType.__eq__("sensor") or all:
            for sensor in me.sensors:
                sensor.updateParameters(*params)
        if neuronType.__eq__("inter") or all:
            for inter in me.inters:
                inter.updateParameters(*params)

    def clearData(me):
        for sensor in me.sensors:
                sensor.clearData()
        for inter in me.inters:
                inter.clearData()



    def run(me):
        #print("Running simulation for "+str(me.numIterations)+" steps...")
        me.xVals = []
        for i in range(0,me.numIterations):
            for sensor in me.sensors:
                sensor.addInput(me.touchStimulus[i])
                sensor.update()
                if (sensor.fired()):
                    activationForInter = 25 
                    #activationForInter = 5 
                else:
                    activationForInter = 0
                me.inters[0].addInput(activationForInter)
            me.inters[0].update()
            me.xVals.append(i)
        
    def updatePlot(me, thePlotCanvas):
        #######################
        ##### PyQt5 Integration
        #######################
        ##
        '''
        nX = len(me.xVals)
        nS0 = len(me.sensors[0].activationHistory)
        nI0 = len(me.inters[0].activationHistory)
        nTS = len(me.touchStimulus)
        print("PLOT UPDATE: length of (xVals - {0}, sensors[0] - {1}, inters[0] - {2}, touchStimulus - {3}). numIterations: {4}".format( nX, nS0, nI0, nTS, me.numIterations))
        '''
        topData = []
        topData.append(me.sensors[0].activationHistory)
        topData.append(me.inters[0].activationHistory)
        #
        thePlotCanvas.plotTop(me.xVals, topData)
        ##
        bottomData = []
        bottomData.append(me.touchStimulus)
        #
        thePlotCanvas.plotBottom(me.xVals, bottomData)

 
class Display(QWidget):

    #paramEmissionSignal = pyqtSignal()

    
    def __init__(me):
        super().__init__()
        me.initUI()
    
    def updateLabel(me, value, paramId):
        me.paramEmissions[paramId] = value
        #me.paramEmissionSignal.emit()
        me.networkSim.clearData()
        me.networkSim.updateParams(me.paramEmissions, 'sensor')
        me.networkSim.run()
        me.networkSim.updatePlot(me.plotCanvas)
        me.sbl.setText("Changing parameter #"+str(paramId)+": "+str(value))
        
    def initUI(me):
        
        grid = QGridLayout()
        me.setLayout(grid)
        
        sliderBox = QVBoxLayout(me)
        sliderArray = []
        me.paramEmissions = []
        # parameter ID is its index
        # a: recovery time - 0.001 - 1.0 => default: 0.02
        # b: sensitivity of recoery variable u to subthreshold fluctuations of membrane potential v: .01 - 1 => default: .2
        # c: voltage reset: -25 - -100 => default: -65
        # d: after-spike reset of recovery variable u: 0 - 20 => default: 8
        parameters = ['a', 'b', 'c', 'd']
        parametersLow = [.001, .01, -100, 0]
        parametersHigh = [1, 1, -25, 20]
        parametersDefault = [.02, .2, -65, 8]
        #parametersDefaultInter = [.02, .2, -65, 8]
        #parametersDefaultSensory = [.02, .2, -55, 4]
        numParams = len(parameters)
        for i in range(0, numParams):
            sliderArray.append(SliderUnit(parameters[i],i,parametersLow[i], parametersHigh[i], parametersDefault[i]))
            me.paramEmissions.append(parametersDefault[i])
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
        
        ###############################
        ####### matplotlib plot #######
        ###############################

        me.plotCanvas = PlotCanvas()
        me.networkSim = NetworkSimulator(me.plotCanvas)
        me.networkSim.run()
        me.networkSim.updatePlot(me.plotCanvas)
        
        ##
        #me.plotCanvas.draw()
        grid.addWidget(me.plotCanvas, 0, 1)

        me.setGeometry(500, 300, 1400, 800)
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

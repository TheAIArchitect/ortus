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

from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt


from SliderUnit import SliderUnit
from NetworkSimulator import NetworkSimulator
from NeuronTypeRadioSelector import NeuronTypeRadioSelector


#class Communicate(QObject):
#    closeApp = pyqtSignal()


class SingleParameterBox(QWidget):

    # we want to send the float that was entered, 
    # as well as the id of this parameter
    paramChangedSignal = pyqtSignal(float, int)

    LINE_EDIT_WIDTH = 50
    LINE_EDIT_HEIGHT = 20

    def __init__(me, paramName, paramId, paramDefault):
        super().__init__()
        me.paramId = paramId
        me.paramName = paramName
        me.paramLabel = QLabel(me.paramName+":", me)
        me.paramValue = paramDefault
        me.initUI()

    def initUI(me):
        me.hBox = QHBoxLayout(me)
        me.setLayout(me.hBox)
        me.makeParamBox()

    def makeParamBox(me):
        # line edit box
        me.paramEditBox = QLineEdit(me)
        me.paramEditBox.setMaximumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.paramEditBox.setMinimumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.paramEditBox.setText(str(me.paramValue))
        # add both widgets
        me.hBox.addWidget(me.paramLabel)
        me.hBox.addWidget(me.paramEditBox)
        # connect signals
        me.paramEditBox.textChanged.connect(me.onParamChange)

    @pyqtSlot('QString')
    def onParamChange(me, value):
        try:
            floatValue = float(value) 
            me.paramChangedSignal.emit(floatValue, me.paramId)
        except ValueError:
            pass

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
                #print(lineNo)
                tempLine = me.ax.plot(xVals, data, label=me.topLineNames[lineNo])[0]
                me.topLines.append(tempLine)
                lineNo += 1
            me.ax.legend()
            me.topFirst = False
        else:
            for data in dataArray:
                me.topLines[lineNo].set_ydata(data)
                lineNo += 1
            me.draw()
            me.flush_events()



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
            me.ax.legend()
            me.bottomFirst = False
        else:
            lineNo = 0
            for data in dataArray:
                me.bottomLines[lineNo].set_ydata(data)
                lineNo += 1
            me.draw()
            me.flush_events()
 
class Display(QWidget):

    #paramEmissionSignal = pyqtSignal()
    saveSliderStateSignal = pyqtSignal('QString', 'QString')

    
    def __init__(me):
        super().__init__()
        me.initUI()
    
    def updateLabel(me, value, paramId):
        me.paramEmissions[paramId] = value
        #me.paramEmissionSignal.emit()
        me.networkSim.updateParams(me.paramEmissions, me.neuronTypeSelectedString)
        me.networkSim.reRun()
        me.sbl.setText("Changing parameter #"+str(paramId)+": "+str(value)+" ["+me.neuronTypeSelectedString+"]")

    def updateSingleParam(me, value, paramId):
        me.singleParamValues[paramId] = value
        me.networkSim.updateSingleParameters(*me.singleParamValues)
        if paramId in {1, 4, 5}: # (touch signal) 1 - tIntense, 4 - tDur, 5 - tOffDur 
            me.networkSim.generateTouch()
        elif paramId in {2, 3}: # (network) 2 - numSensors, 3 - numInters
            me.networkSim.generateNetwork()
        # index 0 just needs a re-run
        me.networkSim.reRun()


    def updateNeuronTypeSelected(me, strType, intType):
        oldType = me.neuronTypeSelectedString
        me.neuronTypeSelectedString = strType
        me.neuronTypeSelectedInt = intType
        #print("OLD: {0} => NEW: {1}".format(oldType, strType))
        me.saveSliderStateSignal.emit(oldType, strType)
        me.networkSim.updateParams(me.paramEmissions, me.neuronTypeSelectedString)

        

        
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
        parameters = ['a (recovery time)', 'b (u sense, sub-t fluc)', 'c (mV reset)', 'd (u reset)']
        parametersLow = [.001, .01, -100, 0]
        parametersHigh = [1, 1, -25, 20]
        parametersDefault = [.02, .2, -65, 8]
        #parametersDefaultInter = [.02, .2, -65, 8]
        #parametersDefaultSensory = [.02, .2, -55, 4]
        numParams = len(parameters)
        ''' PUT THIS IN A SCROLL PANE AT SOME POINT??? ''' 

        # create array of sliders
        for i in range(0, numParams):
            sliderArray.append(SliderUnit(parameters[i],i,parametersLow[i], parametersHigh[i], parametersDefault[i]))
            me.paramEmissions.append(parametersDefault[i])
        for sliderUnit in sliderArray:
            sliderBox.addWidget(sliderUnit)
            me.sbl = QLabel(me)
            sliderBox.addWidget(me.sbl)
            sliderUnit.valueChangeSignal.connect(me.updateLabel)
            # set slot to save param value for different neuron types 
            me.saveSliderStateSignal.connect(sliderUnit.updateSliderForTypeSwitch)
        sliderBox.addStretch(1)
        #me.addSlider(grid, [0,0])

        #me.addSlider(grid, [2,0])
        grid.addLayout(sliderBox,1,0) 
        
        ##############################
        ### single parameters
        ##############################
        '''
        touch:
            duration, offDuration, intensity
        num sensors
        num inters
        AP Impact
        '''
        me.singleParamContainer = QHBoxLayout(me)
        me.singleParamNames = ["AP Impact", "tIntense",  "#Sensors", "#Inters", "tDur", "tOffDur"]
        singleParamDefaults = [25.0, 10.0, 1, 1, 100, 200]
        me.singleParamValues = singleParamDefaults
        me.singleParamBoxes = []
        numSPs = len(me.singleParamNames)
        for i in range(0, numSPs): 
            me.singleParamBoxes.append(SingleParameterBox(me.singleParamNames[i], i, singleParamDefaults[i]))
            me.singleParamBoxes[i].paramChangedSignal.connect(me.updateSingleParam)
            me.singleParamContainer.addWidget(me.singleParamBoxes[i])
        me.singleParamContainer.addStretch(1)
        grid.addLayout(me.singleParamContainer,0,1)

        ################################
        ### radio buttons to select type
        ### of neuron we are modifying
        ################################
        me.neuronTypeSelector = NeuronTypeRadioSelector()
        me.neuronTypeSelectedString = 'sensor'
        me.neuronTypeSelectedInt = 0
        me.neuronTypeSelector.neuronTypeSelectedSignal.connect(me.updateNeuronTypeSelected)
        
        grid.addWidget(me.neuronTypeSelector, 0, 0)
        

        ####### matplotlib plot #######
        ###############################

        me.plotCanvas = PlotCanvas()
        me.networkSim = NetworkSimulator(me.plotCanvas)
        me.networkSim.run()
        me.networkSim.updatePlot()
        
        ##
        #me.plotCanvas.draw()
        grid.addWidget(me.plotCanvas, 1, 1)

        ######################
        ### matplotlib toolbar
        ######################
        me.navigationToolbar = NavigationToolbar(me.plotCanvas, me)
        grid.addWidget(me.navigationToolbar, 2, 1)

        ###############
        me.setGeometry(500, 300, 1400, 800)
        me.setWindowTitle('Gooey Spikes v0.1')
        me.show()



if __name__ == '__main__':
    
    app = QApplication(sys.argv)
    display = Display()
    sys.exit(app.exec_())


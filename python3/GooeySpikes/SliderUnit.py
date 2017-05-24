from __future__ import division
import sys
from PyQt5.QtCore import Qt, pyqtSignal, QObject
from PyQt5.QtWidgets import (QWidget, QLCDNumber, QSlider, QLineEdit, QLabel, QGridLayout, QHBoxLayout, QVBoxLayout, QApplication)
from PyQt5.QtCore import pyqtSlot, Qt, pyqtSignal



'''
** NOTE: default is 100 ints for slider,
so, each slider 'step' is "(max-min)/100"

horizontal row, from upper to lower:
    - qlabel "Min: "
    - min qlineedit
    - qlabel "Max: "
    - max qline edit
    - QSlider
    - QLabel with value
'''
class SliderUnit(QWidget):

    sliderChangeSignal = pyqtSignal(int)
    valueChangeSignal = pyqtSignal(float, int)
    minChangeSignal = pyqtSignal('QString')
    maxChangeSignal = pyqtSignal('QString')
    SLIDER_MULTIPLIER = 10000.0
    ABSOLUTE_SLIDER_MAX = 100
    ABSOLUTE_SLIDER_MIN = 0
    SLIDER_RANGE = ABSOLUTE_SLIDER_MAX - ABSOLUTE_SLIDER_MIN
    LINE_EDIT_WIDTH = 50
    LINE_EDIT_HEIGHT = 20

    def __init__(me, paramName, paramId, paramMin = 0, paramMax = 100, paramDefault = 0):
        super().__init__()
        me.paramId = paramId
        me.minLabel = QLabel("Min:", me)
        me.maxLabel = QLabel("Max:", me)
        me.sliderValueLabel = QLabel("Value:", me)
        me.paramNameLabel = QLabel("Parameter: "+paramName, me)
        # not scaled:
        me.sliderMin = me.ABSOLUTE_SLIDER_MIN
        me.sliderMax = me.ABSOLUTE_SLIDER_MAX
        me.sliderValue = 0
        # scaled:
        me.paramMin = paramMin
        me.paramMax = paramMax
        me.paramDefault = paramDefault
        me.paramValue = paramDefault
        me.oldTypesDict = {}
        me.initUI()

    def initUI(me):
        me.hBox = QVBoxLayout(me)
        me.setLayout(me.hBox)
        me.makeSliderUnit()
    
    def makeSliderUnit(me):
        me.meta_upper_hBox = QHBoxLayout(me)
        me.upper_hBox = QHBoxLayout(me)
        me.lower_hBox = QHBoxLayout(me)
        # box for meta info
        me.hBox.insertLayout(0,me.meta_upper_hBox,0)
        # box for slider info
        me.hBox.insertLayout(1,me.upper_hBox,0)
        # box for slider
        me.hBox.insertLayout(2,me.lower_hBox, 0)
        #### META UPPER HBox
        me.meta_upper_hBox.insertStretch(0)
        me.meta_upper_hBox.addWidget(me.paramNameLabel)
        me.meta_upper_hBox.insertStretch(2)
        ##### UPPER HBOX
        # 'Min:' label
        me.upper_hBox.addWidget(me.minLabel)
        # edit box to set min bound
        me.minLineEdit = QLineEdit(me)
        me.minLineEdit.setMaximumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.minLineEdit.setMinimumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.minLineEdit.setText(str(me.paramMin))
        me.upper_hBox.addWidget(me.minLineEdit)
        # 'Max:' label
        me.upper_hBox.addWidget(me.maxLabel)
        # edit box to set max bound
        me.maxLineEdit = QLineEdit(me)
        me.maxLineEdit.setMaximumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.maxLineEdit.setMinimumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.maxLineEdit.setText(str(me.paramMax))
        me.upper_hBox.addWidget(me.maxLineEdit)
        # 'Value:' label
        me.upper_hBox.addWidget(me.sliderValueLabel)
        # slider value box (should get written to, and read from) -- able to display *and* set slider value.
        me.sliderValueLine = QLabel(me)
        me.sliderValueLine.setMaximumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.sliderValueLine.setMinimumSize(me.LINE_EDIT_WIDTH, me.LINE_EDIT_HEIGHT)
        me.sliderValueLine.setText(str(me.paramValue))
        me.upper_hBox.addWidget(me.sliderValueLine)
        ##### RIGHT HBOX
        # slider
        me.slider = QSlider(Qt.Horizontal, me)
        me.slider.setMinimum(me.sliderMin)
        me.slider.setMaximum(me.sliderMax)
        me.slider.setValue(me.convertFromValueBox(me.paramValue))
        me.slider.setTickPosition(QSlider.TicksBelow)
        #lcd = QLCDNumber(me)
        me.lower_hBox.addWidget(me.slider)
        #me.sliderValueLine.setMaxLength(3)
        me.connectSignalsAndSlots()

    def connectSignalsAndSlots(me):
        # slider, from slider to value box
        me.slider.valueChanged.connect(me.onSliderChange)
        me.sliderChangeSignal.connect(me.getSliderValue)
        # value box, from value box to slider
        ##me.sliderValueLine.textChanged.connect(me.onValueChange)
        ##me.valueChangeSignal.connect(me.getValueString)
        # min box, from box to slider min val
        me.minLineEdit.textChanged.connect(me.onMinChange)
        me.minChangeSignal.connect(me.getMinString)
        # max box, from box to slider max val
        me.maxLineEdit.textChanged.connect(me.onMaxChange)
        me.maxChangeSignal.connect(me.getMaxString)

    #####
    ## converters
    #####

    ''' returns false if there was an issue, else, returns a float
    -> the point is to allow the slider to work with non-int values
    '''
    def convertFromValueBox(me, displayValueString):
        try:
            displayValue = float(displayValueString)
        except ValueError:
            return False 
        paramRange = me.paramMax - me.paramMin
        if (displayValue < 0):
            displayValue *= -1
        percentToSet = displayValue/paramRange
        baseSliderValue = percentToSet * me.SLIDER_RANGE
        realSliderValue = int(baseSliderValue + me.sliderMin)
        return realSliderValue
    
    def updateParameterValue(me, value):
        # the value is going to be an int between sliderMin and sliderMax. 
        # we need to convert it to a relative value between paramMin and paramMax.
        adjusted = value - me.sliderMin;
        sliderPercent = float(adjusted)/me.SLIDER_RANGE
        paramRange = me.paramMax - me.paramMin
        baseParamValue = paramRange*sliderPercent
        me.paramValue = round(baseParamValue + me.paramMin, 5)
        me.valueChangeSignal.emit(me.paramValue, me.paramId)
        return me.paramValue

    #####
    ## signals and slots
    #####

    ''' slider change signal ''' 
    def onSliderChange(me, value):
        me.sliderChangeSignal.emit(value)

    ''' slider change slot '''
    @pyqtSlot(int)
    def getSliderValue(me, value):
        newText = str(me.updateParameterValue(value))
        me.sliderValueLine.setText(newText)

    ''' value line change signal 
    def onValueChange(me, value):
        me.valueChangeSignal.emit(value)
    '''

    ''' value line change slot 
    @pyqtSlot('QString')
    def getValueString(me, value):
        valueToSet = me.convertFromValueBox(value)
        if not valueToSet == False:
            print("value to set: "+str(valueToSet))
            #me.slider.setValue(valueToSet)
            me.slider.setValue(int(value))
    '''

    ''' min value change signal '''
    def onMinChange(me, value):
        me.minChangeSignal.emit(value)
        
    ''' min value change slot '''
    @pyqtSlot('QString')
    def getMinString(me, value):
        try: 
            floatVal = float(value)
            me.paramMin = floatVal
            if me.paramMin > me.paramMax:
                me.paramMin = me.paramMax
                me.minLineEdit.setText(str(me.paramMin))
        except ValueError:
            pass

    ''' max value change signal '''
    def onMaxChange(me, value):
        me.maxChangeSignal.emit(value)
        
    ''' max value change slot '''
    @pyqtSlot('QString')
    def getMaxString(me, value):
        try: 
            floatVal = float(value)
            me.paramMax = floatVal
            if me.paramMax < me.paramMin:
                me.paramMax = me.paramMin
                me.minLineEdit.setText(str(me.paramMax))
        except ValueError:
            pass

    ''' allows saving state between neuron types '''
    @pyqtSlot('QString', 'QString')
    def updateSliderForTypeSwitch(me, oldType, newType):
        me.oldTypesDict[oldType] = me.paramValue
        if newType in me.oldTypesDict:
            me.paramValue = me.oldTypesDict[newType]
        else:
            me.paramValue = me.paramDefault
        me.slider.setValue(me.convertFromValueBox(me.paramValue))
        

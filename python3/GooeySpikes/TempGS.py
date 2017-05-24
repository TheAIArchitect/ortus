import sys

from PyQt5.QtWidgets import QApplication, QMainWindow, QMenu, QVBoxLayout, QSizePolicy, QMessageBox, QWidget, QPushButton
from PyQt5.QtGui import QIcon
 
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

import random

 
class App(QMainWindow):
 
    def __init__(me):
        super().__init__()
        #me.left = 10
        #me.top = 10
        me.title = 'Gooey Spikes v0.1'
        #me.width = 640
        #me.height = 400
        me.initUI()
 
    def initUI(me):
        me.setWindowTitle(me.title)
        #me.setGeometry(me.left, me.top, me.width, me.height)
 
        #m = PlotCanvas(me)
        #m.move(0,0)
        #m.plot()
        me.plotCanvas = PlotCanvas(me)
        
        #me.show()

 
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
        #me.plot()
 
 
    def plot(me):
        a = 4.3
        data = [a*i for i in range(25)]
        ax = me.fig.add_subplot(111)
        ax.plot(data, 'r-')
        ax.set_title('Potential (mV)')
        ax.grid(True)
        me.draw()

    ''' 
    xVals is the x data, dataArray is a list of y data arrays, which get looped through,
    all plotted against xVals
    
    plotTop creates the top subplot of a two subplot figure
    '''
    def plotTop(me, xVals, dataArray):
        me.ax = me.fig.add_subplot(211)
        me.ax.set_ylabel("Potential (mV)")
        me.ax.grid(True)
        for data in dataArray:
            me.ax.plot(xVals, data)

    ''' 
    xVals is the x data, dataArray is a list of y data arrays, which get looped through,
    all plotted against xVals
    
    plotBottom creates the bottom subplot of a two subplot figure
    '''
    def plotBottom(me, xVals, dataArray):
        me.ax = me.fig.add_subplot(212)
        me.ax.set_xlabel("Time (ms)")
        me.ax.set_ylabel("Potential (mV)")
        me.ax.grid(True)
        for data in dataArray:
            me.ax.plot(xVals, data)
        #me.ax.axhline(0, color='green', lw=2)

 
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())

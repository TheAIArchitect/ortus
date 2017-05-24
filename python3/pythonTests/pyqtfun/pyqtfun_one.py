#!/usr/bin/python3
# -*- coding: utf-8 -*-

"""
ZetCode PyQt5 tutorial 

In this example, we create a simple
window in PyQt5.

author: Jan Bodnar
website: zetcode.com 
last edited: January 2015
"""

import sys
#basic widgets in QWidget
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton
from PyQt5.QtWidgets import QDesktopWidget, QMainWindow, QGridLayout
from PyQt5.QtCore import QCoreApplication
from PyQt5.QtGui import QIcon, QPixmap

class Example(QMainWindow):
#class Example(QWidget):

    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):

        grid = QGridLayout()

        # make 10x10 grid
        #positions = [(i,j) for i in range(10) for j in range(10)]
        #for position in positions:
                        

        #self.resize(250, 150) # wide, high (px)
        self.move(300, 300) # x, y 
        #self.setGeometry(300, 300, 300, 220) # x, y, width, height
        self.center() # center window on screen.
        self.setWindowTitle('Simple') 
        # can add tooltips.
        #self.setWindowIcon(QIcon("../images/gooeyspikes.png"))

        self.statusBar().showMessage("this is the status bar.")

        button = QPushButton("Button", self) # self is the parent
        button.resize(button.sizeHint())
        #button.move(50,50)
        grid.addWidget(button, *(0,0))

        quitButton = QPushButton("Quit", self)

        # can reimplement the closeEvent() event handler that will get called upon closing application
        quitButton.clicked.connect(QCoreApplication.instance().quit)
        grid.addWidget(quitButton, *(0,5))

        mainWidget = QWidget()

        mainWidget.setLayout(grid)
        self.setCentralWidget(mainWidget)

        self.show()

    def center(self):

        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

# see http://zetcode.com/gui/pyqt5/menustoolbars/
# to make menu and toolbars 
if __name__ == '__main__':
    
    # every pyqt5 application must create an application object
    app = QApplication(sys.argv)

    #w = QWidget() # base class of all UI objects, no parent, thus, is a 'window'
    eg = Example()
        
    sys.exit(app.exec_()) # app.exec_() enters main loop. exec is a python keyword, so, they added the _

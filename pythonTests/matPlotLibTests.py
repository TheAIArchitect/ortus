#!/usr/bin/python

from __future__ import division
import matplotlib.pyplot as mpl
import math


class SimpleNeuron:

    # variables specified out here are class variables, and are *shared* between instances
    
    def __init__(self, neuronType):
        # variables specified in here, are instance variables and are not shared between instances
        self.activationHistory = []
        self.newInputs = 0
        #self.d = 2
        # regular spiking (RS): d = 8, c = -65
        self.a = 0.02
        self.b = 0.2
        self.c = -65
        self.d = 8
        if (neuronType.__eq__("sensory")): #Intrinsically Bursting (IB)
            self.c = -55
            self.d = 4
        #elif (neuronType.__eq__("inter")): #regular spiking for now
            

        self.baselineActivation = self.c
        self.activation = self.baselineActivation
        self.decay = self.b*self.activation
        self.update()


    def getActivation(self):
        return self.activation

    def fired(self):
        # would be better to use slope computation to determine this... right?
        return (self.activation >= 30)

    def addInput(self, inputAct):
        self.newInputs = self.newInputs + inputAct


    def shitupdate(self):
        actDiff = 0
        decayDiff = 0
        if (self.activation >= 30):
            self.activation = self.baselineActivation
            self.decay = self.decay + self.d
        else:
            # then compute the updated activation, based upon current activation, decay, and total of new inputs
            actDiff = 0.04*math.pow(self.activation,2) + 5*self.activation + 140 - self.decay + self.newInputs
            decayDiff = self.a*(self.b*self.activation - self.decay)
        self.activation = self.activation + actDiff
        self.decay = self.decay + decayDiff
        # update our activation history
        self.activationHistory.append(self.activation)
        self.newInputs = 0;

    def update(self):
        actDiff = 0
        decayDiff = 0
        if (self.activation >= 30):
            self.activation = self.baselineActivation
            self.decay = self.decay + self.d
        else:
            # then compute the updated activation, based upon current activation, decay, and total of new inputs
            actDiff = 0.04*math.pow(self.activation,2) + 5*self.activation + 140 - self.decay + self.newInputs
            decayDiff = self.a*(self.b*self.activation - self.decay)
            self.activation = self.activation + actDiff
            if (self.activation > 30):
                self.activation = 30
            self.decay = self.decay + decayDiff
        # update our activation history
        self.activationHistory.append(self.activation)
        self.newInputs = 0;


#merge the bolztmann deal from HH into above to allow membrane potential to impact... influence of individual neurons. maybe

numIterations = 2000
touchStimulus = []
touchDuration = 100
touchOffDuration = 200
touchCycle = touchDuration + touchOffDuration
touchIntensity = 15
touchOn = False
for i in range (0, numIterations+1):
    if (not touchOn and i % touchCycle == 0):
        touchOn = True
    elif (touchOn and i % touchDuration == 0):
        touchOn = False
    if touchOn:
        touchStimulus.append(touchIntensity)
    else:
        touchStimulus.append(0)

sensor = SimpleNeuron("sensory")
inter = SimpleNeuron("inter")
activationForInter = 0
xVals = []
for i in range(0,numIterations):
    sensor.addInput(touchStimulus[i])
    sensor.update()
    if (sensor.fired()):
        activationForInter = 25
    else:
        activationForInter = 0
    inter.addInput(activationForInter)
    inter.update()
    xVals.append(i)
xVals.append(numIterations+1)


fig = mpl.figure()
ax1 = fig.add_subplot(211)
ax1.plot(xVals, sensor.activationHistory)
ax1.plot(xVals, inter.activationHistory)
ax1 = fig.add_subplot(212)
ax1.plot(xVals, touchStimulus)
ax1.grid(True)
#ax1.axhline(0, color='black', lw=2)

#ax.annotate('local max', xy=(2, 1), xytext=(3, 1.5), arrowprops=dict(facecolor='black', shrink=0.05),)

'''
ax2 = fig.add_subplot(212, sharex=ax1)
ax2.acorr(y[1], usevlines=True, normed=True, maxlags=None, lw=2)
ax2.grid(True)
ax2.axhline(0, color='black', lw=2)
'''

mpl.show()

#print(str(tester.activationHistory))


from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt


from SimpleNeuronModel import SimpleNeuron


class NetworkSimulator:

    def __init__(me, thePlotCanvas):
        me.numIterations = 1500
        #numIterations = 100
        me.touchDuration = 100
        me.touchOffDuration = 200
        me.touchIntensity = 10 
        # create the touch signal
        me.generateTouch()
        me.numSensors = 1
        me.numInters = 1
        me.actionPotentialImpact = 25
        # create the network
        me.generateNetwork()
        thePlotCanvas.setLegend(["sensory", "inter"], ["touch"])
        me.plotCanvas = thePlotCanvas

    def reRun(me):
        me.clearData()
        me.run()
        me.updatePlot()

    def updateSingleParameters(me, APimpact, tIntense, numSensors, numInters, tDur, tOffDur):
        me.actionPotentialImpact = APimpact # mV
        me.touchIntensity = tIntense # mV
        me.numSensors = int(numSensors) 
        me.numInters = int(numInters)
        if (tDur != 0):
            me.touchDuration = int(tDur) # mS
        if (tOffDur != 0):
            me.touchOffDuration = int(tOffDur) # mS
        
    def generateTouch(me):
        me.touchStimulus = []
        touchOn = True 
        me.touchCycle = me.touchDuration + me.touchOffDuration
        for i in range (0, me.numIterations):
            if (not touchOn and i % me.touchCycle == 0):
                touchOn = True
            elif (touchOn and i % me.touchDuration == 0):
                touchOn = False
            if touchOn:
                me.touchStimulus.append(me.touchIntensity)
            else:
                me.touchStimulus.append(0)

    def generateNetwork(me):
        me.sensors = []
        me.inters = []
        for i in range(0, me.numSensors):
            me.sensors.append(SimpleNeuron("sensory"))
        for i in range(0, me.numInters):
            me.inters.append(SimpleNeuron("inter"))


    def updateParams(me, params, neuronType):
        if neuronType.__eq__("sensor"):
            for sensor in me.sensors:
                sensor.updateParameters(*params)
        elif neuronType.__eq__("inter"):
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
            # first, all sensors pass 'activation' along to all inters
            for sensor in me.sensors:
                sensor.addInput(me.touchStimulus[i])
                sensor.update()
                # if it fired
                if (sensor.fired()):
                    # we do it such that each sensor sees each inter
                    for inter in me.inters:
                        inter.addInput(me.actionPotentialImpact)
            for inter in me.inters:
                inter.update()
            me.xVals.append(i)
        
    def updatePlot(me):
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
        me.plotCanvas.plotTop(me.xVals, topData)
        ##
        bottomData = []
        bottomData.append(me.touchStimulus)
        #
        me.plotCanvas.plotBottom(me.xVals, bottomData)



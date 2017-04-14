from __future__ import division

def fact(f):
    result = 1
    for i in range(1, f+1):
            result = result * i
    return result

def nCr(n, r):
    nFact = fact(n)
    rFact = fact(r)
    nMINUSrFact = fact(n-r)
    div = rFact*nMINUSrFact
    return nFact/div

def countConsolidatoryInters(numToLink, consolidatoryGranularity):
    res = 0
    for i in range(consolidatoryGranularity, numToLink+1):
        res = res + nCr(numToLink, i)
    return res

def compute(numSensors, granularity):
    res = countConsolidatoryInters(numSensors, granularity)
    print("if only "+str(numSensors)+" sensors, granularity "+str(granularity)+": "+str(res)) 


numSensors = 3
granularity = 1
compute(numSensors, granularity)
compute(5, 1)
compute(10, 1)
compute(50, 1)
compute(50, 2)
compute(50, 3)
compute(20, 3)
print("from 2^n-1 paper..")
compute(8,1)
compute(8,4)
compute(4,1)


#!/usr/local/bin/python3
import math


'''
This function looks odd, but it's just taking:
-> the portion of 'static' from staticOffset to xcorrLen (remember, python lists are exclusive of last index, so you use length instead of the actual last index)
-> the portion of 'dynamic' from dynamicOffset to xcorrLen
-> then, does an element-wise multiplication of the two lists described above (this goes into xcorrList)
-> finally, it sums xcorrList, and returns that value.
'''
def xcorrMultiply(static, dynamic, staticOffset, dynamicOffset, xcorrLen):
    staticStart = staticOffset
    staticEnd = staticOffset + xcorrLen # rememver, last 'index' isn't used by python lists
    dynamicStart = dynamicOffset
    dynamicEnd = dynamicOffset + xcorrLen
    xcorrList = [aItem*bItem for aItem,bItem in zip(static[staticStart:staticEnd],dynamic[dynamicStart:dynamicEnd])]
    return sum(xcorrList)
    
    


A = [5,7,9,3,1,4,6,3]
B = [2,4,8,6,8,9,5,4]
xcorrLength = 4

print("#### xc3_0:")

# get autoA and autoB for [3..6]
autoA = xcorrMultiply(A, A, 3, 3, xcorrLength)
autoA_validation = 3*3 + 1*1 + 4*4 + 6*6  
print("autoA[3..6]: ",autoA,"(function validation:",autoA_validation,")")

autoB = xcorrMultiply(B, B, 3, 3, xcorrLength)
autoB_validation = 6*6 + 8*8 + 9*9 + 5*5 
print("autoB[0..3]: ",autoB,"(function validation:",autoB_validation,")")

xcorrDivisor = math.sqrt(autoA*autoB)
print("xcorr divisor:",xcorrDivisor)

# get xcorr between B and A for windows [3..6]
xc3_0 = xcorrMultiply(B, A, 3, 3, xcorrLength)
xc3_0_validation = (6*3 + 8*1 + 9*4 + 5*6)

print("xc3_0 (pre normalization): ",xc3_0,"(function validation:",xc3_0_validation,")")

xc3_0 = xc3_0/xcorrDivisor
print("xc3_0 (normalized): ",xc3_0)

print("Note: no further validation prints will be made.")


#### the remainer of this example gets repeated for every xcorr with B held satic at [3..6] and A 'sliding' (backward)
xcBA = [xc3_0]
# start at window [3] for B, and [2] for A, as we just did the first one (at 3)
staticOffset = 3 
dynamicOffset = 2
numXCorrs = 4

# start at 1, we just did 0
for i in range(1,numXCorrs):
    print("#### xc3_"+str(i)+":")
    autoA = xcorrMultiply(A, A, dynamicOffset, dynamicOffset, xcorrLength)
    print("autoA["+str(dynamicOffset)+".."+str((dynamicOffset+(numXCorrs-1)))+"]: ",autoA)
    xcorrDivisor = math.sqrt(autoB*autoA)
    print("xcorr divisor:",xcorrDivisor)
    tempXC = xcorrMultiply(B, A, staticOffset, dynamicOffset, xcorrLength)
    tempXC = tempXC/xcorrDivisor
    print("=> xcZY["+str(i)+"]:",tempXC)
    print("xc3_"+str(i)+" (normalized): ",tempXC)
    xcBA.append(tempXC)
    dynamicOffset -= 1

print(xcBA)

############# new
print("\n\nZ and Y:") 
Z = [0, 0, 0, 2, 4, 6, 7, 6, 4, 2, 0, 0, 0]
Y = [0, 0, 0, 0, 0, 0, 2, 4, 5, 4, 2, 0, 0]
xcYZ = []
autoZ = 0
autoY = 0
# and we specially care about Y[5..8], and want to see how Z is correlated with it at time windows [0, -1. -2, and -3]
staticOffset = 5 
dynamicOffset = 5
numXCorrs = 4

autoY = xcorrMultiply(Y, Y, staticOffset, staticOffset, xcorrLength)
for i in range(0,numXCorrs):
    autoZ = xcorrMultiply(Z, Z, dynamicOffset, dynamicOffset, xcorrLength)
    xcorrDivisor = math.sqrt(autoY*autoZ)
    tempXC = xcorrMultiply(Y, Z, staticOffset, dynamicOffset, xcorrLength)
    tempXC = tempXC/xcorrDivisor
    print("=> xcZY["+str(i)+"]:",tempXC)
    xcYZ.append(tempXC)
    dynamicOffset -= 1







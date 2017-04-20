#!/usr/local/bin/python3
import math


'''
This function looks odd, but it's just taking:
-> the portion of 'static' from staticOffset to xcorrLen (remember, python lists are exclusive of last index, so you use length instead of the actual last index)
-> the portion of 'dynamic' form dynamicOffset to xcorrLen
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
    
    


A = [5,7,9,3,1,4]
B = [2,4,8,6,7,9,3,4,8,5]
xcorrLength = 4

print("#### xc0:")

# get autoA and autoB for [0..3]
autoA = xcorrMultiply(A, A, 0, 0, xcorrLength)
autoA_validation = 5*5 + 7*7 + 9*9 + 3*3  
print("autoA[0..3]: ",autoA,"(function validation:",autoA_validation,")")

autoB = xcorrMultiply(B, B, 0, 0, xcorrLength)
autoB_validation = 2*2 + 4*4 + 8*8 + 6*6 
print("autoB[0..3]: ",autoB,"(function validation:",autoB_validation,")")

xcorrDivisor = math.sqrt(autoA*autoB)
print("xcorr divisor:",xcorrDivisor)

# get xcorr between A and B for windows [0..3]
xc0 = xcorrMultiply(A, B, 0, 0, xcorrLength)
xc0_validation = (5*2 + 7*4 + 9*8 + 3*6)

print("xc0 (pre normalization): ",xc0,"(function validation:",xc0_validation,")")

xc0 = xc0/xcorrDivisor
print("xc0 (normalized): ",xc0)

print("Note: no further validation prints will be made.")

#### this chunk gets repeated for every xcorr with A held satic at [0..3] and B 'sliding'
# recompute autoB for [1..4], and recompute xcorrDivisor
print("#### xc1:")
autoB = xcorrMultiply(B, B, 1, 1, xcorrLength)
print("autoB[1..4]: ",autoB)

xcorrDivisor = math.sqrt(autoA*autoB)
print("xcorr divisor:",xcorrDivisor)

# get xcorr between A[0..3] and B[1..4]
xc1 = xcorrMultiply(A, B, 0, 1, xcorrLength)
xc1 = xc1/xcorrDivisor
print("xc1 (normalized): ",xc1)
#### end chunk

#### this chunk gets repeated for every xcorr with A held satic at [0..3] and B 'sliding'
# recompute autoB for [2..5], and recompute xcorrDivisor
print("#### xc2:")
autoB = xcorrMultiply(B, B, 2, 2, xcorrLength)
print("autoB[2..5]: ",autoB)

xcorrDivisor = math.sqrt(autoA*autoB)
print("xcorr divisor:",xcorrDivisor)

# get xcorr between A[0..3] and B[1..4]
xc2 = xcorrMultiply(A, B, 0, 2, xcorrLength)
xc2 = xc2/xcorrDivisor
print("xc2 (normalized): ",xc2)
#### end chunk

#### this chunk gets repeated for every xcorr with A held satic at [0..3] and B 'sliding'
# recompute autoB for [3..6], and recompute xcorrDivisor
print("#### xc3:")
autoB = xcorrMultiply(B, B, 3, 3, xcorrLength)
print("autoB[3..6]: ",autoB)

xcorrDivisor = math.sqrt(autoA*autoB)
print("xcorr divisor:",xcorrDivisor)

# get xcorr between A[0..3] and B[1..4]
xc3 = xcorrMultiply(A, B, 0, 3, xcorrLength)
xc3 = xc3/xcorrDivisor
print("xc3 (normalized): ",xc3)
#### end chunk


xcAB = [xc0, xc1, xc2, xc3]
print(xcAB)

############# new
print("\n\nZ and Y:") 
Z = [0, 0, 0, 2, 4, 6, 7, 6, 4, 2, 0, 0, 0]
Y = [0, 0, 0, 0, 0, 0, 2, 4, 5, 4, 2, 0, 0]
xcZY = []
autoZ = 0
autoY = 0
# in xcorr_explanation the signals Z and Y are both zero before time window 3,
# and we specially care about Z[3..6]
staticOffset = 3 
dynamicOffset = 3
numXCorrs = 4

autoZ = xcorrMultiply(Z, Z, staticOffset, staticOffset, xcorrLength)
for i in range(0,numXCorrs):
    autoY = xcorrMultiply(Y, Y, dynamicOffset, dynamicOffset, xcorrLength)
    xcorrDivisor = math.sqrt(autoZ*autoY)
    tempXC = xcorrMultiply(Z, Y, staticOffset, dynamicOffset, xcorrLength)
    tempXC = tempXC/xcorrDivisor
    print("=> xcZY["+str(i)+"]:",tempXC)
    xcZY.append(tempXC)
    dynamicOffset += 1







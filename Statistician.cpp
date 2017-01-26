//
//  Statistician.cpp
//  ortus
//
//  Created by andrew on 1/23/17.
//  Copyright © 2017 Andrew W.E. McDonald. All rights reserved.
//

#include "Statistician.hpp"


Statistician::Statistician(){};

//Statistician::~Statistician(){};





/* implements the window part of the sliding window, computes all lags of the xcorr.
 *
 * NOTE: this requires A and B have the same length.
 */
float Statistician::xcorrMultiply(ortus::vector A, ortus::vector B, int windowNum){
    size_t len = A.size();
    if ((windowNum < 0) || ((windowNum + 1) >= (2 * len))){// see "aStartingIndex = (windowNum - len) + 1" line for reasoning
        return 0.f;  // out of range
    }
    size_t lastIndex = len - 1;
    size_t iterations = 0;
    size_t aStartingIndex = 0;
    size_t bStartingIndex = 0;
    if (windowNum >= len){ // then we change our approach, because B[0] is under A[1] (assuming we are sliding B)
        bStartingIndex = 0; // always start at the beginning of B
        aStartingIndex = (windowNum - len) + 1; // at (6 - 4) + 1, we're done (if windowNum == 7, we have a problem)... that's the reasoning for the check above.
        iterations = len - aStartingIndex;
    }
    else {
        bStartingIndex = lastIndex - windowNum; // also, windowNum can't be less than 0, or we go past the end of B.
        aStartingIndex = 0; // always start at the beginning of A
        iterations = len - bStartingIndex;
    }
    float result = 0;
    size_t aIndex = aStartingIndex;
    size_t bIndex = bStartingIndex;
    //printf("window: %d\n",windowNum);
    for (int i = 0; i < iterations; ++i){
        //printf("\raIndex, bIndex: %d (%.2f), %d (%.2f)\n",aIndex, A[aIndex], bIndex, B[bIndex]);
        result += A[aIndex] * B[bIndex];
        aIndex++;
        bIndex++;
    }
    return result;
}




int Statistician::computeXCorrNumWindows(int len){
    // see xcorrMulitply for computation reasoning
    return ((2*len)-1);
}


float Statistician::xcorrMultiplyLimited(ortus::vector A, int aOffset, ortus::vector B, int bOffset, int len){
    float result = 0;
    for (int i = 0; i < len; ++i){
        result += A[i+aOffset] * B[i+bOffset];
    }
    return result;
}




/* A is the 'static' one, meaning we compute the xcorr between A and B such that
 * a xcorr is computed between A[start] to A[len-1] and:
 *      -> B[start] to B[len-1]
 *      -> B[start+1] to B[len]
 *      -> B[start+2] to B[len+1]
 *      -> and so on, until len+n == end, which is the final xcorr computation (which means that 'end' is *inclusive*, because it is an index, and not a length, or size.
 
 */
ortus::vector Statistician::xcorrLimited(ortus::vector A, ortus::vector B, int start, int len, int end){
    size_t aLen = A.size();
    size_t bLen = B.size();
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    size_t xcorrLoops = end - (len - 1); // subtract one from len becuase then we have the last index that we consider for the xcorr computation at 0 offset
    int aOffset = 0;
    ortus::vector xcorrResults;
    float autoCorrA = 0;
    float autoCorrB = 0;
    float divisor = 0;
    autoCorrA = xcorrMultiplyLimited(A, aOffset, A, aOffset, len);
    for (int i = 0; i < xcorrLoops; ++i){
        // 'i' is what we would set 'bOffset' to, but this comment should be enough to make that clear.
        autoCorrB = xcorrMultiplyLimited(B, i, B, i, len);
        divisor = sqrtf(autoCorrA * autoCorrB);
        if (divisor == 0){
            xcorrResults.push_back(0.f);// can't divide by 0, this means we don't have full data for this window of time, so there can't be any correlation. set it to 0.
        }
        else {
            xcorrResults.push_back((xcorrMultiplyLimited(A, aOffset, B, i, len)/divisor));
        }
    }
    return xcorrResults;
}

/* The names of elements in computeList are the elements used as the "A" elements in the xcorr computations.
 
 * If computeList is empty, all elements are used.
 */
//std::unordered_map<int, ortus::vectrix> Statistician::computeXCorrBetweenVoltages(ortus::vectrix voltageHistoryVector, int numXCorrComputations, const std::string* computeList ){
std::unordered_map<int, ortus::vectrix> Statistician::computeXCorrBetweenVoltages(DataSteward* dStewiep, const std::string* computeList, int numToCompute){
    
    // this allows us to have the 'key' in the map be the same as the index the actual element has, so we can access what amounts to a sparse 3D matrix as if it were just that.
    ortus::vector indicesToUse(dStewiep->NUM_ELEMS);
    int numIndices = 0;
    if (numToCompute < 0){ // then we do all
        // doing this just so that if numToCompute is not empty, we can use the same code
        for (int i = 0; i < dStewiep->NUM_ELEMS; ++i){
            indicesToUse[i] = i;
        }
        numIndices = dStewiep->NUM_ELEMS;
    }
    else {
        for (int i = 0; i < numToCompute; ++i){
            indicesToUse[i] = dStewiep->officialNameToIndexMap[computeList[i]];
        }
        numIndices = numToCompute;
    }
    // the key, is the "A" element -- the static one
    // second index, is the "B" element -- this one, gets the window sliding across it
    // third index is xcorr computation 0 to dStewiep->numXCorrComputations-1
    // NOTE: totalXCorr[x][x][x] gives the autocorrelation for element X, which should be 1.0 (due to normalization)
    std::unordered_map<int, ortus::vectrix> totalXCorr(numIndices);
    ortus::vectrix voltageHistoryVector = dStewiep->outputVoltageHistory->convertDataTo2DVector();
    size_t numElements = voltageHistoryVector.size();
    int startIndex = 0; // we want to start at the beginning of the voltage history
    
    // endIndex is the startIndex, plus the length that we use for a single xcorr computation, plus the number of xcorr computations (because for each computation, we shift our window for 'B' over by 1), and then we subtract one because it's an index, not a length. It's 2*XCORR_COMPUTATIONS because XCORRR_COMPUTATIONS == the length that we use for a single xcorr computation.
    int endIndex = startIndex + 2*dStewiep->XCORR_COMPUTATIONS - 1;
    // This is going to be slow, and it would probably be best to either rad the resutls back from blade, or thread this computation...
    for (int i = 0; i < numIndices; ++i){// go through each element,
        ortus::vectrix tempXCorrRes(numElements);
        for (int j = 0; j < numElements; ++j){ // and compute the xcorr between it, and everything else (including itself)
            tempXCorrRes[j] = xcorrLimited(voltageHistoryVector[indicesToUse[i]], voltageHistoryVector[j], startIndex, dStewiep->XCORR_COMPUTATIONS, endIndex);
        }
        totalXCorr[indicesToUse[i]] = tempXCorrRes;
    }
    return totalXCorr;
}

/* normalizer should be 1, unless being called form normalizedXCorr, in which case, it will pass the appropriate value */
ortus::vector Statistician::xcorr(ortus::vector A, ortus::vector B, float normalizer){
    size_t aLen = A.size();
    size_t bLen = B.size();
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    size_t len = aLen;
    int maxWindows = computeXCorrNumWindows(len);
    ortus::vector xcorrResults;
    // note, signals are on top of eachother at (windowNum = len - 1)
    printf("note, signals are on top of eachother at (windowNum = len - 1) => %ld:\n",len-1);
    for (int i = 0; i < maxWindows; ++i){
        xcorrResults[i] = (xcorrMultiply(A, B, i))/normalizer;
        printf("window %d: %.2f\n",i, xcorrResults[i]);
    }
    return xcorrResults;
}

ortus::vector Statistician::normalizedXCorr(ortus::vector A, ortus::vector B){
    size_t aLen = A.size();
    size_t bLen = B.size();
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    float autoCorrA = xcorrMultiply(A, A, aLen - 1 ); // windowNum 'len - 1' (if first windowNum is 0), gives signals on top of each other (time 0)
    float autoCorrB = xcorrMultiply(B, B, bLen - 1 );
    float divisor = sqrt(autoCorrA*autoCorrB);
    printf("DIVISOR: %.2f\n", divisor);
    return  xcorr(A, B, divisor);
}

int main(int argc, char** argv){
    int len = 23;
    float A[] = {1,1,2,4,6,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float B[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,4,6,8,8};
    ortus::vector aVec;
    ortus::vector bVec;
    
    for (int i = 0; i < len; ++i){
        aVec.push_back(A[i]);
        bVec.push_back(B[i]);
    }
    
    //int len = 4;
    //float A[] = {0, 1, 0, 0};
    //float B[] = {0, 1, 0, 0};
    
    Statistician statr;
    ortus::vector results = statr.xcorr(aVec, bVec, 1.f);
    ortus::vector normalizedResults = statr.normalizedXCorr(aVec, bVec);
    
    return 0;
}

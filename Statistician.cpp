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
 * a xcorr is computed between A[start..start+(len-1)] (inclusive), and:
 *      -> B[start..start+(len-1)]
 *      -> B[(start-1)..(start-1)+(len-1)]
 *      -> B[(start-2)..(start-2)+(len-1)]
 *          ....
 *      -> B[(start-(len-1))..(start-(len-1))+(len-1)]
 * So, A indices are static, and B's create a backward sliding window.
 */
ortus::vector Statistician::xcorrLimited(ortus::vector A, ortus::vector B, int start, int len){
    size_t aLen = A.size();
    size_t bLen = B.size();
    if ((aLen != bLen) || (aLen == 0)){
        printf("xcorr requires two non-zero arrays of equal length.\n");
    }
    size_t xcorrLoops = len;
    int aOffset = start;
    int bOffset = start;
    ortus::vector xcorrResults;
    float autoCorrA = 0;
    float autoCorrB = 0;
    float divisor = 0;
    autoCorrA = xcorrMultiplyLimited(A, aOffset, A, aOffset, len);
    for (int i = 0; i < xcorrLoops; ++i){
        if (bOffset < 0){
            printf("bOffset == %d, must be >= 0...\n", bOffset);
            exit(83);
        }
        autoCorrB = xcorrMultiplyLimited(B, bOffset, B, bOffset, len);
        divisor = sqrtf(autoCorrA * autoCorrB);
        if (divisor == 0){
            xcorrResults.push_back(0.f);// can't divide by 0, this means we don't have full data for this window of time, so there can't be any correlation. set it to 0.
        }
        else {
            xcorrResults.push_back((xcorrMultiplyLimited(A, aOffset, B, bOffset, len)/divisor));
        }
        bOffset--;
    }
    return xcorrResults;
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

/**
 * function is an adaption of 'mitchnull's answer:
 * http://stackoverflow.com/questions/9430568/generating-combinations-in-c
 *
 * it's harder than you might think to generate combinations...
 * so, thank you, mitchnull.
 *
 * **********************
 * This function returns a vector of vectors, where each inner vector
 * holds the indices that should be combined into a new interneuron
 *
 * essentially, it performs the combinatoric logic behind sensory consolidation.
 * 
 * << this could also be called something like 'getCombinations'...>>
 */
std::vector<std::vector<int>> Statistician::getIndicesToLink(int numIndices, int resolution){
    int n = numIndices;
    int rMin = resolution;
    int rMax = n;
    std::vector<std::vector<int>> all;
    for (int r = rMin; r <= rMax; ++r){
        std::vector<bool> v(n);
        std::fill(v.begin(), v.begin() + r, true);
        do {
            std::vector<int> tempVec;
            for (int i = 0; i < n; ++i) {
                if (v[i]) tempVec.push_back(i);
            }
            all.push_back(tempVec);
        } while (std::prev_permutation(v.begin(), v.end()));
    }
    return all;
}




/*
int main() {
    
    std::vector<std::vector<int>> all = getIndicesToLink(8,3);
    int allSize = all.size();
    for (int i = 0; i < allSize; ++i){
        for (int j = 0; j < all[i].size(); ++j){
            printf("%d, ", all[i][j]);
        }
        printf("\n");
    }
    return 0;
}
*/

/*
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
*/

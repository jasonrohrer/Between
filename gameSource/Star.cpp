#include "Star.h"
#include "common.h"

#include "minorGems/util/random/StdRandomSource.h"

extern StdRandomSource randSource;



Star::Star()
        : mShiftA( NULL ), mShiftB( NULL ) {
    mBaseFade = 1.0;
    
    mShimmerTime = randSource.getRandomBoundedInt( 0, 1000 );
    
    mShimmerRate = randSource.getRandomBoundedDouble( 0.5, 1.0 );
    }



void Star::shiftBetween( Star *inA, Star *inB, int inNumSteps ) {
    mShiftA = inA;
    mShiftB = inB;
    mNumShiftSteps = inNumSteps;
    mNumShiftStepsDone = 0;
    }



void Star::step() {
    mShimmerTime ++;
    

    if( mShiftA == NULL ) {
        
        mFadeFactor = 0.5 + 0.3 * sin( mShimmerRate * mShimmerTime / 25.0 );
        
        mFadeFactor *= mBaseFade;
        }
    else {
        // shifting
        
        mNumShiftStepsDone ++;
        
        if( mNumShiftStepsDone > mNumShiftSteps ) {
            // stop shifting
            mShiftA = NULL;
            mShiftB = NULL;
            }
        else {
            

            double bBlend = mNumShiftStepsDone / (double)mNumShiftSteps;
            
            bBlend = smoothBlend( bBlend );
            

            double aBlend = 1 - bBlend;
            
            mX = mShiftA->mX * aBlend + mShiftB->mX * bBlend;
            mY = mShiftA->mY * aBlend + mShiftB->mY * bBlend;
            
            mFadeFactor = mShiftA->mFadeFactor * aBlend + 
                mShiftB->mFadeFactor * bBlend;
            }
        }
    
    }

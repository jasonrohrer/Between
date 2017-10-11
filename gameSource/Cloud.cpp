#include "Cloud.h"
#include "common.h"

#include "minorGems/util/random/StdRandomSource.h"

extern StdRandomSource randSource;



Cloud::Cloud()
        : mBaseFade( 1.0 ), mShiftA( NULL ), mShiftB( NULL ) {

    mXSpeed = randSource.getRandomBoundedDouble( 0.005, 0.001 );
    }



void Cloud::shiftBetween( Cloud *inA, Cloud *inB, int inNumSteps ) {
    mShiftA = inA;
    mShiftB = inB;
    mNumShiftSteps = inNumSteps;
    mNumShiftStepsDone = 0;
    }



void Cloud::draw( double inRotation, Vector3D *inPosition, 
                  double inScale,
                  double inFadeFactor,
                  Color *inColor ) {

    if( mShiftA == NULL ) {
        
        GameObject::draw ( inRotation, inPosition,
                           inScale, inFadeFactor, inColor );
        }
    else {

        double bBlend = mNumShiftStepsDone / (double)mNumShiftSteps;
        double aBlend = 1 - bBlend;
        
        

        // draw A underneath, but don't use full blend,
        // because both A and be might be transparent
        mShiftA->draw( inRotation, inPosition,
                       inScale, inFadeFactor * aBlend, inColor );
        
        // fade B in on top
        mShiftB->draw( inRotation, inPosition,
                       inScale, inFadeFactor * bBlend, inColor );

        }
    }



void Cloud::step() {
    
    if( mShiftA == NULL ) {

        mFadeFactor = mBaseFade;

        // can see the stepping, pixel, pixel, pixel
        //mX += mXSpeed;
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

#include "BlendedBlock.h"
#include "common.h"


BlendedBlock::BlendedBlock( Block *inA, Block *inB, int inNumTransitionSteps )
        : mA( inA ), mB( inB ), mStepsTotal( inNumTransitionSteps ),
          mStepsDone( 0 ), mDummy( false ) {
    
    if( mA != NULL ) {
        mDepthLayer = mA->mDepthLayer;
        }
    else if( mB != NULL ) {
        mDepthLayer = mB->mDepthLayer;
        }
    else {
        // dummy
        mDepthLayer = 0;
        mDummy = true;
        }
    
    
    }



double BlendedBlock::getFractionDone() {
    return (double)mStepsDone / (double)mStepsTotal;
    }



void BlendedBlock::draw( double inRotation, Vector3D *inPosition, 
                         double inScale,
                         double inFadeFactor,
                         Color *inColor ) {

    if( mDummy ) {
        return;
        }
    

    double blendB = (double)mStepsDone / (double)mStepsTotal;

    blendB = smoothBlend( blendB );
    
    double blendA = 1.0 - blendB;

    if( mA != NULL ) {
        
        mA->draw( inRotation, inPosition,
                  inScale, blendA * inFadeFactor,
                  inColor );
        }
    

    if( mB != NULL ) {
                
        mB->draw( inRotation, inPosition,
                  inScale, blendB * inFadeFactor,
                  inColor );
        }
    }



void BlendedBlock::step() {
    mStepsDone ++;
        
    if( mStepsDone > mStepsTotal ) {
        mStepsDone = mStepsTotal;
        }

    if( mDummy ) {
        return;
        }


    double blendB = (double)mStepsDone / (double)mStepsTotal;
    blendB = smoothBlend( blendB );
    
    double blendA = 1.0 - blendB;
    

    mX = 0;
    mY = 0;
    mScale = 1.0;
    
    if( mA != NULL && mB == NULL ) {
        mX = mA->mX;
        mY = mA->mY;

        mScale = mA->mScale;
        }
    else if( mA == NULL && mB != NULL ) {
        mX = mB->mX;
        mY = mB->mY;

        mScale = mB->mScale;
        }
    else if( mA != NULL &&  mB != NULL ) {
        
        mX = blendA * mA->mX + blendB * mB->mX;
        mY = blendA * mA->mY + blendB * mB->mY;

        mScale = blendA * mA->mScale + blendB * mB->mScale;
        }
    }

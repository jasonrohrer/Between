#include "GameObject.h"

#include "common.h"


void GameObject::shiftBetweenPages( int inPageA, int inPageB,
                                    int inNumSteps ) {
    mShiftingPages = true;
    mShiftPageA = inPageA;
    mShiftPageB = inPageB;
    mNumShiftSteps = inNumSteps;
    mNumShiftStepsDone = 0;
    }

    
        

void GameObject::draw( double inRotation, Vector3D *inPosition, 
                       double inScale,
                       double inFadeFactor,
                       Color *inColor ) {

    if( mSprite == NULL ) {
        return;
        }
    
    
    if( !mShiftingPages ) {
        
        mSprite->draw( mSpriteFrame, inRotation, inPosition,
                       inScale, inFadeFactor, inColor );
        }
    else {
        int oldPage = mSprite->getPage();
        

        double bBlend = mNumShiftStepsDone / (double)mNumShiftSteps;

        bBlend = smoothBlend( bBlend );
        

        // draw A at full blend
        mSprite->setPage( mShiftPageA );
        
        mSprite->draw( mSpriteFrame, inRotation, inPosition,
                       inScale, inFadeFactor, inColor );
        
        // fade B in on top
        mSprite->setPage( mShiftPageB );
        
        mSprite->draw( mSpriteFrame, inRotation, inPosition,
                       inScale, inFadeFactor * bBlend, inColor );

        // restore page
        mSprite->setPage( oldPage );
        }
    }



void GameObject::mainStep() {
    
    if( mShiftingPages ) {
        
        mNumShiftStepsDone ++;
    
        if( mNumShiftStepsDone > mNumShiftSteps ) {
            mShiftingPages = false;
            }
        }
    
    

    // call child's step function
    step();    
    }

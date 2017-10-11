#include "TargetBlock.h"

#include "minorGems/util/random/StdRandomSource.h"

extern StdRandomSource randSource;


void TargetBlock::initShimmerState( int inShimmerTime ) {
    mState = 0;
    
    if( inShimmerTime != -1 ) {
        mShimmerTime = inShimmerTime;
        }
    else {
        mShimmerTime = randSource.getRandomBoundedInt( 0, 1000 );
        }
    }



TargetBlock::TargetBlock( Image *inOverlayImage, const char *inColorSignature,
                          int inShimmerTime )
        : Block( inOverlayImage, inColorSignature ) {
    
    initShimmerState( inShimmerTime );
    
    
    // replace sprite with plain overlay
    delete mSprite;
    
    mSprite = new Sprite( mOverlayImage, false );
    
    }



TargetBlock::TargetBlock( Sprite *inSprite, const char *inColorSignature,
                          int inShimmerTime )
        : Block( inSprite, inColorSignature ) {
    
    initShimmerState( inShimmerTime );
    }



void TargetBlock::setState( int inState ) {
    mState = inState;
    }



TargetBlock *TargetBlock::copy() {
    return new TargetBlock( mOverlayImage, mColorSignature,
                            mShimmerTime );
    }



void TargetBlock::step() {
    mShimmerTime ++;

    switch( mState ) {
        case 0:
            mFadeFactor = 0.3 + 0.2 * sin( mShimmerTime / 25.0 );
            break;
        case 1:
            // faster
            mFadeFactor = 0.3 + 0.2 * sin( mShimmerTime / 5.0 );
            break;
        case 2:
            // invisible

            // but fade out gradually
            mFadeFactor -= 0.04;
            if( mFadeFactor < 0 ) {
                mFadeFactor = 0;
                }
            break;
        }
    
    }

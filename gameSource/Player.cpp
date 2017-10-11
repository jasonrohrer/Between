#include "Player.h"

#include "common.h"



Player::Player()
        : mShiftingPosition( false ) {

    const char *imageFileName = "player.tga";
    

    mSprite = new Sprite( imageFileName, true, 16, 4 );
    
    Image *spriteImage = readTGA( imageFileName );

    
    Image *whiteImage = spriteImage->copy();
    Image *brownImage = spriteImage->copy();
    
    // change all black pixels to white or light brown
    Color black( 0, 0, 0 );
    Color white( 1, 1, 1 );
    Color lightBrown( 191.0/255, 136.0/255, 0 );
    
    
    // change all dark grey pixels to light gray or dark brown 
    Color darkGrey( 54.0/255, 54.0/255, 54.0/255 );
    Color lightGrey( 182.0/255, 182.0/255, 182.0/255 );
    Color darkBrown( 141.0/255, 100.0/255, 0 );
    

    int w = spriteImage->getWidth();
    int h = spriteImage->getHeight();
    
    int numPixels = w * h;
    
    for( int i=0; i<numPixels; i++ ) {
        Color c = spriteImage->getColor( i );
        
        if( c.equals( &black ) ) {
            whiteImage->setColor( i, white );
            brownImage->setColor( i, lightBrown );
            }
        else if( c.equals( &darkGrey ) ) {
            whiteImage->setColor( i, lightGrey );
            brownImage->setColor( i, darkBrown );
            }
        }
    mWhiteSprite = new Sprite( whiteImage, true, 16, 4 );
    mBrownSprite = new Sprite( brownImage, true, 16, 4 );
    
    delete whiteImage;
    delete brownImage;
    
    delete spriteImage;
    
    
    mSprites[0] = mSprite;
    mSprites[1] = mWhiteSprite;
    mSprites[2] = mBrownSprite;
    }

        

Player::~Player() {
    delete mSprite;
    delete mWhiteSprite;
    delete mBrownSprite;
    }



void Player::shiftPosition( double inNewX, int inNumSteps ) {
    mShiftingPosition = true;
    mShiftPositionA = mX;
    mShiftPositionB = inNewX;
    mNumShiftPositionSteps = inNumSteps;
    mNumShiftPositionStepsDone = 0;
    }



void Player::step() {
    if( mShiftingPosition ) {
        
        mNumShiftPositionStepsDone ++;
    
        if( mNumShiftPositionStepsDone > mNumShiftPositionSteps ) {
            mShiftingPosition = false;
            }
        else {
            double bBlend = 
                mNumShiftPositionStepsDone / (double)mNumShiftPositionSteps;
            
            bBlend = smoothBlend( bBlend );
            
            double aBlend = 1 - bBlend;

            mX = mShiftPositionA * aBlend + mShiftPositionB * bBlend;
            }
        }
    }



void Player::draw( double inRotation, Vector3D *inPosition, 
                   double inScale,
                   double inFadeFactor,
                   Color *inColor ) {

    
    if( mX > -26.5 && mX < 24.5 ) {
        mSprite->draw( mSpriteFrame, inRotation, inPosition,
                       inScale, inFadeFactor, inColor );
        }

    if( mX < -22.5 ) {
        // fade in white sprite on top
        
        double fade = ( mX - (-22.5) ) / ( -26.5 - (-22.5) );
        
        
        if( fade > 1 ) {
            fade = 1;
            }
        

        mWhiteSprite->draw( mSpriteFrame, inRotation, inPosition,
                            inScale, 
                            fade * inFadeFactor, 
                            inColor );
        }

    if( mX > 20.5 ) {
        // fade in brown sprite on top
        
        double fade = ( mX - 20.5 ) / ( 24.5 - 20.5 );
        
        
        if( fade > 1 ) {
            fade = 1;
            }
        

        mBrownSprite->draw( mSpriteFrame, inRotation, inPosition,
                            inScale, 
                            fade * inFadeFactor, 
                            inColor );
        }
    
    }

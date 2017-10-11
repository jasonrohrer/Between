#ifndef GAME_OBJECT_INCLUDED
#define GAME_OBJECT_INCLUDED


#include "minorGems/graphics/Color.h"
#include "Sprite.h"


// superclass for all game objects
class GameObject {
    public:

        GameObject() 
                : mEnableSteps( true ),
                  mDepthLayer( 0 ), 
                  mX( 0 ), mY( 0 ), mRotation( 0 ), mScale( 1.0 ),
                  mFadeFactor( 1 ), mColor( 1, 1, 1 ), mSprite( NULL ),
                  mSpriteFrame( 0 ),
                  mShiftingPages( false ) {
            }
        

        virtual ~GameObject() {
            }
        

        // start a shift between sprite pages
        virtual void shiftBetweenPages( int inPageA, int inPageB,
                                        int inNumSteps );
        

        virtual void draw( double inRotation, Vector3D *inPosition, 
                           double inScale,
                           double inFadeFactor,
                           Color *inColor );
        

        // should not be overridden
        // calls child's step function
        void mainStep();
        

        // to be overridden by child classes
        virtual void step() {
            }
        
        
        
        char mEnableSteps;
        

        int mDepthLayer;
        

        double mX, mY;
        double mRotation;
        double mScale;
        
        double mFadeFactor;
        
        Color mColor;
        
        // can be NULL
        Sprite *mSprite;

        
        int mSpriteFrame;
        

        char mShiftingPages;
        int mShiftPageA;
        int mShiftPageB;
        int mNumShiftSteps;
        int mNumShiftStepsDone;
        
        
    };



#endif

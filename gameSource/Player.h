#ifndef PLAYER_OBJECT_INCLUDED
#define PLAYER_OBJECT_INCLUDED


#include "GameObject.h"
#include "common.h"



class Player : public GameObject {
    public:

        Player();
        

        ~Player();
        

        // these three functions set values for all sprites in this object

        void setFlipHorizontal( char inFlip ) {
            for( int i=0; i<mNumSprites; i++ ) {
                mSprites[i]->setFlipHorizontal( inFlip );
                }
            }

        void setHorizontalOffset( double inOffset ) {
            for( int i=0; i<mNumSprites; i++ ) {
                mSprites[i]->setHorizontalOffset( inOffset );
                }
            }

        void setPage( int inPage ) {
            for( int i=0; i<mNumSprites; i++ ) {
                mSprites[i]->setPage( inPage );
                }
            }

        
        // smooth blends to new position
        void shiftPosition( double inNewX, int inNumSteps );
        

        

        // overrides GameObject functions

        virtual void step();
        
        virtual void draw( double inRotation, Vector3D *inPosition, 
                           double inScale,
                           double inFadeFactor,
                           Color *inColor );
        
    private:
        
        // main sprite is in GameObject::mSprite

        // white version of main sprite
        Sprite *mWhiteSprite;

        // brown version of main sprite
        Sprite *mBrownSprite;

        static const int mNumSprites = 3;
        
        // pointers to sprites
        Sprite *mSprites[mNumSprites];
        

        char mShiftingPosition;
        double mShiftPositionA;
        double mShiftPositionB;
        int mNumShiftPositionSteps;
        int mNumShiftPositionStepsDone;
                
    };



#endif

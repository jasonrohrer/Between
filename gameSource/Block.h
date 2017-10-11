#ifndef BLOCK_INCLUDED
#define BLOCK_INCLUDED


#include "GameObject.h"



class Block : public GameObject {
    public:
        
        // inColor must be destroyed by caller
        Block( Color *inColor, char inColorCode );
        
        // constructs a block by copying an overlay image
        Block( Image *inOverlayImage, const char *inColorSignature );
        
        // constructs a block from a color signature only
        Block( const char *inColorSignature );
        
        // constructs a compound block out of 4 sub-blocks
        Block( Block *inBlocks[4] );
        

        // constructs a block using a sprite
        Block( Sprite *inSprite, const char *inColorSignature );
        


        virtual ~Block();


        Block *copy();
        
        virtual char equal( Block *inBlock );
        

        Image *getOverlayImage() {
            return mOverlayImage;
            }
        
        char *getColorSignature() {
            return mColorSignature;
            }
        
        
        

        // override default draw function
        void draw( double inRotation, Vector3D *inPosition, 
                   double inScale,
                   double inFadeFactor,
                   Color *inColor );
        
        // override step function
        virtual void step();
        

        // shift vertically
        void shiftTo( double inY );
        
        // cancel shift, if one is in progress
        void stopShifting();
        

        // sets fade to 0 and fades in gradually
        void fadeIn();
        

        // must be called before first block is constructed
        static void staticInit();
        // must be called after last block is destroyed
        static void staticFree();
        
        // utility function for creating color signatures
        static char *runLengthEncode( const char *inString );

        // utility function for decompressing color signatures
        static char *runLengthDecode( const char *inString );
        

    protected:

        // called by constructors
        void initFromOverlay();
        
        char *mColorSignature;
        

        Image *mOverlayImage;
        
        char mDeleteSprite;
        Sprite *mSprite;
        
        
        static Image *sBaseImage;
        static Image *sNoiseImage;
        
        char mShifting;
        
        double mShiftTargetY;
        
        char mFadingIn;
        
    };



#endif

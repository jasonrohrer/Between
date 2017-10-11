#ifndef SPRITE_INCLUDED
#define SPRITE_INCLUDED


#include "minorGems/graphics/openGL/SingleTextureGL.h"
#include "minorGems/graphics/Color.h"


#include "minorGems/math/geometry/Vector3D.h"

#include <stdlib.h>



class Sprite{
    public:
        
        // transparent color for RGB images can be taken from lower-left
        // corner pixel
        // lower-left corner ignored for RGBA images
        // image split vertically into inNumFrames of equal size
        Sprite( const char *inImageFileName,
                char inTransparentLowerLeftCorner = false,
                int inNumFrames = 1,
                int inNumPages = 1 );
        
        
        Sprite( Image *inImage,
                char inTransparentLowerLeftCorner = false,
                int inNumFrames = 1,
                int inNumPages = 1 );
        

        ~Sprite();
        

        void draw( int inFrame,
                   double inRotation, Vector3D *inPosition,
                   double inScale = 1,
                   double inFadeFactor = 1,
                   Color *inColor = NULL );
        

        int getNumFrames() {
            return mNumFrames;
            }
        


        void setFlipHorizontal( char inFlip ) {
            mFlipHorizontal = inFlip;
            }
        
        // used to adjust horizontal center point, in pixels
        void setHorizontalOffset( double inOffset ) {
            mHorizontalOffset = inOffset;
            }
        

        void setPage( int inPage ) {
            mCurrentPage = inPage;
            }


        int getPage() {
            return mCurrentPage;
            }
        
        

    protected:
        SingleTextureGL *mTexture;
        
        int mNumFrames;
        int mNumPages;
        
        double mBaseScaleX;
        double mBaseScaleY;

        char mFlipHorizontal;
        double mHorizontalOffset;
        
        int mCurrentPage;
        

        void initTexture( Image *inImage,
                          char inTransparentLowerLeftCorner = false,
                          int inNumFrames = 1,
                          int inNumPages = 1 );
        
        
    };



#endif

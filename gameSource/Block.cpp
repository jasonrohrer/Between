#include "Block.h"
#include "common.h"

#include "minorGems/util/random/StdRandomSource.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"



extern StdRandomSource randSource;



Image *Block::sBaseImage = NULL;
Image *Block::sNoiseImage = NULL;



void Block::staticInit() {
    sBaseImage = readTGA( "blockBase.tga" );
    sNoiseImage = readTGA( "blockNoise.tga" );
    }



void Block::staticFree() {
    delete sBaseImage;
    delete sNoiseImage;
    }



Block::Block( Color *inColor, char inColorCode ) {
    double r = inColor->r;
    double g = inColor->g;
    double b = inColor->b;
    
    // color signatures are run-length encoded
    mColorSignature = autoSprintf( "256%c", inColorCode );
    

    int w= sBaseImage->getWidth();
    int h=w;
    
    mOverlayImage = new Image( w, h, 3, false );
    
    int numPixels = w * h;
    
    for( int i=0; i<numPixels; i++ ) {
        mOverlayImage->getChannel( 0 )[i] = r;
        mOverlayImage->getChannel( 1 )[i] = g;
        mOverlayImage->getChannel( 2 )[i] = b;
        }

    initFromOverlay();
    }



Block::Block( Image *inOverlayImage, const char *inColorSignature ) {
    mOverlayImage = inOverlayImage->copy();
    mColorSignature = stringDuplicate( inColorSignature );
    
    initFromOverlay();
    }



char *Block::runLengthEncode( const char *inString ) {
    
    SimpleVector<char> encodedSig;

    int stringLength = strlen( inString );
    
    int i = 0;
    
    while( i < stringLength ) {
        
        char c = inString[i];
        
        int charCount = 1;
        i++;
        
        while( i < stringLength && inString[i] == c ) {
            charCount ++;
            i++;
            }
        if( charCount > 1 ) {
            // add string encoding of run length to vector

            char *countAsString = autoSprintf( "%d", charCount );
            
            int countStringLength = strlen( countAsString );
            for( int j=0; j<countStringLength; j++ ) {
                encodedSig.push_back( countAsString[j] );
                }

            delete [] countAsString;
            }
        
        encodedSig.push_back( c );
        }
    
    return encodedSig.getElementString();
    }



char *Block::runLengthDecode( const char *inString ) {

    SimpleVector<char> decodedSig;

    char *remainingString = (char *)inString;
        
    while( remainingString[0] != '\0' ) {
        
        int runLength = 0;
        char code = ' ';
        

        int numRead = sscanf( remainingString, "%d%c", &runLength, &code );
        
        if( numRead != 2 ) {
            // a run of length 1?
            numRead = sscanf( remainingString, "%c", &code );
            
            if( numRead == 1 ) {
                runLength = 1;
                }
            }

        
        for( int r=0; r<runLength; r++ ) {
            decodedSig.push_back( code );
            }
        
        // skip past code

        remainingString = strchr( remainingString, code );
        
        remainingString = &( remainingString[1] );
        }
    
    return decodedSig.getElementString();
    }



Block::Block( const char *inColorSignature ) {
    mColorSignature = stringDuplicate( inColorSignature );
    
    
    char *decodedSig = runLengthDecode( inColorSignature );
    

    int w= sBaseImage->getWidth();
    int h=w;
    
    mOverlayImage = new Image( w, h, 3, false );
    
    int numPixels = w * h;
    
    int numCodes = strlen( decodedSig );
    
    int numSteps = numPixels;
    
    if( numPixels != numCodes ) {
        
        printf( "Error:  color signature not right length\n" );
        
        // use shorter to avoid overrun

        if( numCodes < numPixels ) {
            numSteps = numCodes;
            }
        }
    

    for( int i=0; i<numSteps; i++ ) {
        mOverlayImage->setColor( i, 
                                 codeToColor( decodedSig[ i ] ) );
        }

    delete [] decodedSig;
    
    initFromOverlay();
    }



Block::Block( Block *inBlocks[4] ) {
    // arrange like
    // [0 1]
    // [2 3]
     
    int w= sBaseImage->getWidth();
    int h=w;
    
    mOverlayImage = new Image( w, h, 3, false );
    
    //int numPixels = w * h;
    
    // construct full source signatures
    char fullSourceSignatures[4][256];
    

    int s;
    
    for( s=0; s<4; s++ ) {
        char *sig = inBlocks[s]->getColorSignature();

        char *fullSig = runLengthDecode( sig );
        
        unsigned int lengthToCopy = 256;
        
        if( strlen( fullSig ) < lengthToCopy ) {
            lengthToCopy = strlen( fullSig );
        
            printf( "Error:  full block signature length = %d, "
                    "expecting 256\n", lengthToCopy );
            }
        memcpy( fullSourceSignatures[s], fullSig, lengthToCopy );
        
        delete [] fullSig;
        }
    
        
    // now build a full overlay signature for this block
    // along with the overlay image (indexing color codes just like
    // we index pixels)
    // leave room at end for \0
    char fullOverlaySignature[257];
    

        
    for( int y=0; y<h; y++ ) {
        for( int x=0; x<w; x++ ) {
            int overlayIndex = y * w + x;
                
            int sourceBlock = 0;
            int xOffset = 0;
            int yOffset = 0;
                
            if( y < h/2 ) {
                yOffset = 0;
                    
                if( x < w/2 ) {
                    sourceBlock = 0;
                    xOffset = 0;
                    }
                else {
                    sourceBlock = 1;
                    xOffset = w/2;
                    }
                }
            else {
                yOffset = h/2;
                    
                if( x < w/2 ) {
                    sourceBlock = 2;
                    xOffset = 0;
                    }
                else {
                    sourceBlock = 3;
                    xOffset = w/2;
                    }
                }
                
            Image *sourceImage = 
                inBlocks[ sourceBlock ]->getOverlayImage();
                        

            // nearest neighbor scaling (skip every other pixel)
            int sourceX = (x - xOffset) * 2;
            int sourceY = (y - yOffset) * 2;
            
            int sourceIndex = sourceY * w + sourceX;            
            
            fullOverlaySignature[ overlayIndex ] = 
                    fullSourceSignatures[ sourceBlock ][ sourceIndex ];
                

            for( int c=0; c<3; c++ ) {    
                double *overlayChannel = mOverlayImage->getChannel( c );
                double *sourceChannel = sourceImage->getChannel( c );
                

                overlayChannel[ overlayIndex ] = sourceChannel[ sourceIndex ];
                }
            }
        }
    
    
    fullOverlaySignature[256] = '\0';
    

    /*
    // debug:
    // check it
    for( int i=0; i<256; i++ ) {
        char code = fullOverlaySignature[i];
        
        Color c = codeToColor( code );
        
        Color imageC = mOverlayImage->getColor( i );
        
        if( !imageC.equals( &c ) ) {
            printf( "Constructed signature does not match image at index "
                    "%d:  "
                    "code = %c\n", i, code );
            c.print();
            printf( "\n" );
            imageC.print();
            printf( "\n" );
            }
        }
    */


    mColorSignature = runLengthEncode( fullOverlaySignature );
    

    initFromOverlay();    
    }



// constructs a block using a sprite
Block::Block( Sprite *inSprite, const char *inColorSignature ) {
    
    mShifting = false;
    mFadingIn = false;

    
    mScale = 1.0 / 16.0;

    
    mColorSignature = stringDuplicate( inColorSignature );

    mOverlayImage = NULL;
    
    mDeleteSprite = false;
    mSprite = inSprite;
    }



void Block::initFromOverlay(){
    //printf( "New block: %s\n", mColorSignature );
    
    mShifting = false;
    mFadingIn = false;

    
    mScale = 1.0 / 16.0;
    

    // pick a different frame to source each quadrant
    int baseSpriteFrame[4];
    int q;
    for( q=0; q<4; q++ ) {
        baseSpriteFrame[q] =
            randSource.getRandomBoundedInt( 
                0, 3 );
        }
    
    
    
        

    // DONE:
    // Probably should copy background texture and modulate it with
    // overlay directly
    // There's no way to modulate the color of a drawn polygon with 
    // another drawn polygon while still paying attention to the alpha
    // mask of the underlying polygon
    // Can use multiplicative blending, but that ignores the underlying
    // alpha.  Even if we copied the alpha of the background texture
    // and used it as the alpha for the overlay, multiplicative blending
    // would cause it to be ignored.
    // Every block has a texture anyway, so we're not wasting texture
    // memory by doing this.    
    int w= sBaseImage->getWidth();
    int h=w;
    
    int numPixels = w * h;
    

    // jump to a random line in noise image
    // leaving room for a full WxH image starting at that line
    int noiseImageHeight = sNoiseImage->getHeight();
    
    int noiseSpriteLineA = 
        randSource.getRandomBoundedInt( 
            0, noiseImageHeight - h - 1 );
    int noiseSpriteLineB = 
        randSource.getRandomBoundedInt( 
            0, noiseImageHeight - h - 1 );



    // mix overlay image with base image and build alpha
    Image spriteImage( w, h, 5, false );
    
    // find transparency color

    double *baseR = sBaseImage->getChannel( 0 );
    double *baseG = sBaseImage->getChannel( 1 );
    double *baseB = sBaseImage->getChannel( 2 );
        
    // index of transparency
    int tI = 
        sBaseImage->getWidth() * 
        ( sBaseImage->getHeight() - 1 );
        
    // color of transparency
    double tR = baseR[tI];
    double tG = baseG[tI];
    double tB = baseB[tI];

    int baseOffsetInQuad[4];
    
    for( q=0; q<4; q++ ) {
        baseOffsetInQuad[q] = baseSpriteFrame[q] * numPixels;
        }
    
    int noiseOffsetA = noiseSpriteLineA * w;
    int noiseOffsetB = noiseSpriteLineB * w;
    
    double *noiseR = sNoiseImage->getChannel( 0 );
    double *noiseG = sNoiseImage->getChannel( 1 );
    double *noiseB = sNoiseImage->getChannel( 2 );
    

    double *overlayR = mOverlayImage->getChannel( 0 );
    double *overlayG = mOverlayImage->getChannel( 1 );
    double *overlayB = mOverlayImage->getChannel( 2 );
    
    double *spriteR = spriteImage.getChannel( 0 );
    double *spriteG = spriteImage.getChannel( 1 );
    double *spriteB = spriteImage.getChannel( 2 );
    double *spriteA = spriteImage.getChannel( 3 );

    int halfW = w / 2;
    int halfH = h / 2;
    
    for( int i=0; i<numPixels; i++ ) {
        int y = i / w;
        int x = i % w;
        
        if( x < halfW ) {
            if( y < halfH ) {
                q = 0;
                }
            else {
                q = 1;
                }
            }
        else {
            if( y < halfH ) {
                q = 2;
                }
            else {
                q = 3;
                }
            }
        
        int baseOffset = baseOffsetInQuad[q];
        

        spriteR[i] = 
            overlayR[i] 
            * baseR[ i + baseOffset ]
            * noiseR[ i + noiseOffsetA ]
            * noiseR[ i + noiseOffsetB ];
        
        spriteG[i] = 
            overlayG[i] 
            * baseG[ i + baseOffset ]
            * noiseG[ i + noiseOffsetA ]
            * noiseG[ i + noiseOffsetB ];
        
        spriteB[i] = 
            overlayB[i] 
            * baseB[ i + baseOffset ]
            * noiseB[ i + noiseOffsetA ]
            * noiseB[ i + noiseOffsetB ];
        

        if( baseR[ i + baseOffset ] == tR
            &&
            baseG[ i + baseOffset ] == tG
            &&
            baseB[ i + baseOffset ] == tB ) {
            
            spriteA[i] = 0;
            }
        else {
            spriteA[i] = 1;
            }
        }
    
            
    mDeleteSprite = true;
    
    mSprite = new Sprite( &spriteImage, false );
    }

    
        
Block::~Block() {
    if( mOverlayImage != NULL ) {
        delete mOverlayImage;
        }
    
    if( mDeleteSprite ) { 
        delete mSprite;
        }
    
    delete [] mColorSignature;
    }



Block *Block::copy() {
    if( mOverlayImage != NULL ) {
        return new Block( mOverlayImage, mColorSignature );
        }
    else if( !mDeleteSprite ) {
        return new Block( mSprite, mColorSignature );
        }
    else {
        printf( 
            "Error:  neither sprite nor overlay usable for Block::copy()." );
        
        return NULL;
        }
    }



char Block::equal( Block *inBlock ) {
    int result = strcmp( mColorSignature, inBlock->getColorSignature() );
    
    return ( result == 0 );
    }


        
void Block::draw( double inRotation, Vector3D *inPosition, 
                  double inScale,
                  double inFadeFactor,
                  Color *inColor ) {

    mSprite->draw( 0, inRotation, inPosition,
                   inScale, inFadeFactor, inColor );
    }



void Block::step() {
    if( mShifting ) {
        if( mY > mShiftTargetY ) {
            
            mY -= 0.04;
            if( mY < mShiftTargetY ) {
                mY = mShiftTargetY;
                mShifting = false;
                }
            }
        else if( mY < mShiftTargetY ) {
            
            mY += 0.04;
            if( mY > mShiftTargetY ) {
                mY = mShiftTargetY;
                mShifting = false;
                }
            }
        else {
            mShifting = false;
            }
        }
    
    if( mFadingIn ) {
        mFadeFactor += 0.04;
        if( mFadeFactor > 1 ) {
            mFadeFactor = 1;
            mFadingIn = false;
            }
        }
    }



void Block::shiftTo( double inY ) {
    mShifting = true;
    mShiftTargetY = inY;
    }



void Block::stopShifting() {
    if( mShifting ) {
        mShiftTargetY = mY;
        mShifting = false;
        }
    }



void Block::fadeIn() {
    mFadingIn = true;
    mFadeFactor = 0;
    }




        


#include "NightBackground.h"


NightBackground::NightBackground( Color inColor, int inDirection )
        :mColor( inColor ), mDirection( inDirection ) {

    }




void NightBackground::draw( double inRotation, Vector3D *inPosition, 
                            double inScale,
                            double inFadeFactor,
                            Color *inColor ) {
    
    
    // radius of one stripe
    double xRadius = inScale * 0.5 * 16;
    
    double yRadius = inScale * 5 * 16;
    

    // how far blackness stretches
    double farReachX = mDirection * inScale * 23 * 16 + inPosition->mX;
    

    Vector3D corners[4];

    // first, set up corners relative to 0,0
    corners[0].mX = - xRadius;
    corners[0].mY = - yRadius;
    corners[0].mZ = 0;

    corners[1].mX = xRadius;
    corners[1].mY = - yRadius;
    corners[1].mZ = 0;

    corners[2].mX = xRadius;
    corners[2].mY = yRadius;
    corners[2].mZ = 0;

    corners[3].mX = - xRadius;
    corners[3].mY = yRadius;
    corners[3].mZ = 0;

    int i;
    

    // add inPosition so that center is at inPosition
    
    for( i=0; i<4; i++ ) {
        corners[i].add( inPosition );
        }

    int numStripes = 7;

    Vector3D stripeOffset( mDirection * 2 * xRadius, 0, 0 );
    


    glBegin( GL_QUADS ); {
    
        for( int s=1; s<numStripes; s++ ) {
            
            glColor4d( mColor.r, mColor.g, mColor.b, 
                       s / (double)(numStripes-1) );
            glVertex2d( corners[0].mX, corners[0].mY );

        
            glVertex2d( corners[1].mX, corners[1].mY );
            
        
            glVertex2d( corners[2].mX, corners[2].mY );
            
        
            glVertex2d( corners[3].mX, corners[3].mY );
        

            for( i=0; i<4; i++ ) {
                corners[i].add( &stripeOffset );
                }
            }
        

        // draw solid color farther out
        glColor4d( mColor.r, mColor.g, mColor.b, 1 );

        int a, b;
        
        if( mDirection == -1 ) {
            a = 1;
            b = 2;
            }
        else {
            a = 0;
            b = 3;
            }

        glVertex2d( corners[a].mX, corners[a].mY );
        glVertex2d( corners[b].mX, corners[b].mY );
        
        glVertex2d( farReachX, corners[b].mY );
        glVertex2d( farReachX, corners[a].mY );
        
        }
    glEnd();
    }

    

#include "World.h"



World::~World() {
    int numObjects = mObjects.size();
    for( int i=0; i<numObjects; i++ ) {
        delete *( mObjects.getElement( i ) );
        }    
    }

        

void World::add( GameObject *inObject ) {
    mObjects.push_back( inObject );
    }



void World::remove( GameObject *inObject ) {
    mObjects.deleteElementEqualTo( inObject );
    }



double numBlocksOnScreen = 10;


void World::draw( double inScreenCenterX ) {
    int numObjects = mObjects.size();
    //printf( "Drawing %d objects\n", numObjects );
    

    int i;


    // search for deepest layer
    int thisDepthLayer = 0;
    
    for( i=0; i<numObjects; i++ ) {
        GameObject *o = *( mObjects.getElement( i ) );
        
        if( o->mDepthLayer > thisDepthLayer ) {
            thisDepthLayer = o->mDepthLayer;
            }
        }
    
    int nextDepthLayer = thisDepthLayer;
    thisDepthLayer += 1;

    // found with profiler
    // Construct one vector outside of loop
    Vector3D screenPos( 0, 0, 0 );
    
    
    while( nextDepthLayer != thisDepthLayer ) {
    
        thisDepthLayer = nextDepthLayer;
        

        for( i=0; i<numObjects; i++ ) {
            GameObject *o = *( mObjects.getElement( i ) );

                        
            if( o->mDepthLayer == thisDepthLayer ) {
                
                // translate into screen coords

                screenPos.mX = ( o->mX - inScreenCenterX ) / numBlocksOnScreen;
                screenPos.mY = o->mY / numBlocksOnScreen 
                    + (0.5 / numBlocksOnScreen)
                    - 0.5;
                                
                double scale = o->mScale / numBlocksOnScreen;
                    
                //printf( "Drawing sprite at %f,%f, %f\n", x, y, scale );
                    
                o->draw( o->mRotation, &screenPos, 
                         scale,
                         o->mFadeFactor,
                         &( o->mColor ) );
                }
            else if( o->mDepthLayer < thisDepthLayer ) {
                if( nextDepthLayer == thisDepthLayer ) {
                    nextDepthLayer = o->mDepthLayer;
                    }
                else if( o->mDepthLayer > nextDepthLayer ) {
                    nextDepthLayer = o->mDepthLayer;
                    }
                }
            }
        }
    }



void World::step() {
    int numObjects = mObjects.size();
    for( int i=0; i<numObjects; i++ ) {
        GameObject *o = *( mObjects.getElement( i ) );
        if( o->mEnableSteps ) {
            o->mainStep();
            }
        }    
    }

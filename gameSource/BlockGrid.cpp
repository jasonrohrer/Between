#include "BlockGrid.h"

#include "minorGems/util/stringUtils.h"


void BlockGrid::initBlocks() {
    for( int r=0; r<mBlockRows; r++ ) {
        for( int c=0; c<mBlockColumns; c++ ) {
            mBlocks[r][c] = NULL;
            }
        }
    
    clearDirtyFlags();
    }



BlockGrid::BlockGrid()
        : mWorld( NULL ),
          mDepthLayer( 0 ) {
    initBlocks();
    }


    
BlockGrid::BlockGrid( World *inWorld )
        : mWorld( inWorld ),
          mDepthLayer( 0 ) {    
    initBlocks();
    }

        

BlockGrid::~BlockGrid() {
    }



void BlockGrid::setWorld( World *inWorld ) {
    mWorld = inWorld;
    }



void BlockGrid::setDepthLayer( int inDepthLayer ) {
    mDepthLayer = inDepthLayer;
    }


        
char BlockGrid::add( Block *inBlock, int inX, int inY ) {
    // 0,0 at center of our grid
    
    int c = inX + mBlockColumns / 2;
    int r = inY - 1;
    
    if( c>=0 && c < mBlockColumns && r>=0 && r < mBlockRows ) {
        // in bounds

        if( ! isColumnFull( inX ) ) {
            // there's room
            
    
            inBlock->mX = inX;
            inBlock->mY = inY;
            inBlock->mDepthLayer = mDepthLayer;
            

            mWorld->add( inBlock );
            
            if( mBlocks[r][c] != NULL ) {
                // other blocks in way
                                
                // cause all blocks at or above to rise
                for( int r2=mBlockRows-2; r2>=r; r2-- ) {
                    if( mBlocks[r2][c] != NULL ) {
                        Block *b = mBlocks[r2][c];
                
                        // put in new grid spot
                        mBlocks[r2 + 1][c]= b;
                        mBlocks[r2][c] = NULL;
                        
                        // rise to new world position
                        b->shiftTo( (r2 + 1) + 1 );
                        }
                    }
                }

            mBlocks[r][c] = inBlock;
            mDirtyFlags[c] = true;
            
            return true;
            }
        }
    // failed to add
    return false;
    }



SimpleVector<Block *> *BlockGrid::getAllBlocks() {
    
    SimpleVector<Block *> *returnVector = new SimpleVector<Block *>();
    

    for( int r=0; r<mBlockRows; r++ ) {
        for( int c=0; c<mBlockColumns; c++ ) {
            if( mBlocks[r][c] != NULL ) {
                returnVector->push_back( mBlocks[r][c] );
                }
            }
        }
    
    return returnVector;
    }



Block *BlockGrid::getBlock( int inX, int inY ) {
    int c = worldToGridC( inX );
    int r = worldToGridR( inY );
    
    if( c>=0 && c < mBlockColumns && r>=0 && r < mBlockRows ) {
        // in bounds
        
        return mBlocks[r][c];
        }    

    return NULL;
    }



Block *BlockGrid::removeBlock( int inX, int inY ) {
    int c = worldToGridC( inX );
    int r = worldToGridR( inY );
    
    
    if( c>=0 && c < mBlockColumns && r>=0 && r < mBlockRows ) {
        // in bounds
        
        Block *returnBlock = mBlocks[r][c];

        mBlocks[r][c] = NULL;
        
        mWorld->remove( returnBlock );
        
        // cancel any shift for this block
        returnBlock->stopShifting();
        
        
        // cause all blocks above to fall
        for( int r2=r+1; r2<mBlockRows; r2++ ) {
            if( mBlocks[r2][c] != NULL ) {
                Block *b = mBlocks[r2][c];
                
                // put in new grid spot
                mBlocks[r2 - 1][c]= b;
                mBlocks[r2][c] = NULL;
                
                // fall to new world position
                b->shiftTo( (r2 - 1) + 1 );
                }
            }

        mDirtyFlags[c] = true;
        
        return returnBlock;
        }    

    return NULL;
    }



char BlockGrid::isColumnFull( int inX ) {
    int c = worldToGridC( inX );
    
    if( c>=0 && c < mBlockColumns ) {
        // in bounds
        // check top spot only
        if( mBlocks[ mBlockRows - 1 ][c] != NULL ) {
            return true;
            }
        else {
            return false;
            }
        }
    else {
        // out of bounds always full
        return true;
        }
    
    }



SimpleVector<char*> *BlockGrid::getDirtyColumns() {
    SimpleVector<char*> *returnVector = new SimpleVector<char *>();
    

    for( int c=0; c<mBlockColumns; c++ ) {
        if( mDirtyFlags[c] ) {
            
            char *columnString = autoSprintf( "%d", c );
            
            int r=0;
            while( r<mBlockRows && mBlocks[r][c] != NULL ) {
                
                char *blockString = 
                    autoSprintf( "_%s", mBlocks[r][c]->getColorSignature() );
                
                
                char *newColumnString = concatonate( columnString, 
                                                     blockString );
                
                delete [] blockString;
                delete [] columnString;
                columnString = newColumnString;
                
                r++;
                }

            if( r == 0 ) {
                // empty column
                
                // add _ after column number

                delete [] columnString;
                
                columnString = autoSprintf( "%d_", c );
                }

            
            returnVector->push_back( columnString );
            }
        }

    return returnVector;
    }

        

void BlockGrid::clearDirtyFlags() {
    for( int c=0; c<mBlockColumns; c++ ) {
        mDirtyFlags[c] = false;
        }
    }

        

void BlockGrid::setColumns( SimpleVector<char*> *inColumns ) {
    
    int numToSet = inColumns->size();
    

    for( int i=0; i<numToSet; i++ ) {
        
        int numParts;
        

        char **parts = 
            split( *( inColumns->getElement( i ) ), "_", &numParts );
        
        // first part is column number
        if( numParts > 0 ) {
            
            int c;
        
        
            int numRead = sscanf( parts[0], "%d", &c );
            
            if( numRead == 1 ) {
                
                // first clear the column
                
                int r=0;
                
                while( r<mBlockRows && mBlocks[r][c] != NULL ) {
                    Block *b = mBlocks[r][c];

                    mBlocks[r][c] = NULL;
        
                    mWorld->remove( b );
                    delete b;
                    
                    r++;
                    }
                
                // now see if the new column has any blocks
                
                for( int p=1; p<numParts; p++ ) {

                    // make sure part not empty
                    if( strcmp( parts[p], "" ) != 0 ) {
                        
                        r = p-1;
                    
                        // add blocks here
                        
                        Block *b = new Block( parts[p] );
                        
                        b->mX = gridToWorldX( c );
                        b->mY = gridToWorldY( r );
                        
                        b->mDepthLayer = mDepthLayer;
                    
                        mBlocks[r][c] = b;
                        
                        mWorld->add( b );
                        }
                    }
                
                }
            }
        
        
        for( int p=0; p<numParts; p++ ) {
            delete [] parts[p];
            }
        delete [] parts;
        
        
        }
    
    
    }




#ifndef BLOCK_GRID_INCLUDED
#define BLOCK_GRID_INCLUDED


#include "minorGems/util/SimpleVector.h"

#include "Block.h"
#include "World.h"


class BlockGrid {
    public:

        // sets world to NULL
        // setWorld must be called before using the resulting class
        BlockGrid();
        
        // the world that this grid is in
        BlockGrid( World *inWorld );

        ~BlockGrid();

        void setWorld( World *inWorld );
        
        void setDepthLayer( int inDepthLayer );
        
        
        // blocks destroyed when corresponding world destroyed
        // return true if add succeeded
        char add( Block *inBlock, int inX, int inY );

        
        // gets all blocks managed by this grid
        SimpleVector<Block *> *getAllBlocks();
        

        // can return NULL
        Block *getBlock( int inX, int inY );
        
        Block *removeBlock( int inX, int inY );
                

        // is the column at world position inX full?
        char isColumnFull( int inX );
        
        
        
        // for synching with server
        
        // gets all dirty columns as strings in format that matches
        // server's protocol.txt specification
        // vector and strings destroyed by caller
        SimpleVector<char*> *getDirtyColumns();
        

        void clearDirtyFlags();
        
        
        // sets columns using server formatted column strings
        void setColumns( SimpleVector<char*> *inColumns );
        


    protected:
        
        World *mWorld;

        // depth layer to set for all blocks added to this grid
        int mDepthLayer;
        
        
        static const int mBlockColumns = 40;
        static const int mBlockRows = 9;
        
        Block *mBlocks[mBlockRows][mBlockColumns];
        
        char mDirtyFlags[mBlockColumns];
        
        
        void initBlocks();
        
        

        // coordinate conversion functions
        
        int worldToGridC( int inX ) {
            return inX + mBlockColumns / 2;
            }
        
        int worldToGridR( int inY ) {
            return inY - 1;
            }

        int gridToWorldX( int inC ) {
            return inC - mBlockColumns / 2;
            }
        
        int gridToWorldY( int inR ) {
            return inR + 1;
            }
        
        
        
    };



#endif

#ifndef GRID_TRANSITION_INCLUDED
#define GRID_TRANSITION_INCLUDED


#include "BlockGrid.h"
#include "BlendedBlock.h"
#include "World.h"


class GridTransition {
    public:

        GridTransition( BlockGrid *inA, BlockGrid *inB, World *inWorld,
                        int inNumTransitionSteps );
        
        // removes grid's blocks from inWorld
        ~GridTransition();
        

        char isDone();
        

    protected:
        World *mWorld;
        
        // used to check if blend done
        BlendedBlock *mFirstBlendedBlock;
        
        SimpleVector<BlendedBlock *> mBlocks;
        
        
    };



#endif

#ifndef BUILDER_TARGET_BLOCK_INCLUDED
#define BUILDER_TARGET_BLOCK_INCLUDED


#include "TargetBlock.h"


// idea:
// -- override constructor to take a sprite (do this in Block too )
// -- override "equals" function to always return true
// -- leave setState and step alone!


class BuilderTargetBlock : public TargetBlock {
    public:
        
        BuilderTargetBlock( Sprite *inSprite, int inShimmerTime = -1 );
        

        // override
        virtual char equal( Block *inBlock ) {
            return true;
            }



        
        // subclass copy function
        BuilderTargetBlock *copy();



        // use TargetBlock's step and setState functions
        
    };



#endif

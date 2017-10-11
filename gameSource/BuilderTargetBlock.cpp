

#include "BuilderTargetBlock.h"



#include "minorGems/util/random/StdRandomSource.h"

extern StdRandomSource randSource;



BuilderTargetBlock::BuilderTargetBlock( Sprite *inSprite, int inShimmerTime )
        : TargetBlock( inSprite, "256A", inShimmerTime ) {
    }



BuilderTargetBlock *BuilderTargetBlock::copy() {
    return new BuilderTargetBlock( mSprite, mShimmerTime );
    }


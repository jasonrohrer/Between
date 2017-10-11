#ifndef TARGET_BLOCK_INCLUDED
#define TARGET_BLOCK_INCLUDED


#include "Block.h"



class TargetBlock : public Block {
    public:
        
        TargetBlock( Image *inOverlayImage, const char *inColorSignature,
                     int inShimmerTime = -1 );
        
        TargetBlock( Sprite *inSprite, const char *inColorSignature,
                     int inShimmerTime = -1 );
        

        // set state
        // 0 = unmatched
        // 1 = mismatched
        // 2 = perfect match
        void setState( int inState );
        
        
        // subclass copy function
        TargetBlock *copy();
        

        // override step function
        virtual void step();
        

    protected:

        int mState;
        
        int mShimmerTime;
        

        void initShimmerState( int inShimmerTime );
        
    };



#endif

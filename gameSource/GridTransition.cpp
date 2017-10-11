#include "GridTransition.h"


GridTransition::GridTransition( BlockGrid *inA, BlockGrid *inB, World *inWorld,
                                int inNumTransitionSteps )
    : mWorld( inWorld ),
      mFirstBlendedBlock( NULL ) {

    SimpleVector<Block *> *blocksA = inA->getAllBlocks();
    SimpleVector<Block *> *blocksB = inB->getAllBlocks();
    

    int indexA = 0;
    int indexB = 0;
    
    char doneA = false;
    char doneB = false;

    while( !( doneA && doneB ) ) {
        
        if( indexA == blocksA->size() ) {
            doneA = true;
            indexA = 0;
            }
        if( indexB == blocksB->size() ) {
            doneB = true;
            indexB = 0;
            }

        if( !( doneA && doneB ) ) {
        
            Block *blockA = NULL;
            Block *blockB = NULL;

            if( indexA < blocksA->size() ) {
                blockA = *( blocksA->getElement( indexA ) );
                indexA ++;
                }
            if( indexB < blocksB->size() ) {
                blockB = *( blocksB->getElement( indexB ) );
                indexB ++;
                }
            
            if( blockA != NULL || blockB != NULL ) {
            
                BlendedBlock * blockBlend = 
                    new BlendedBlock( blockA, blockB,
                                      inNumTransitionSteps );
            
                if( mFirstBlendedBlock == NULL ) {
                    mFirstBlendedBlock = blockBlend;
                    }
            
                mWorld->add( blockBlend );
                mBlocks.push_back( blockBlend );
                }
            }
        
        }
    
    if( blocksA->size() == 0 && blocksB->size() == 0 ) {
        // empty to empty transition

        // one dummy block
        mFirstBlendedBlock = new BlendedBlock( NULL, NULL, 
                                               inNumTransitionSteps );

        mWorld->add( mFirstBlendedBlock );
        mBlocks.push_back( mFirstBlendedBlock );
        }
    

    delete blocksA;
    delete blocksB;
    }



GridTransition::~GridTransition() {
    for( int i=0; i<mBlocks.size(); i++ ) {
        BlendedBlock *block = *( mBlocks.getElement( i ) );
    
        mWorld->remove( block );
        delete block;
        }
    }



char GridTransition::isDone() {
    if( mFirstBlendedBlock == NULL ) {
        return true;
        }
    else {
        return mFirstBlendedBlock->isDone();
        }
    }

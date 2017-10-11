#ifndef BLENDED_BLOCK_INCLUDED
#define BLENDED_BLOCK_INCLUDED


#include "GameObject.h"
#include "Block.h"



class BlendedBlock : public GameObject {
    public:
        
        // if inA is NULL, block fades in in place
        // if inB is NULL, block fades out in place
        // both NULL makes a dummy block that isn't drawn (but still counts 
        //  steps)
        BlendedBlock( Block *inA, Block *inB, int inNumTransitionSteps );
        


        char isDone() {
            return ( mStepsDone == mStepsTotal );
            }


        double getFractionDone();
        
            
        // override default draw function
        void draw( double inRotation, Vector3D *inPosition, 
                   double inScale,
                   double inFadeFactor,
                   Color *inColor );
        
        // override step function
        virtual void step();
                

    protected:

        Block *mA;
        Block *mB;
        

        int mStepsTotal;
        
        int mStepsDone;
        
        char mDummy;
        
        
    };



#endif

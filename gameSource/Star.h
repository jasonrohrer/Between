#ifndef STAR_OBJECT_INCLUDED
#define STAR_OBJECT_INCLUDED


#include "GameObject.h"



class Star : public GameObject {
    public:
        

        Star();
        

        // shifts between position and fade of start and end stars
        void shiftBetween( Star *inA, Star *inB, int inNumSteps );
        

        // override step function
        virtual void step();
        

        double mBaseFade;
        

    protected:
        
        int mShimmerTime;
        double mShimmerRate;

        Star *mShiftA;
        Star *mShiftB;
        
        int mNumShiftSteps;
        int mNumShiftStepsDone;
        
        
    };



#endif

#ifndef CLOUD_OBJECT_INCLUDED
#define CLOUD_OBJECT_INCLUDED


#include "GameObject.h"



class Cloud : public GameObject {
    public:
        

        Cloud();
        

        // shifts between position, fade, and sprite of start and end clouds
        void shiftBetween( Cloud *inA, Cloud *inB, int inNumSteps );
        
        
        // override draw function
        virtual void draw( double inRotation, Vector3D *inPosition, 
                           double inScale,
                           double inFadeFactor,
                           Color *inColor );

        // override step function
        virtual void step();
        

        double mBaseFade;
        

    protected:
        
        
        Cloud *mShiftA;
        Cloud *mShiftB;
        
        int mNumShiftSteps;
        int mNumShiftStepsDone;
        
        
        double mXSpeed;
        
    };



#endif

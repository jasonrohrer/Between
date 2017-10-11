#ifndef NIGHT_BACKGROUND_INCLUDED
#define NIGHT_BACKGROUND_INCLUDED


#include "GameObject.h"



// originally just for night background, but now more general
class NightBackground : public GameObject {
    public:
        
        // direction -1 or +1
        NightBackground( Color inColor, int inDirection );
        

        virtual void draw( double inRotation, Vector3D *inPosition, 
                           double inScale,
                           double inFadeFactor,
                           Color *inColor );
        

    private:
        
        Color mColor;
        int mDirection;
        
    };



#endif

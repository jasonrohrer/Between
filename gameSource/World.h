#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED


#include "minorGems/util/SimpleVector.h"

#include "GameObject.h"



class World {
    public:
        

        ~World();
        

        // inObject will be destroyed when World is destroyed
        void add( GameObject *inObject );

        // inObject is NOT destroyed by this call
        void remove( GameObject *inObject );
        

        // draws all objects in the world
        // screen centered on world x position inScreenCenterX
        void draw( double inScreenCenterX );
        
        // steps all objects in world
        void step();
        
        

    protected:
        
        SimpleVector<GameObject*> mObjects;
        
        

    };



#endif

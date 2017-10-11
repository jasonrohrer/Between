int mainFunction( int inArgCount, char **inArgs );

int main( int inArgCount, char **inArgs ) {
    return mainFunction( inArgCount, inArgs );
    }


#include <SDL/SDL.h>


#include "musicPlayer.h"

#include "minorGems/system/Thread.h"

double musicTrackFadeLevel = 9 / 9.0;


int mainFunction( int inArgCount, char **inArgs ) {
    
    setMusicLoudness( 1 );
    
    if( inArgCount > 1 ) {
        startMusic( inArgs[1] );
        }
    else {
        startMusic( "music.tga" );
        }
    
    while( true ) {
        int number;
        int numRead = scanf( "%d", &number );
        
        if( numRead == 1
            && number >0 && number <=9 ) {
            
            musicTrackFadeLevel = number / 9.0;
            }
        
        //Thread::staticSleep( 1000 );
        }
    }


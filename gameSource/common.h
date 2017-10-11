#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED



#include "minorGems/graphics/Image.h"

//#include <SDL/SDL.h>

// reads a TGA file from the default ("graphics") folder
Image *readTGA( const char *inFileName );


Image *readTGA( const char *inFolderName, const char *inFileName );


// implemented in game.cpp
Color codeToColor( char inCode );

// implemented in game.cpp
void flipColorCodes( char *inColorSignature );


// maps linear values in [0,1] to smoothed [0,1] using sine
double smoothBlend( double inValue );



#endif

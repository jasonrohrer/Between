#include "common.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"

#include "minorGems/io/file/FileInputStream.h"


#include <math.h>



Image *readTGA( const char *inFileName ) {
    return readTGA( "graphics", inFileName );
    }



Image *readTGA( const char *inFolderName, const char *inFileName ) {
    File tgaFile( new Path( inFolderName ), inFileName );
    FileInputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    return converter.deformatImage( &tgaStream );
    }



double smoothBlend( double inValue ) {
    

    return 1 - ( sin( inValue * M_PI
                      +
                      M_PI / 2 ) 
                 + 1 ) / 2;
    }

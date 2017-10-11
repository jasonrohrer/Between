#ifndef HIGHLIGHT_LABEL_GL_INCLUDED
#define HIGHLIGHT_LABEL_GL_INCLUDED 

#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/Color.h"


// a label with a highlight that can be turned on and off
class HighlightLabelGL : public LabelGL {


	public:


        // same params as LabelGL
		HighlightLabelGL( double inAnchorX, double inAnchorY, double inWidth,
                          double inHeight, const char *inString, 
                          TextGL *inText )
                : LabelGL( inAnchorX, inAnchorY, inWidth,
                           inHeight, inString, inText ),
                  mHighlightOn( false ), mHighlightColor( 1.0, 0.5, 0 ) {

            }



        void setHighlight( char inHighlightOn ) {
            mHighlightOn = inHighlightOn;
            }
        


        char isHighlightOn() {
            return mHighlightOn;
            }
                

		// override fireRedraw in LabelGL
		virtual void fireRedraw();


		
	protected:
        char mHighlightOn;
        
        Color mHighlightColor;
        
	};


		
inline void HighlightLabelGL::fireRedraw() {
	Color *oldColor = NULL;
    
    if( mHighlightOn ) {
        
        oldColor = mText->getFontColor()->copy();
        
        mText->setFontColor( mHighlightColor.copy() );
        }
    
    mText->drawText( mString, mAnchorX, mAnchorY,
					 mWidth, mHeight );
	
    if( mHighlightOn ) {
        mText->setFontColor( oldColor );
        }
    
    }



#endif




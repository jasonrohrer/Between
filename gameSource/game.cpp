/*
 * Modification History
 *
 * 2008-September-11  Jason Rohrer
 * Created.  Copied from Cultivation.
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// let SDL override our main function with SDLMain
#include <SDL/SDL_main.h>


// must do this before SDL include to prevent WinMain linker errors on win32
int mainFunction( int inArgCount, char **inArgs );

int main( int inArgCount, char **inArgs ) {
    return mainFunction( inArgCount, inArgs );
    }


#include <SDL/SDL.h>



#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"
#include "minorGems/graphics/Color.h"

#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"



#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"

#include "minorGems/io/file/File.h"


#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"

#include "minorGems/util/random/StdRandomSource.h"

StdRandomSource randSource;



#include "common.h"
#include "Sprite.h"
#include "World.h"
#include "Block.h"
#include "BlockGrid.h"
#include "TargetBlock.h"
#include "BuilderTargetBlock.h"
#include "HighlightLabelGL.h"
#include "Client.h"
#include "GridTransition.h"
#include "NightBackground.h"
#include "Player.h"
#include "Star.h"
#include "Cloud.h"
#include "musicPlayer.h"




// some settings

// size of game image
int gameWidth = 320;
int gameHeight = 240;

// size of screen for fullscreen mode
int screenWidth = 640;
int screenHeight = 480;


int transitionSteps = 63;

int animationFrameSteps = 2;

int minSleepAnimationFrameSteps = 5;

int numSleepFrames = 8;

int rubbingEyesFrameSteps = 10;



double baseScale = 1.0 / 16.0;

double playerBaseHeight = 1;

double playerSpeed = 0.0625;


double leftWorldBoundary = -34;
double rightWorldBoundary = 33;


int numStars = 20;
int numClouds = 30;


int towerX = 7;



class GameSceneHandler :
    public SceneHandlerGL, public MouseHandlerGL, public KeyboardHandlerGL,
    public RedrawListenerGL { 

	public:

        /**
         * Constructs a sceen handler.
         *
         * @param inScreen the screen to interact with.
         *   Must be destroyed by caller after this class is destroyed.
         */
        GameSceneHandler( ScreenGL *inScreen );

        virtual ~GameSceneHandler();


        
        /**
         * Executes necessary init code that reads from files.
         *
         * Must be called before using a newly-constructed GameSceneHandler.
         *
         * This call assumes that the needed files are in the current working
         * directory.
         */
        void initFromFiles();

        

        ScreenGL *mScreen;


        


        
        
        
		// implements the SceneHandlerGL interface
		virtual void drawScene();

        // implements the MouseHandlerGL interface
        virtual void mouseMoved( int inX, int inY );
        virtual void mouseDragged( int inX, int inY );
        virtual void mousePressed( int inX, int inY );
        virtual void mouseReleased( int inX, int inY );

        // implements the KeyboardHandlerGL interface
        virtual char isFocused() {
            // always focused
            return true;
            }        
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );
        
        // implements the RedrawListener interface
		virtual void fireRedraw();

        
        
    protected:
        // sets the string on a label and re-centers it
        void setLabelString( LabelGL *inLabel, 
                             const char *inTranslationString,
                             double inScaleFactor = 1.0 );
        
        // creates a centerd label at a particular height
        HighlightLabelGL *createLabel( double inGuiY, 
                                       const char *inTranslationString,
                                       TextGL *inText = NULL );
        

        // the time that the last frame was drawn
        unsigned long mLastFrameSeconds;
        unsigned long mLastFrameMilliseconds;

        // our current frame rate
        unsigned long mFrameMillisecondDelta;

        int mStartTimeSeconds;
        
        char mPaused;

        double mMaxFrameRate;

        char mPrintFrameRate;
        unsigned long mNumFrames;
        unsigned long mFrameBatchSize;
        unsigned long mFrameBatchStartTimeSeconds;
        unsigned long mFrameBatchStartTimeMilliseconds;


        Color mBackgroundColor;

        
        Client *mClient;
        
        SimpleVector<Client *> mOldClients;
        
        // for status display during game
        GUIPanelGL *mStatusPanel;
        GUITranslatorGL *mStatusPanelGuiTranslator;
        HighlightLabelGL *mStatusLabel;
        

        // for game selection display
        char mGUIVisible;
        
        GUIPanelGL *mMainPanel;
        GUITranslatorGL *mMainPanelGuiTranslator;
        
        TextGL *mTextGL;
        TextGL *mTextGLFixedWidth;
        
        GUIPanelGL *mCurrentPanel;
        

        GUIPanelGL *mTitlePanel;
        HighlightLabelGL *mTitleLabel;
        
        
        GUIPanelGL *mInitialRequestPanel;
        HighlightLabelGL *mInitialRequestLabel;
        
        double mInitialRequestPanelShownAtTime;
        


        GUIPanelGL *mSelectPanel;
        HighlightLabelGL *mCreateFriendLabel;
        HighlightLabelGL *mJoinFriendLabel;
        HighlightLabelGL *mJoinStrangerLabel;
        
        // track as array to make toggling easier
        HighlightLabelGL *mSelectLabels[3];
        int mSelectHighlightIndex;
        
        
        GUIPanelGL *mWaitFriendPanel;        
        HighlightLabelGL *mWaitFriendLabel;
        HighlightLabelGL *mGameIDLabel;

        GUIPanelGL *mWaitStrangerPanel;        
        HighlightLabelGL *mWaitStrangerLabel;

        GUIPanelGL *mJoinFriendPanel;
        HighlightLabelGL *mEnterGameIDLabel;
        TextFieldGL *mEnterGameIDField;
        
        


        static const int mNumWorlds = 3;
        
        // extra world is used by transition grid
        World mWorlds[ mNumWorlds + 1];
        BlockGrid mBlockGrids[ mNumWorlds + 1 ];
        BlockGrid mTargetGrids[ mNumWorlds + 1 ];

        // track all target blocks separate from the grid they are in
        SimpleVector<TargetBlock *> mTargetBlocks[ mNumWorlds + 1 ];
        SimpleVector<BuilderTargetBlock *> mBuilderTargets[ mNumWorlds + 1 ];
        
        // track all stars and clouds separate from the world they are in
        SimpleVector<Star *> mStars[ mNumWorlds + 1 ];
        SimpleVector<Cloud *> mClouds[ mNumWorlds + 1 ];
        

        // track all tiles in one big blob
        SimpleVector<GameObject *> mTiles;
        
        // track all alarms stands separate from their world
        SimpleVector<GameObject *> mAlarmStands;
        

        int mCurrentWorld;
        // -1 if not waiting for transition between worlds
        int mNextWorld;
        int mWorldTransitionDirection;
        

        GridTransition *mTransitionGrid;
        World *mTransitionWorld;
        
        // adjust toward this gradually (based on completeness of tower
        //   in each world)
        double mMusicTrackFadeLevelTarget[ mNumWorlds + 1 ];

        
        
        Player *mPlayer;
        
        char mLeftKeyDown;
        char mRightKeyDown;
        
        
        double mPlayerXDelta;
        double mPlayerFadeDelta;
        double mPlayerFadeUpperLimit;
        double mPlayerFadeLowerLimit;
        

        double mPlayerYSineParam;
        double mPlayerYSineDelta;
        

        int mStepsSinceAnimationFrame;
        
        char mCountingServerSteps;
        int mLastServerLatencyNumSteps;
        int mStepsPerSleepFrame;
        
        char mGoingToSleep;
        char mWakingUp;
        char mRubbingEyes;
        int mRubbingDirection;
        
        // count steps to make it consistent every time
        // matching last server latency
        // resets back to 0 every time we start going to sleep
        int mGoingToSleepSteps;
        
        int mTargetGoingToSleepSteps;
        
        


        // where player was last in each world
        // not that last one (corresponding to transition world) is never used
        // but array size is same so that same loops can be used.
        double mLastPlayerX[ mNumWorlds + 1];
        


        GameObject *mSelector;
        char mSelectorInBounds;
        

        Block *mHoldingBlock;
        

        Sprite mSelectorSprite;
        Sprite mBuilderSprite;
        Sprite mBuilderTargetSprite;
        Sprite mBuilderHintSprite;
        
        // one shared in all worlds
        GameObject *mBuilderHint;
        


        Sprite mTileSprite;


        Sprite mNightTileSprite;
        Sprite mNightTileVerticalSprite;
        Sprite mNightTileCornerSprite;
        Sprite mBedSprite;
        Sprite mNightWindowSprite;
        Sprite mStarSprite;


        Sprite mMorningTileSprite;
        Sprite mMorningTileVerticalSprite;
        Sprite mMorningTileCornerSprite;
        Sprite mAlarmStandSprite;
        Sprite mMorningWindowSprite;
        
        
        Sprite mBackHintSprite;
        Sprite mNextHintSprite;
        Sprite mInsertHintSprite;
        
        char mDrawUpHint;
        char mDrawDownHint;
        double mUpHintFade;
        double mDownHintFade;
        Sprite mUpHintSprite;
        Sprite mDownHintSprite;
        

        Sprite mCloudSprite;
        
        

        // inserts a block into the current grid at current selector position
        void insertBlock( Block *inBlock );


        // bounds the selector's Y position based on current block column
        void boundSelectorY();
        
        
        
	};



GameSceneHandler *sceneHandler;
ScreenGL *screen;

// how many pixels wide is each game pixel drawn as?
int pixelZoomFactor;



Color ourColors[3] = { Color( 1.0, 0, 0 ),
                       Color( 0, 1.0, 0 ),
                       Color( 0, 0, 1.0 ) };
char ourColorCodes[3] = { 'A', 'B', 'C' };



// GameObject because this can hold a BlendedBlock at index 3 during transition
GameObject *ourAvailableBlocks[4];
int ourCurrentBlockIndex;


// used on palate during transition
Block *oldConstructedBlock = NULL;
Block *newConstructedBlock = NULL;
BlendedBlock *transitionConstructedBlock = NULL;


// fade out hints after they've each been used enough
//... what's enough?  10 times each
int numTimesHintsUsedBeforeFade = 10;

// hint types:
// (don't track W and S keys, because those hints can't be turned off anyway)
enum HintType { 
    HINT_SPACE = 0,
    HINT_N,
    HINT_B,
    HINT_UP,
    HINT_DOWN,
    NUM_HINT_TYPES
    };

int hintUsedCount[ NUM_HINT_TYPES ];

char fadeOutHints = false;
double hintFadeFactor = 1.0;



// for music
// 0 means no tracks playing
// 1 means all tracks playing
// 0.5 means first half of tracks playing, rest silent
double musicTrackFadeLevel = 0;

double loudnessAdjust = 1.0;
double loudnessAdjustTarget = 1.0;





Color theirColors[3] = { Color( 1.0, 1.0, 0 ),
                         Color( 0, 1.0, 1.0 ),
                         Color( 1.0, 0, 1.0 ) };
char theirColorCodes[3] = { 'D', 'E', 'F' };


// from common.h
Color codeToColor( char inCode ) {
    switch( inCode ) {
        case 'A':
            return ourColors[0];
        case 'B':
            return ourColors[1];
        case 'C':
            return ourColors[2];
        case 'D':
            return theirColors[0];
        case 'E':
            return theirColors[1];
        case 'F':
            return theirColors[2];
        }
    
    // default
    return Color();
    }



void replaceChar( char *inString, char inTarget, char inReplacement ) {

    int length = strlen( inString );
    
    for( int i=0; i<length; i++ ) {
        if( inString[i] == inTarget ) {
            inString[i] = inReplacement;
            }
        }
    }



// from common.h
void flipColorCodes( char *inColorSignature ) {
    for( int c=0; c<3; c++ ) {
        replaceChar( inColorSignature, ourColorCodes[c], 'X' );
        replaceChar( inColorSignature, theirColorCodes[c], ourColorCodes[c] );
        replaceChar( inColorSignature, 'X', theirColorCodes[c] );
        }
    }



int gameIDLength = 7;

const char *placeholderGameIDString = ".......";
                       


char showingQuitQuestion = false;
// track text that was there before we showed the question
char *oldStatusText = NULL;



// function that destroys object when exit is called.
// exit is the only way to stop the loop in  ScreenGL
void cleanUpAtExit() {
    printf( "exiting\n" );
    
    stopMusic();

    delete sceneHandler;
    delete screen;

    for( int b=0; b<4; b++ ) {
        if( ourAvailableBlocks[b] != NULL ) {
            delete ourAvailableBlocks[b];
            }
        }
    if( newConstructedBlock != NULL ) {
        delete newConstructedBlock;
        }
    if( oldConstructedBlock != NULL ) {
        delete oldConstructedBlock;
        }
    // transitionConstructedBlock deleted in loop above if it exists


    SDL_Quit();
    }




int mainFunction( int inNumArgs, char **inArgs ) {

    for( int h=0; h<NUM_HINT_TYPES; h++ ) {
        hintUsedCount[h] = 0;
        }

        
    // must pass args to GLUT before constructing the screen
    // glutInit( &inNumArgs, inArgs );


    
    


    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
        printf( "Couldn't initialize SDL: %s\n", SDL_GetError() );
        return 0;
        }


    // do this mac check after initing SDL,
    // since it causes various Mac frameworks to be loaded (which can
    // change the current working directory out from under us)
    #ifdef __mac__
        // make sure working directory is the same as the directory
        // that the app resides in
        // this is especially important on the mac platform, which
        // doesn't set a proper working directory for double-clicked
        // app bundles

        // arg 0 is the path to the app executable
        char *appDirectoryPath = stringDuplicate( inArgs[0] );
    
        char *appNamePointer = strstr( appDirectoryPath,
                                       "Between.app" );

        if( appNamePointer != NULL ) {
            // terminate full app path to get parent directory
            appNamePointer[0] = '\0';
            
            chdir( appDirectoryPath );
            }
        
        delete [] appDirectoryPath;
    #endif



    // read screen size from settings
    char widthFound = false;
    int readWidth = SettingsManager::getIntSetting( "screenWidth", 
                                                    &widthFound );
    char heightFound = false;
    int readHeight = SettingsManager::getIntSetting( "screenHeight", 
                                                    &heightFound );
    
    if( widthFound && heightFound ) {
        // override hard-coded defaults
        screenWidth = readWidth;
        screenHeight = readHeight;
        }
    
    printf( "Screen dimensions for fullscreen mode:  %dx%d\n",
            screenWidth, screenHeight );


    char fullscreenFound = false;
    int readFullscreen = SettingsManager::getIntSetting( "fullscreen", 
                                                         &fullscreenFound );
    
    char fullscreen = true;
    
    if( readFullscreen == 0 ) {
        fullscreen = false;
        }
    

    screen =
        new ScreenGL( screenWidth, screenHeight, fullscreen, 
                      "Between", NULL, NULL, NULL );

    pixelZoomFactor = screenWidth / gameWidth;

    if( pixelZoomFactor * gameHeight > screenHeight ) {
        // screen too short
        pixelZoomFactor = screenHeight / gameHeight;
        }
    

    // hack:  new version of minorGems scales screen differently
    screen->setImageSize( pixelZoomFactor * gameWidth,
                          //pixelZoomFactor * gameHeight );
                          pixelZoomFactor * gameWidth );
    

    SDL_ShowCursor( SDL_DISABLE );


    sceneHandler = new GameSceneHandler( screen );
    



    for( int b=0; b<3; b++ ) {
        ourAvailableBlocks[b] = new Block( &( ourColors[b] ), 
                                           ourColorCodes[b] );
        }
    ourAvailableBlocks[3] = NULL;
    ourCurrentBlockIndex = 0;
    

    // also do file-dependent part of init for GameSceneHandler here
    // actually, constructor is file dependent anyway.
    sceneHandler->initFromFiles();
    
        
    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );


    setMusicLoudness( 1 * loudnessAdjust );
    startMusic( "music.tga" );

    
    screen->switchTo2DMode();
    
    screen->start();

    
    return 0;
    }




GameSceneHandler::GameSceneHandler( ScreenGL *inScreen )
    : mScreen( inScreen ),
      mFrameMillisecondDelta( 0 ),
      mStartTimeSeconds( time( NULL ) ),
      mPaused( false ),
      mMaxFrameRate( 60 ),  
      mPrintFrameRate( false ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( time( NULL ) ),
      mFrameBatchStartTimeMilliseconds( 0 ),
      //mBackgroundColor( 0.75, 0.75, 0.75, 1 ),
      mBackgroundColor( 0, 0, 0, 1 ),
      mClient( new Client ),
      mSelectorSprite( "selector.tga", true, 4 ),
      mBuilderSprite( "blockBuilder.tga", true ),
      mBuilderTargetSprite( "blockBuilderTarget.tga", true ),
      mBuilderHintSprite( "builderHint.tga", true ),
      mTileSprite( "tile.tga", false, 1, 4 ),
      mNightTileSprite( "nightTile.tga" ),
      mNightTileVerticalSprite( "nightTileVertical.tga" ),
      mNightTileCornerSprite( "nightTileCorner.tga" ),
      mBedSprite( "bed.tga", true ),
      mNightWindowSprite( "nightWindow.tga", true ),
      mStarSprite( "star.tga", true ),
      mMorningTileSprite( "morningTile.tga" ),
      mMorningTileVerticalSprite( "morningTileVertical.tga" ),
      mMorningTileCornerSprite( "morningTileCorner.tga" ),
      mAlarmStandSprite( "alarmStand.tga", true, 1, 4 ),
      mMorningWindowSprite( "morningWindow.tga", true ),
      mBackHintSprite( "backHint.tga", true ),
      mNextHintSprite( "nextHint.tga", true ),
      mInsertHintSprite( "insertHint.tga", true ),
      mDrawUpHint( false ),
      mDrawDownHint( false ),
      mUpHintFade( 0 ),
      mDownHintFade( 0 ),
      mUpHintSprite( "upHint.tga", true ),
      mDownHintSprite( "downHint.tga", true ),
      mCloudSprite( "clouds.tga", false, 4 ) { 
    


    glClearColor( mBackgroundColor.r,
                  mBackgroundColor.g,
                  mBackgroundColor.b,
                  mBackgroundColor.a );
    

    // set external pointer so it can be used in calls below
    sceneHandler = this;

    
    mScreen->addMouseHandler( this );
    mScreen->addKeyboardHandler( this );
    mScreen->addSceneHandler( this );
    mScreen->addRedrawListener( this );

    
    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );
    

    mPlayer = new Player();

    mLeftKeyDown = false;
    mRightKeyDown = false;
    
    mPlayerXDelta = 0;
    mPlayerFadeDelta = 0;
    mPlayerFadeUpperLimit = 1.0;
    mPlayerFadeLowerLimit = 0;
    

    mPlayerYSineParam = 0;
    mPlayerYSineDelta = 0;
    

    mStepsSinceAnimationFrame = 0;
    
   
    mCountingServerSteps = false;
    mLastServerLatencyNumSteps = minSleepAnimationFrameSteps * numSleepFrames;
    // default until we can measure server response 
    mStepsPerSleepFrame = minSleepAnimationFrameSteps;
    mGoingToSleep = false;
    mWakingUp = false;
    
    mRubbingEyes = false;
    mRubbingDirection = 1;
    
    mGoingToSleepSteps = 0;
    mTargetGoingToSleepSteps = mLastServerLatencyNumSteps;
    
    
    // we step it manually, since one object is shared by all worlds
    mPlayer->mEnableSteps = false;
    
    mPlayer->mScale = baseScale;
    mPlayer->mY = playerBaseHeight;
    mPlayer->mDepthLayer = 0;
    
    mSelector = new GameObject();
    mSelector->mSprite = &mSelectorSprite;
    mSelector->mScale = baseScale;
    mSelector->mY = mPlayer->mY;
    mSelector->mDepthLayer = 1;
    
    mSelectorInBounds = true;
    

    mHoldingBlock = NULL;
    

    mCurrentWorld = 0;
    mNextWorld = -1;
    
    mWorldTransitionDirection = 0;
    

    mTransitionGrid = NULL;
    mTransitionWorld = &( mWorlds[ mNumWorlds ] );
    

    // four base blocks copied into each world's target grid
    BuilderTargetBlock builderTargets[4] =
        { BuilderTargetBlock( &mBuilderTargetSprite ),
          BuilderTargetBlock( &mBuilderTargetSprite ),
          BuilderTargetBlock( &mBuilderTargetSprite ),
          BuilderTargetBlock( &mBuilderTargetSprite ) 
            };
    //    int builderTargetsX[4] = { -1, 0, 0, -1 };
    //int builderTargetsY[4] = { 0, 0, 1, 1 };

    int builderTargetsX[4] = { -1, 0, 0, -1 };
    int builderTargetsY[4] = { 1, 1, 2, 2 };
    

    // shared by all worlds
    mBuilderHint = new GameObject();
    mBuilderHint->mSprite = &mBuilderHintSprite;
    mBuilderHint->mScale = baseScale;
    mBuilderHint->mY = 0;
    mBuilderHint->mX = -0.5 - baseScale;
    mBuilderHint->mDepthLayer = 0;
    // start invisible
    mBuilderHint->mFadeFactor = 0;
    

    for( int w=0; w<mNumWorlds + 1; w++ ) {
        // player starts in center of each world
        mLastPlayerX[ w ] = 0;
        

        mBlockGrids[w].setWorld( &( mWorlds[w] ) );
        mTargetGrids[w].setWorld( &( mWorlds[w] ) );

        mBlockGrids[w].setDepthLayer( 7 );
        mTargetGrids[w].setDepthLayer( 3 );
        

        // add same player object to all worlds
        mWorlds[w].add( mPlayer );
        mWorlds[w].add( mSelector );
        mWorlds[w].add( mBuilderHint );
        

    
        GameObject *builder = new GameObject();
        builder->mSprite = &mBuilderSprite;
        builder->mScale = baseScale;
        builder->mDepthLayer = 3;
        builder->mX = -0.5;
        builder->mY = 1.5;
    
        mWorlds[w].add( builder );

        // add builder targets

        for( int b=0; b<4; b++ ) {
            
            BuilderTargetBlock *blockCopy = builderTargets[b].copy();
                                
            mTargetGrids[w].add( blockCopy,
                                 builderTargetsX[b],
                                 builderTargetsY[b] );
            
            mBuilderTargets[w].push_back( blockCopy );
            }
        

        // add tiles
        for( int x=-20; x<=19; x++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
        
    
            mWorlds[w].add( tile );

            mTiles.push_back( tile );
            }


        // add transition tiles to night
        
        
        // add a few extra world tiles underneath (to be covered by 
        //  night tiles that fade in)
        for( int x=-21; x>-27; x-- ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
    
            mWorlds[w].add( tile );
            mTiles.push_back( tile );
            }

        // add fading-in night tiles on top, for a bit
        for( int x=-21; x>-27; x-- ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mNightTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
            tile->mFadeFactor = 1 - (double)(x - (-26)) / 6;
            
    
            mWorlds[w].add( tile );
            }


        
        // add solid night tiles all the rest of the way out
        for( int x=-27; x>leftWorldBoundary; x-- ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mNightTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
                
            mWorlds[w].add( tile );
            }
        
        
        
        // add vertical night tiles as left boundary
        for( int y=1; y<10; y++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mNightTileVerticalSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = leftWorldBoundary;
            tile->mY = y;
                            
            mWorlds[w].add( tile );
            }
        
        // neaten up corner
        GameObject *cornerNightTile = new GameObject();
        cornerNightTile->mSprite = &mNightTileCornerSprite;
        cornerNightTile->mScale = 1.0 / 16.0;
        cornerNightTile->mDepthLayer = 2;
        cornerNightTile->mX = leftWorldBoundary;
        cornerNightTile->mY = 0;
        
        mWorlds[w].add( cornerNightTile );


        NightBackground *night = new NightBackground( Color( 0, 0, 0 ),
                                                      -1 );
        night->mScale = baseScale;
        night->mDepthLayer = 3;
        night->mX = -21;
        night->mY = 4.5;
        mWorlds[w].add( night );

        
        GameObject *bed = new GameObject();
        bed->mSprite = &mBedSprite;
        bed->mScale = baseScale;
        bed->mDepthLayer = 3;
        bed->mX = leftWorldBoundary + 5;
        bed->mY = 1.5;
        mWorlds[w].add( bed );

        GameObject *nightWindow = new GameObject();
        nightWindow->mSprite = &mNightWindowSprite;
        nightWindow->mScale = baseScale;
        nightWindow->mDepthLayer = 3;
        nightWindow->mX = leftWorldBoundary + 2;
        nightWindow->mY = 2.5;
        mWorlds[w].add( nightWindow );

        // add some stars

        for( int s=0; s<numStars; s++ ) {
            Star *star = new Star();
            
            star->mSprite = &mStarSprite;
            star->mScale = baseScale;
            star->mDepthLayer = 3;
            star->mX = 
                randSource.getRandomBoundedDouble( leftWorldBoundary + 0.5, 
                                                   -23.5 );
            star->mY = 
                randSource.getRandomBoundedDouble( 4, 9.5 );

            double yFadeFactor = 1.0;
            
            if( star->mY < 6 ) {
                yFadeFactor = ( (star->mY - 4) / ( 6 - 4 ) );
                }
            
            double xFadeFactor = 1.0;
            
            if( star->mX > -26.5 ) {
                xFadeFactor = ( (star->mX - (-23.5)) / ( (-26.5) - (-23.5) ) );
                }
            
                    
            star->mBaseFade =
                yFadeFactor * xFadeFactor;
            
            
            mWorlds[w].add( star );

            mStars[w].push_back( star );
            }





        
        // now add morning stuff on right end


        // add transition tiles to morning
        
        
        // add a few extra world tiles underneath (to be covered by 
        //  morning tiles that fade in)
        for( int x=20; x<26; x++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
            
    
            mWorlds[w].add( tile );
            mTiles.push_back( tile );
            }

        // add fading-in morning tiles on top, for a bit
        for( int x=20; x<26; x++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mMorningTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
            tile->mFadeFactor = (double)(x - 19) / 6;
            
    
            mWorlds[w].add( tile );
            }


        
        // add solid morning tiles all the rest of the way out
        for( int x=26; x<rightWorldBoundary; x++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mMorningTileSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = x;
                
            mWorlds[w].add( tile );
            }
        
        
        
        // add vertical morning tiles as right boundary
        for( int y=1; y<10; y++ ) {
            GameObject *tile = new GameObject();
            tile->mSprite = &mMorningTileVerticalSprite;
            tile->mScale = 1.0 / 16.0;
            tile->mDepthLayer = 2;
            tile->mX = rightWorldBoundary;
            tile->mY = y;
                            
            mWorlds[w].add( tile );
            }
        
        // neaten up corner
        GameObject *cornerMorningTile = new GameObject();
        cornerMorningTile->mSprite = &mMorningTileCornerSprite;
        cornerMorningTile->mScale = 1.0 / 16.0;
        cornerMorningTile->mDepthLayer = 2;
        cornerMorningTile->mX = rightWorldBoundary;
        cornerMorningTile->mY = 0;
        
        mWorlds[w].add( cornerMorningTile );




        // reuse night background for morning too
        NightBackground *morning = new NightBackground( Color( 1, 1, 0 ),
                                                      1 );
        morning->mScale = baseScale;
        morning->mDepthLayer = 3;
        morning->mX = 20;
        morning->mY = 4.5;
        mWorlds[w].add( morning );


        GameObject *alarmStand = new GameObject();
        alarmStand->mSprite = &mAlarmStandSprite;
        alarmStand->mScale = baseScale;
        alarmStand->mDepthLayer = 3;
        alarmStand->mX = rightWorldBoundary - 5;
        alarmStand->mY = 1.5;
        mWorlds[w].add( alarmStand );
        
        mAlarmStands.push_back( alarmStand );
                

        GameObject *morningWindow = new GameObject();
        morningWindow->mSprite = &mMorningWindowSprite;
        morningWindow->mScale = baseScale;
        morningWindow->mDepthLayer = 3;
        morningWindow->mX = rightWorldBoundary - 2;
        morningWindow->mY = 2.5;
        mWorlds[w].add( morningWindow );





        // finally, clouds behind everything
        
        for( int c=0; c<numClouds; c++ ) {
            Cloud *cloud = new Cloud();
            
            cloud->mSprite = &mCloudSprite;
            cloud->mScale = baseScale * 4;
            
            // fraction of clouds are in front of blocks and semi-transparent
            if( c < numClouds / 2 ) {
                cloud->mDepthLayer = 6;
                cloud->mBaseFade = 0.5;
                }
            else {
                // rest are behind and fully opaque
                cloud->mDepthLayer = 9;
                cloud->mBaseFade = 1.0;
                }
            
            cloud->mSpriteFrame = 
                randSource.getRandomBoundedInt( 
                    0,
                    mCloudSprite.getNumFrames() - 1 );
            

            cloud->mX = 
                randSource.getRandomBoundedDouble( -27, 
                                                   26 );
            cloud->mY = 
                randSource.getRandomBoundedDouble( 7, 10 );

            double yFadeFactor = 1.0;
            
            if( cloud->mY < 9 ) {
                yFadeFactor = ( (cloud->mY - 7) / ( 9 - 7 ) );
                }            

            cloud->mBaseFade *= yFadeFactor;
            
            
            mWorlds[w].add( cloud );

            mClouds[w].push_back( cloud );
            }


        mMusicTrackFadeLevelTarget[w] = 0;
        }
    
    
    
    Block::staticInit();

    // set up status UI
    mStatusPanel = new GUIPanelGL( 0, 0.75, 2, 0.25,
                                   new Color( 0,
                                              0,
                                              0,
                                              // transparent background
                                              0 ) );

	mStatusPanelGuiTranslator = new GUITranslatorGL( mStatusPanel, mScreen );

    // no keyboard handler... status panel not interactive
	mScreen->addSceneHandler( mStatusPanelGuiTranslator );

    


    // set up main UI
    mMainPanel = new GUIPanelGL( 0, 0, 2, 2,
                                 new Color( 0,
                                            0,
                                            0, 1.0 ) );

	mMainPanelGuiTranslator = new GUITranslatorGL( mMainPanel, mScreen );

	mScreen->addSceneHandler( mMainPanelGuiTranslator );
    mScreen->addKeyboardHandler( mMainPanelGuiTranslator );
    
    mGUIVisible = true;
    

    // construct sub-panels, but only add the first one
    mTitlePanel = new GUIPanelGL( 0, 0, 2, 2,
                                  new Color( 0,
                                             0,
                                             0, 1.0 ) );
    mInitialRequestPanel = new GUIPanelGL( 0, 0, 2, 2,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );
    mSelectPanel = new GUIPanelGL( 0, 0, 2, 2,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );
    mWaitFriendPanel = new GUIPanelGL( 0, 0, 2, 2,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );
    mWaitStrangerPanel = new GUIPanelGL( 0, 0, 2, 2,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );

    mJoinFriendPanel = new GUIPanelGL( 0, 0, 2, 2,
                                       new Color( 0,
                                                  0,
                                                  0, 1.0 ) );
    
    mMainPanel->add( mTitlePanel );
    mCurrentPanel = mTitlePanel;
    }



GameSceneHandler::~GameSceneHandler() {
    mScreen->removeMouseHandler( this );
    mScreen->removeSceneHandler( this );
    mScreen->removeRedrawListener( this );

    for( int w=0; w<mNumWorlds + 1; w++ ) {
        
        mWorlds[w].remove( mPlayer );
        mWorlds[w].remove( mSelector );
        mWorlds[w].remove( mBuilderHint );
        }
    delete mPlayer;
    delete mSelector;
    delete mBuilderHint;
    
    
    
    Block::staticFree();
    Client::staticFree();
    
    // remove before deleting, since we're not sure which one is still
    // added
    mMainPanel->remove( mTitlePanel );
    mMainPanel->remove( mInitialRequestPanel );
    mMainPanel->remove( mSelectPanel );
    mMainPanel->remove( mWaitStrangerPanel );
    mMainPanel->remove( mWaitFriendPanel );
    mMainPanel->remove( mJoinFriendPanel );
    
    delete mTitlePanel;
    delete mInitialRequestPanel;
    delete mSelectPanel;
    delete mWaitStrangerPanel;
    delete mWaitFriendPanel;
    delete mJoinFriendPanel;
    
    // this will recursively delete all status GUI components
    delete mStatusPanelGuiTranslator;
    

    // this will recursively delete all of our selector GUI components
	delete mMainPanelGuiTranslator;

    delete mTextGL;
    delete mTextGLFixedWidth;

    delete mClient;

    for( int i=0; i<mOldClients.size(); i++ ) {
        delete *( mOldClients.getElement( i ) );
        }
    }



void GameSceneHandler::setLabelString( LabelGL *inLabel, 
                                       const char *inTranslationString,
                                       double inScaleFactor ) {
    char *labelString =
        (char *)TranslationManager::translate( (char*)inTranslationString );
    
    inLabel->setText( labelString );

    TextGL *text = inLabel->getTextGL();
    
    int screenImageHeight = mScreen->getImageHeight();
    

    // 1:1 aspect ratio
    // we know font is 6 pixels wide/tall
    double height = inScaleFactor * 
        pixelZoomFactor * 6 * 1.0 / screenImageHeight;
    double width = height * strlen( labelString );
    
    double actualDrawWidth =
        height * text->measureTextWidth( labelString );
          
    // round to an integer number of pixels
    actualDrawWidth = 
        round( actualDrawWidth * screenImageHeight )
        /
        screenImageHeight;
    
      
    double centerW = 
        0.5 * (double)( mScreen->getImageWidth() ) / screenImageHeight;
    
    double guiY = inLabel->getAnchorY();
    

    double labelX = centerW - 0.5 * actualDrawWidth;
    

    // round to an integer number of pixels
    labelX =
        round( labelX * screenImageHeight )
        /
        screenImageHeight;
    
    guiY =
        round( guiY * screenImageHeight )
        /
        screenImageHeight;

    
    
    inLabel->setPosition( labelX,
                          guiY,
                          width,
                          height );
    }



HighlightLabelGL *GameSceneHandler::createLabel( 
    double inGuiY, 
    const char *inTranslationString,
    TextGL *inText ) {
    

    if( inText == NULL ) {
        inText = mTextGL;
        }

    HighlightLabelGL *returnLabel =
        new HighlightLabelGL( 0, inGuiY, 0, 0, "", inText );
    
    setLabelString( returnLabel, inTranslationString );

    return returnLabel;
    }






void GameSceneHandler::initFromFiles() {
    
    // translation language
    File languageNameFile( NULL, "language.txt" );

    if( languageNameFile.exists() ) {
        char *languageNameText = languageNameFile.readFileContents();

        SimpleVector<char *> *tokens = tokenizeString( languageNameText );

        int numTokens = tokens->size();
        
        // first token is name
        if( numTokens > 0 ) {
            char *languageName = *( tokens->getElement( 0 ) );
        
            TranslationManager::setLanguage( languageName );
            }
        else {
            // default

            // TranslationManager already defaults to English, but
            // it looks for the language files at runtime before we have set
            // the current working directory.
            
            // Thus, we specify the default again here so that it looks
            // for its language files again.
            TranslationManager::setLanguage( "English" );
            }
        
        delete [] languageNameText;

        for( int t=0; t<numTokens; t++ ) {
            delete [] *( tokens->getElement( t ) );
            }
        delete tokens;
        }
    


    // load text font
    Image *fontImage = readTGA( "font.tga" );

    if( fontImage == NULL ) {
        // default
        // blank font
        fontImage = new Image( 256, 256, 4, true );
        }
    
    mTextGL = new TextGL( fontImage,
                          // use alpha
                          true,
                          // variable character width
                          false,
                          // extra space around each character
                          0.16666666666,
                          // space is 5 pixels wide (out of 6)
                          0.83333333333 );
    mTextGLFixedWidth = new TextGL( fontImage,
                                    // use alpha
                                    true,
                                    // fixed character width
                                    true,
                                    // extra space around each character
                                    0,
                                    // space is full char width
                                    1.0 );
    delete fontImage;




    // now build gui panels based on labels, which depend on textGL

    // off screen!
    //mStatusLabel = createLabel( 0.95, "BROKEN" );
    mStatusLabel = createLabel( 0.8, "" );
    
    mStatusPanel->add( mStatusLabel );
    

    mTitleLabel = createLabel( 0.50, "title" );

    // increase scale
    setLabelString( mTitleLabel, "title", 2.0 );

    mTitlePanel->add( mTitleLabel );



    mInitialRequestLabel = createLabel( 0.65, "initialRequest" );

    mInitialRequestPanel->add( mInitialRequestLabel );
    


    mSelectLabels[0] = 
        mCreateFriendLabel = createLabel( 0.6, "createForFriend" );

    mSelectLabels[1] = 
        mJoinFriendLabel = createLabel( 0.5, "joinWithFriend" );

    mSelectLabels[2] = 
        mJoinStrangerLabel = createLabel( 0.4, "joinWithStranger" );
    
    mSelectPanel->add( mCreateFriendLabel );
    mSelectPanel->add( mJoinFriendLabel );
    mSelectPanel->add( mJoinStrangerLabel );
    
    mSelectLabels[0]->setHighlight( true );
    mSelectHighlightIndex = 0;



    mWaitFriendLabel = createLabel( 0.55, "creatingGame" );
    mGameIDLabel = createLabel( 0.45, placeholderGameIDString, 
                                mTextGLFixedWidth );

    mWaitFriendPanel->add( mWaitFriendLabel );
    mWaitFriendPanel->add( mGameIDLabel );
    


    mWaitStrangerLabel = createLabel( 0.55, "waitingForStranger" );
    mWaitStrangerPanel->add( mWaitStrangerLabel );
    


    mEnterGameIDLabel = createLabel( 0.55, "enterCode" );
    mJoinFriendPanel->add( mEnterGameIDLabel );
    


    // 1:1 aspect ratio
    // we know font is 6 pixels wide/tall
        
    double height = pixelZoomFactor * 6 * 1.0 / mScreen->getImageHeight();
    double width = height * gameIDLength;
                
    double centerW = 
        0.5 * (double)( mScreen->getImageWidth() ) / mScreen->getImageHeight();
    

    mEnterGameIDField = new TextFieldGL( centerW - 0.5 * width,
                                         0.45,
                                         width,
                                         height,
                                         // 1 pixel wide border
                                         pixelZoomFactor * 1.0 / 
                                            mScreen->getImageHeight(),
                                         "",
                                         mTextGLFixedWidth,
                                         new Color( 0.75, 0.75, 0.75 ),
                                         new Color( 0.75, 0.75, 0.75 ),
                                         new Color( 0.15, 0.15, 0.15 ),
                                         new Color( 0.75, 0.75, 0.75 ),
                                         gameIDLength,
                                         true );
    
    mJoinFriendPanel->add( mEnterGameIDField );
        




    // load target image and split it into blocks
    
    Image *targetImage = readTGA( "target.tga" );
    
    int w = targetImage->getWidth();
    int h = targetImage->getHeight();
    
    int blocksWide = w / 16;
    // one extra row of pixels at bottom defines color map
    int blocksHigh = h / 16;
    
    
    // build color map, mapping A to B, and also to codes
    Color mapA[6];
    Color mapB[6] = 
        { ourColors[0],
          ourColors[1],
          ourColors[2],
          theirColors[0],
          theirColors[1],
          theirColors[2] };
    char mapCodes[6] = 
        { ourColorCodes[0],
          ourColorCodes[1],
          ourColorCodes[2],
          theirColorCodes[0],
          theirColorCodes[1],
          theirColorCodes[2] };
    
    // read A key from bottom line of image
    int lineY = h - 1;
    int startIndex = lineY * w;
    
    for( int i=0; i<6; i++ ) {
        int index = startIndex + i;
        
        for( int c=0; c<3; c++ ) {
            ( mapA[i] )[c] = ( targetImage->getChannel( c ) )[ index ];
            }
        /*
        printf( "Color map a = " );
        mapA[i].print();
        printf( "\n" );
        */
        }
    
    // now read blocks
    
    // starting position in grid (where to place by=0, bx=0
    int gridY = 9;
    // center at towerX
    int gridX = towerX - 1;
    
    // add in reverse order, bottom up
    for( int by=blocksHigh-1; by>=0; by-- ) {
        for( int bx=0; bx<blocksWide; bx++ ) {
            
            Image *blockImage = targetImage->getSubImage( bx * 16, by * 16,
                                                          16, 16 );
            // transform the block's color
            // and map to codes
            char blockColorCodes[257];
            
            for( int i=0; i<256; i++ ) {
                
                int mapIndex = 0;
                char foundMatch = false;
                
                Color c = blockImage->getColor( i );
                
                while( mapIndex < 6 && !foundMatch ) {
                    if( mapA[ mapIndex ].equals( &c ) ) {
                        foundMatch = true;
                        }
                    else {
                        mapIndex++;
                        }
                    }
                if( foundMatch ) {
                    blockColorCodes[i] = mapCodes[ mapIndex ];
                    blockImage->setColor( i, mapB[ mapIndex ] );
                    }
                }
            blockColorCodes[256] = '\0';
            
            char *colorSig = Block::runLengthEncode( blockColorCodes );
            
            TargetBlock *block = new TargetBlock( blockImage, colorSig );
            
            
            // dummy block for testing
            // Block *dummy = new Block( blockImage, colorSig );
            
            delete blockImage;
            delete [] colorSig;
            
            // add to target in each world
            for( int w=0; w<mNumWorlds + 1; w++ ) {
                TargetBlock *blockCopy = block->copy();
                                
                mTargetGrids[w].add( blockCopy,
                                     gridX + bx,
                                     gridY - by );
            
                /*
                if( w < mNumWorlds ) {
                    
                    Block *dummyCopy = dummy->copy();
                    mBlockGrids[w].add( dummyCopy,
                                        gridX + bx - 5,
                                        gridY - by );
                    }
                */

                mTargetBlocks[w].push_back( blockCopy );
                }
            delete block;
            //delete dummy;
            
            }
        }
    delete targetImage;
    
    }


double rot = 0;
double rotDelta = 0.01;


void GameSceneHandler::drawScene() {
    /*
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
    */
	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );

    
    // sky
    glColor4d( 0.75, 0.75, 0.75, 1.0 );

    glBegin( GL_QUADS ); {
        glVertex2d( -1, 1 );
        glVertex2d( 1, 1 );
        glVertex2d( 1, -1 );
        glVertex2d( -1, -1 );
        }
    glEnd();

    
    
    if( mTransitionGrid == NULL ) {
        // scoot over one pixel
        mWorlds[ mCurrentWorld ].draw( mPlayer->mX - baseScale );
        }
    else {
        // scoot over one pixel
        mTransitionWorld->draw( mPlayer->mX - baseScale );
        }
    
    
    /*
    // draw sprites
    Vector3D posA( 0, 0, 0 );
    //mTestSprite.draw( rot, &pos, 0.5 * sin( rot / 10 ) + 0.5 );
    //rot += rotDelta;
    

    //mTestSpriteA.draw( rot, &posA, 0.03 );

    
    Vector3D posB( 0.5, 0, 0 );
    mTestSpriteA.draw( rot, &posB, 0.03 );
    mTestSpriteB.draw( rot, &posB, 0.03 );
    */
    
    

    
    // letterbox
    glColor4d( 0, 0, 0, 1.0 );

    glBegin( GL_QUADS ); {
        
        glVertex2d( -1, 1 );
        glVertex2d( 1, 1 );
        glVertex2d( 1, 0.5 );
        glVertex2d( -1, 0.5 );

        glVertex2d( -1, -1 );
        glVertex2d( 1, -1 );
        glVertex2d( 1, -0.5 );
        glVertex2d( -1, -0.5 );        
        }
    glEnd();


    
    // draw block palate on top of letterbox

    double palateScale = baseScale / 10.0;
    
    
    // keep palate centered, even during transition
    double xOffset = 0;
    
    double xOffsetA = 1 / 10.0;
    double xOffsetB = 0;
    

    if( transitionConstructedBlock == NULL ) {
        if( ourAvailableBlocks[3] == NULL ) {
            xOffset = xOffsetA;
            }
        else {
            xOffset = xOffsetB;
            }
        }
    else {
        double blend = transitionConstructedBlock->getFractionDone();
        

        if( newConstructedBlock == NULL && oldConstructedBlock == NULL ) {
            xOffset = xOffsetA;
            }
        else if( newConstructedBlock != NULL && oldConstructedBlock == NULL ) {
            xOffset = blend * xOffsetB + (1 - blend) * xOffsetA;
            }
        else if( newConstructedBlock == NULL && oldConstructedBlock != NULL ) {
            xOffset = blend * xOffsetA + (1 - blend) * xOffsetB;
            }
        else if( newConstructedBlock != NULL && oldConstructedBlock != NULL ) {
            xOffset = xOffsetB;
            }
        }
    



    // key hints
    
    Vector3D hintPos( -2.25 * 2 / 10.0 + xOffset, -0.675, 0 );
    
    mBackHintSprite.draw( 0, 0, &hintPos, palateScale, 
                          hintFadeFactor, NULL );
    
    
    double nextHintPosA = 1.25 * 2 / 10.0;
    double nextHintPosB = 2.25 * 2 / 10.0;
    
    
    if( transitionConstructedBlock == NULL ) {
        if( ourAvailableBlocks[3] == NULL ) {
            hintPos.mX = nextHintPosA;
            }
        else {
            hintPos.mX = nextHintPosB;
            }
        }
    else {
        double blend = transitionConstructedBlock->getFractionDone();
        
        if( newConstructedBlock == NULL && oldConstructedBlock == NULL ) {
            hintPos.mX = nextHintPosA;
            }
        else if( newConstructedBlock != NULL && oldConstructedBlock == NULL ) {
            hintPos.mX = blend * nextHintPosB + (1 - blend) * nextHintPosA;
            }
        else if( newConstructedBlock == NULL && oldConstructedBlock != NULL ) {
            hintPos.mX = blend * nextHintPosA + (1 - blend) * nextHintPosB;
            }
        else if( newConstructedBlock != NULL && oldConstructedBlock != NULL ) {
            hintPos.mX = nextHintPosB;
            }
        }
    hintPos.mX += xOffset;
    
    

    mNextHintSprite.draw( 0, 0, &hintPos, palateScale, 
                          hintFadeFactor, NULL );


    // insert always centered at 0 as palate centering changes below it
    hintPos.mX = 0;
    hintPos.mY = -0.55;
    
    mInsertHintSprite.draw( 0, 0, &hintPos, palateScale, 
                            hintFadeFactor, NULL );
    

    

    
    double selectorOffset = 0;
    
    for( int b=0; b<4; b++ ) {
        GameObject *block = ourAvailableBlocks[b];
        
        if( block != NULL ) {
            
            double xBlockOffset = ( b - 1.5 ) * 2 / 10.0 + xOffset;
            
            if( b == ourCurrentBlockIndex ) {
                selectorOffset = xBlockOffset;
                }
            
            Vector3D blockPos( xBlockOffset, -0.675, 0 );
            
            block->draw( 0, &blockPos,
                         palateScale,
                         1, NULL );
            }
        
        }
    
    
    // draw selector around current block
    Vector3D selectorPos( selectorOffset, -0.675, 0 );
    
    mSelectorSprite.draw( 0, 0, &selectorPos, 
                          palateScale, 
                          1, NULL );

    
    hintPos.mY = -0.575;
    hintPos.mX = -0.9;
    
    if( mDrawUpHint ) {
        mUpHintSprite.draw( 0, 0, 
                            &hintPos, palateScale, 
                            mUpHintFade * hintFadeFactor, NULL );
        }

    hintPos.mY -= 0.1;
    
    if( mDrawDownHint ) {
        mDownHintSprite.draw( 0, 0, 
                              &hintPos, palateScale, 
                              mDownHintFade * hintFadeFactor, NULL );
        }
    }



void GameSceneHandler::mouseMoved( int inX, int inY ) {
    }



void GameSceneHandler::mouseDragged( int inX, int inY ) {
    }




void GameSceneHandler::mousePressed( int inX, int inY ) {
    }



void GameSceneHandler::mouseReleased( int inX, int inY ) {
    }



void GameSceneHandler::fireRedraw() {
    
    if( mPaused ) {
        // ignore redraw event

        // sleep to avoid wasting CPU cycles
        Thread::staticSleep( 1000 );
        
        // also ignore time that passes while paused
        Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );
        
        return;
        }

    
    // deal with frame timing issues
    
    unsigned long lastMillisecondDelta = mFrameMillisecondDelta;
    
    // how many milliseconds have passed since the last frame
    mFrameMillisecondDelta =
        Time::getMillisecondsSince( mLastFrameSeconds,
                                    mLastFrameMilliseconds );

    
    // lock down to mMaxFrameRate frames per second
    unsigned long minFrameTime = (unsigned long)( 1000 / mMaxFrameRate );
    if( mFrameMillisecondDelta < minFrameTime ) {
        unsigned long timeToSleep = minFrameTime - mFrameMillisecondDelta;
        Thread::staticSleep( timeToSleep );

        // get new frame second delta, including sleep time
        mFrameMillisecondDelta =
            Time::getMillisecondsSince( mLastFrameSeconds,
                                        mLastFrameMilliseconds );
        }

    // avoid huge position "jumps" if we have a very large delay during a frame
    // (possibly caused by something going on in the background)
    // This will favor a slight visual slow down, but this is better than
    // a disorienting jump

    // skip this check if we are just starting up
    if( lastMillisecondDelta != 0 ) {
        if( mFrameMillisecondDelta > 6 * lastMillisecondDelta ) {
            // limit:  this frame represents at most twice the jump of the last
            // frame
            // printf( "Limiting time jump (requested=%lu ms, last=%lu ms)\n",
            //        mFrameMillisecondDelta, lastMillisecondDelta );

            if( mFrameMillisecondDelta > 10000 ) {
                printf( "Time between frames more than 10 seconds:\n" );
                // way too big... investigate
                printf( "Last time = %lu s, %lu ms\n",
                        mLastFrameSeconds, mLastFrameMilliseconds );

                Time::getCurrentTime( &mLastFrameSeconds,
                                      &mLastFrameMilliseconds );
                printf( "current time = %lu s, %lu ms\n",
                        mLastFrameSeconds, mLastFrameMilliseconds );

                }
            
            mFrameMillisecondDelta = 2 * lastMillisecondDelta;
            
            }
        }
    
    
    // record the time that this frame was drawn
    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );

    
    // for use with non-constant time-per-frame
    // this game is constant time-per-frame
    // double frameSecondsDelta = (double)mFrameMillisecondDelta / 1000.0;
    


    mNumFrames ++;

    if( mPrintFrameRate ) {
        
        if( mNumFrames % mFrameBatchSize == 0 ) {
            // finished a batch
            
            unsigned long timeDelta =
                Time::getMillisecondsSince( mFrameBatchStartTimeSeconds,
                                            mFrameBatchStartTimeMilliseconds );

            double frameRate =
                1000 * (double)mFrameBatchSize / (double)timeDelta;
            
            printf( "Frame rate = %f frames/second\n", frameRate );
            
            mFrameBatchStartTimeSeconds = mLastFrameSeconds;
            mFrameBatchStartTimeMilliseconds = mLastFrameMilliseconds;
            }
        }


    // process old clients to clear pending ops and destroy them
    for( int i=0; i<mOldClients.size(); i++ ) {
        Client *c = *( mOldClients.getElement( i ) );
        
        if( c->step() == false ) {
            // done
            
            delete c;

            mOldClients.deleteElement( i );
            
            
            i--;
            }
        }
    


    // deal with current network client operations
    // and pending startup menu operations
    
    if( ! mClient->isError() ) {
        char clientWorkLeft = mClient->step();
        
        if( !clientWorkLeft ) {
            
            // end measurement, if we were conducting one
            if( mCountingServerSteps ) {
                //printf( "Server took %d steps to respond\n", 
                //        mLastServerLatencyNumSteps );
                }
            
            mCountingServerSteps = false;
            }
        

        
        if( mNextWorld != -1 
            && ! clientWorkLeft
            && ! mGoingToSleep
            && mGoingToSleepSteps >= mTargetGoingToSleepSteps
            && mPlayerFadeDelta == 0 ) {
        
            // done fetching any state from server
            // and done with server latency cloaking animation
            

            

            // start transition
            if( mTransitionGrid == NULL ) {
            
                mTransitionGrid = 
                    new GridTransition( &( mBlockGrids[ mCurrentWorld ] ),
                                        &( mBlockGrids[ mNextWorld ] ),
                                        mTransitionWorld,
                                        transitionSteps );

                // transition stars
                int i;
                for( i=0; i<numStars; i++ ) {
                    Star *transitionStar = 
                        *( mStars[ mNumWorlds ].getElement( i ) );
                    
                    transitionStar->shiftBetween(
                        *( mStars[ mCurrentWorld ].getElement( i ) ),
                        *( mStars[ mNextWorld ].getElement( i ) ),
                        transitionSteps );
                    }

                // transition clouds
                for( i=0; i<numClouds; i++ ) {
                    Cloud *transitionCloud = 
                        *( mClouds[ mNumWorlds ].getElement( i ) );
                    
                    transitionCloud->shiftBetween(
                        *( mClouds[ mCurrentWorld ].getElement( i ) ),
                        *( mClouds[ mNextWorld ].getElement( i ) ),
                        transitionSteps );
                    }

                // transition tiles
                int numTiles = mTiles.size();
                for( i=0; i<numTiles; i++ ) {
                    GameObject *tile = *( mTiles.getElement( i ) );
                    
                    tile->shiftBetweenPages( mCurrentWorld, mNextWorld,
                                             transitionSteps );
                    
                    }


                // transition alarm stands
                int numAlarmStands = mAlarmStands.size();
                for( i=0; i<numAlarmStands; i++ ) {
                    GameObject *stand = *( mAlarmStands.getElement( i ) );
                    
                    stand->shiftBetweenPages( mCurrentWorld, mNextWorld,
                                              transitionSteps );
                    
                    }


                // transition constructed block on palate

                if( oldConstructedBlock != NULL || 
                    newConstructedBlock != NULL ) {
                    

                    
                    transitionConstructedBlock =
                        new BlendedBlock( oldConstructedBlock,
                                          newConstructedBlock,
                                          transitionSteps );

                    ourAvailableBlocks[3] = transitionConstructedBlock;
                    }
                
                // auto switch selector position at *start* of transition
                // if it is moving left (to get out of way of hint that 
                // is sliding in
                if( newConstructedBlock == NULL &&
                    ourCurrentBlockIndex == 3 ) {
                    
                    ourCurrentBlockIndex = 2;
                    }
                
                
                

                // set up a transition to position in next world
                /*
                mPlayerXDelta = 
                    ( mLastPlayerX[ mNextWorld ] - mPlayer->mX ) /
                    transitionSteps;
                */
                mPlayer->shiftPosition( mLastPlayerX[ mNextWorld ],
                                        transitionSteps );
                

                mStepsSinceAnimationFrame = 0;

                if( mWorldTransitionDirection == -1 ) {
                    
                    // fade out and y arc
                    mPlayerFadeDelta =  -1.0 / transitionSteps;
                    mPlayerFadeLowerLimit = 0;
                    
                    mPlayerYSineParam = 0;
                    mPlayerYSineDelta = M_PI / transitionSteps;
                    }
                else if( mWorldTransitionDirection == 1 ) {
                    
                    mRubbingEyes = false;
                    
                    // snap to lying down frame (while faded out)
                    mPlayer->setPage( 2 );
                    mPlayer->mSpriteFrame = 7;
                    

                    // fade in during y arc
                    mPlayerFadeDelta = 1.0 / transitionSteps;
                    mPlayerFadeUpperLimit = 1;
                    
                    mPlayerYSineParam = 0;
                    mPlayerYSineDelta = M_PI / transitionSteps;
                    }
                
                }
            }
        

        }
    else {
        //printf( "%s\n", mClient->getErrorString() );
        }
    
    // give panel enough time to be read
    if( mCurrentPanel == mInitialRequestPanel &&
        Time::getCurrentTime() - mInitialRequestPanelShownAtTime > 1 ) {
        // error?
        if( mClient->isError() ) {
            char *errorMessage = mClient->getErrorString();
            
            if( errorMessage != NULL ) {
                setLabelString( mInitialRequestLabel, errorMessage );
                }
            }
        else if( mClient->isClientReady() ) {
            // move on to selection panel
            mMainPanel->remove( mInitialRequestPanel );
            mMainPanel->add( mSelectPanel );
            mCurrentPanel = mSelectPanel;
            }
        }
    else if( mCurrentPanel == mWaitFriendPanel ) {
        // error?
        if( mClient->isError() ) {
            char *errorMessage = mClient->getErrorString();
            
            if( errorMessage != NULL ) {
                setLabelString( mWaitFriendLabel, errorMessage );
                }
            }
        else if( strcmp( mGameIDLabel->getText(), 
                         placeholderGameIDString ) == 0 ) {
                        
            char *clientGameID = mClient->getGameID();
            
            if( clientGameID != NULL ) {
                //printf( "setting ID to %s\n", clientGameID );
                
                mGameIDLabel->setText( clientGameID );
                setLabelString( mWaitFriendLabel, "waitingForFriend" );
                }
            }
        else {
            // game ready yet?

            if( mClient->isGameReady() ) {
                // start playing
                mMainPanel->remove( mCurrentPanel );
                mCurrentPanel = NULL;

                mScreen->removeSceneHandler( mMainPanelGuiTranslator );
                mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
                mGUIVisible = false;
                }
            }
        
         }
    else if( mCurrentPanel == mJoinFriendPanel ) {
        // error?
        if( mClient->isError() ) {
            char *errorMessage = mClient->getErrorString();
            
            if( errorMessage != NULL ) {
                setLabelString( mEnterGameIDLabel, errorMessage );
                }
            // re-enable text field for additional attempts
            mEnterGameIDField->setFocus( true );
            }
        else if( mClient->isGameReady() ) {
            // start playing
            mMainPanel->remove( mCurrentPanel );
            mCurrentPanel = NULL;
            
            mScreen->removeSceneHandler( mMainPanelGuiTranslator );
            mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
            mGUIVisible = false;
            }

        }
    else if( mCurrentPanel == mWaitStrangerPanel ) {
        // error?
        if( mClient->isError() ) {
            char *errorMessage = mClient->getErrorString();
            
            if( errorMessage != NULL ) {
                setLabelString( mWaitStrangerLabel, errorMessage );
                }
            }
        else if( mClient->isGameReady() ) {
            // start playing
            mMainPanel->remove( mCurrentPanel );
            mCurrentPanel = NULL;
            
            mScreen->removeSceneHandler( mMainPanelGuiTranslator );
            mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
            mGUIVisible = false;
            }
        }
    


    if( !showingQuitQuestion ) {
        
        // report dead partner?

        unsigned long seconds = mClient->getSecondsSincePartnerActed();
        
        // ten minutes
        if( seconds > 600 ) {
            
            unsigned long minutes = seconds / 60;
            
            char *sourceString =
                (char *)TranslationManager::translate( "otherSilent" );
            
            char *numberString = autoSprintf( "%lu", minutes );
            
            char found;
            
            char *labelString = replaceOnce( sourceString, "#VAR", 
                                             numberString,
                                             &found );
        
            
            setLabelString( mStatusLabel, labelString );
            
            delete [] numberString;
            delete [] labelString;
            }
        else {
            // blank status
            setLabelString( mStatusLabel, "" );
            }
        }
    
    
    



    // update world object positions

    

    
    mPlayer->mX += mPlayerXDelta;

    // stop at boundary
    if( mPlayer->mX < leftWorldBoundary + 0.625 ) {
        mPlayer->mX = leftWorldBoundary + 0.625;
        
        mPlayer->mSpriteFrame = 0;
        
        mPlayerXDelta = 0;
        }
    if( mPlayer->mX > rightWorldBoundary - 0.5625 ) {
        mPlayer->mX = rightWorldBoundary - 0.5625;
        
        mPlayer->mSpriteFrame = 0;
        
        mPlayerXDelta = 0;
        }
    

    mPlayer->mFadeFactor += mPlayerFadeDelta;
    
    // cap fade and stop at limits
    if( mPlayer->mFadeFactor < mPlayerFadeLowerLimit ) {
        mPlayer->mFadeFactor = mPlayerFadeLowerLimit;
        
        mPlayerFadeDelta = 0;
        }
    if( mPlayer->mFadeFactor > mPlayerFadeUpperLimit ) {
        mPlayer->mFadeFactor = mPlayerFadeUpperLimit;
        
        mPlayerFadeDelta = 0;
        }
    

    
    // advance y arc
    if( mPlayerYSineParam != 0 ) {
        
        mPlayer->mY = playerBaseHeight - 0.5 * sin( mPlayerYSineParam );
        }
    
    if( mPlayerYSineDelta != 0 ) {
        mPlayerYSineParam += mPlayerYSineDelta;
        
        if( mPlayerYSineParam > M_PI ) {
            mPlayerYSineParam = 0;
            mPlayerYSineDelta = 0;
            
            mPlayer->mY = playerBaseHeight;
            }
        }
    


    // don't run walking animation in transition
    // or while waking/sleeping
    if( !mGoingToSleep && !mWakingUp && mNextWorld == -1 ) {
        
        if( mPlayerXDelta != 0 ) {
        
            if( mStepsSinceAnimationFrame >= animationFrameSteps ) {
                mStepsSinceAnimationFrame = 0;
                        

                // update animation
                switch( mPlayer->mSpriteFrame ) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                        mPlayer->mSpriteFrame ++;
                        break;
                    case 12:
                        mPlayer->mSpriteFrame = 1;
                        break;
                    }
                }
            else {
                mStepsSinceAnimationFrame ++;
                }
            }
        else {
            mStepsSinceAnimationFrame = 0;
        
            // switch to still frame
        
            switch( mPlayer->mSpriteFrame ) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    mPlayer->mSpriteFrame = 0;
                    break;
                }
            }
        }
    

    if( mGoingToSleep ) {
        if( mStepsSinceAnimationFrame >= mStepsPerSleepFrame ) {
            mStepsSinceAnimationFrame = 0;
            
            if( mPlayer->mSpriteFrame == 7 ) {
                mGoingToSleep = false;
                }
            else {
                mPlayer->mSpriteFrame ++;
                }
            }
        
        else {
            mStepsSinceAnimationFrame ++;
            }
        }
       
    if( mWakingUp ) {
        if( mStepsSinceAnimationFrame >= mStepsPerSleepFrame ) {
            mStepsSinceAnimationFrame = 0;
            
            if( mPlayer->mSpriteFrame == 0 ) {
                mWakingUp = false;
                mPlayer->setPage( 0 );
                }
            else {
                mPlayer->mSpriteFrame --;
                }
            }
        
        else {
            mStepsSinceAnimationFrame ++;
            }
        }


    if( mRubbingEyes ) {
        if( mStepsSinceAnimationFrame >= rubbingEyesFrameSteps ) {
            mStepsSinceAnimationFrame = 0;
            
            mPlayer->mSpriteFrame += mRubbingDirection;
            
            if( mPlayer->mSpriteFrame > 15 ) {
                mPlayer->mSpriteFrame = 14;
                mRubbingDirection = - mRubbingDirection;
                }
            if( mPlayer->mSpriteFrame < 13 ) {
                mPlayer->mSpriteFrame = 14;
                mRubbingDirection = - mRubbingDirection;
                }
            }        
        else {
            mStepsSinceAnimationFrame ++;
            }
        }



    // respond to key held down that was ignored during transition
    if( !mGoingToSleep && !mWakingUp && mNextWorld == -1 ) {
        if( mLeftKeyDown && ! mRightKeyDown ) {
            mPlayer->setFlipHorizontal( false );
            mPlayer->setHorizontalOffset( 0 );
            mPlayerXDelta = - playerSpeed;
            }
        else if( mRightKeyDown && ! mLeftKeyDown ) {
            mPlayer->setFlipHorizontal( true );
            mPlayer->setHorizontalOffset( -1 );
            mPlayerXDelta = + playerSpeed;
            }
        }
    
    
    if( mHoldingBlock != NULL ) {
        mHoldingBlock->mX = mPlayer->mX;
        }
    

    double oldSelectorX = mSelector->mX;
    
    mSelector->mX = rint( mPlayer->mX );
    
    if( oldSelectorX != mSelector->mX ) {
        mSelector->mFadeFactor = 0;
        }
    

    if( mNextWorld == -1 ) {
        // fade selector in
        if( mSelector->mFadeFactor < 1 && mPlayerXDelta == 0 ) {
            mSelector->mFadeFactor += 0.02;
            if( mSelector->mFadeFactor > 1 ) {
                mSelector->mFadeFactor = 1;
                }
            }
        }
    else {
        // fade selector out
        if( mSelector->mFadeFactor > 0 ) {
            mSelector->mFadeFactor -= 0.02;
            if( mSelector->mFadeFactor < 0 ) {
                mSelector->mFadeFactor = 0;
                }
            }
        }
    
    if( mSelector->mX < -20 || mSelector->mX > 19 ) {
        // off grid
        mSelectorInBounds = false;
        
        mSelector->mFadeFactor = 0;

        if( mSelector->mX < -20 ) {
            mSelector->mX = -20;
            }
        if( mSelector->mX > 19 ) {
            mSelector->mX = 19;
            }
        

        if( mHoldingBlock ) {
            // drop it
            mWorlds[mCurrentWorld].remove( mHoldingBlock );

            insertBlock( mHoldingBlock );
            mHoldingBlock = NULL;
            mPlayer->setPage( 0 );
            }
        }
    else {
        mSelectorInBounds = true;
        }
    
    
    
    // auto-fade up/down hints
    if( mDrawUpHint && mUpHintFade < 1 ) {
        mUpHintFade += 0.02;
        if( mUpHintFade > 1 ) {
            mUpHintFade = 1;
            }
        }    
    if( mDrawDownHint && mDownHintFade < 1 ) {
        mDownHintFade += 0.02;
        if( mDownHintFade > 1 ) {
            mDownHintFade = 1;
            }
        }

    
    if( ! fadeOutHints ) {
    
        char allUsedEnough = true;
        
        for( int h=0; h<NUM_HINT_TYPES; h++ ) {
            if( hintUsedCount[ h ] < numTimesHintsUsedBeforeFade ) {
                allUsedEnough = false;
                }
            }
        fadeOutHints = allUsedEnough;
        }
    else if( hintFadeFactor > 0 ) {
        
        hintFadeFactor -= 0.001;
        
        if( hintFadeFactor < 0 ) {
            hintFadeFactor = 0;
            }
        }
    
    




    if( mHoldingBlock != NULL && 
        mBlockGrids[mCurrentWorld].isColumnFull( (int)( mSelector->mX ) ) ) {
        
        // too full to add the block we're holding
        // hide selector
        mSelector->mFadeFactor = 0;
        }
    
    
    
    boundSelectorY();
    

    


    for( int w=0; w<mNumWorlds + 1; w++ ) {
        mWorlds[w].step();
        }
    
    if( transitionConstructedBlock != NULL ) {
        transitionConstructedBlock->step();
        }
    
    // step player object manually
    mPlayer->mainStep();
    


    if( mTransitionGrid != NULL ) {
        if( mTransitionGrid->isDone() ) {
            delete mTransitionGrid;
            
            mTransitionGrid = NULL;

            // switch to next world
            mPlayer->mX = mLastPlayerX[ mNextWorld ];
            mCurrentWorld = mNextWorld;
            mNextWorld = -1;

            mTileSprite.setPage( mCurrentWorld );
            
            mAlarmStandSprite.setPage( mCurrentWorld );
            
            
            // switch to newly constructed block
            if( oldConstructedBlock != NULL ) {
                delete oldConstructedBlock;
                }
            if( transitionConstructedBlock != NULL ) {
                delete transitionConstructedBlock;
                }
            
            ourAvailableBlocks[3] = newConstructedBlock;
            
            oldConstructedBlock = NULL;
            newConstructedBlock = NULL;
            transitionConstructedBlock = NULL;
            
            // auto-switch palate selector to new block
            if( ourAvailableBlocks[3] != NULL ) {
                ourCurrentBlockIndex = 3;
                }
            // already moved it left in case where new block is NULL
            // at start of transition


            mPlayerXDelta = 0;
            

            if( mWorldTransitionDirection == -1 ) {
                
                // switch to standing
                mPlayer->mSpriteFrame = 0;
                mPlayer->setPage( 0 );
                

                // fade in
                int fadeSteps = mLastServerLatencyNumSteps;
                
                // watch for server latency that's too short
                if( fadeSteps < 
                    numSleepFrames * minSleepAnimationFrameSteps ) {
                    
                    fadeSteps = numSleepFrames * minSleepAnimationFrameSteps;
                    }
            
                mPlayerFadeDelta = 1.0 / fadeSteps;
                mPlayerFadeUpperLimit = 1;
                }
            else if( mWorldTransitionDirection == 1 ) {
                mGoingToSleep = false;

                // fixed rate for standing back up
                mStepsPerSleepFrame = minSleepAnimationFrameSteps;

                mPlayer->mSpriteFrame = 7;
                mWakingUp = true;
                mPlayer->setPage( 2 );
                }
            
            // loudness fades back up
            loudnessAdjustTarget = 1.0;
            }
        }
    
            

    // set target block states (only in current world)
    
    int numBlocks = mTargetBlocks[mCurrentWorld].size();

    // count number of matches in each row
    int numMatched[9] = {0,0,0,0,0,0,0,0,0};
    
    char anyMismatched = false;
    

    int i;
    
    for( i=0; i<numBlocks; i++ ) {
        TargetBlock *b = *( mTargetBlocks[mCurrentWorld].getElement( i ) );
        
        Block *realBlock = 
            mBlockGrids[mCurrentWorld].getBlock( (int)( b->mX ),
                                                 (int)( b->mY ) );
        if( realBlock == NULL ) {
            b->setState( 0 );
            }
        else {
            if( b->equal( realBlock ) ) {
                b->setState( 2 );
                // match
                numMatched[ (int)(b->mY) - 1 ] ++;
                }
            else {
                // mismatch
                b->setState( 1 );
                anyMismatched = true;
                }
            }
        }

    int highestRowWithFullMatch = -1;
    
    for( int r=0; r<9; r++ ) {
        if( numMatched[r] == 3 ) {
            highestRowWithFullMatch = r;
            }
        }
    
    if( anyMismatched ) {
        mMusicTrackFadeLevelTarget[mCurrentWorld] = 0;
        }
    else {
        // fade level based on higest row with full match
        // (each row essentially turns on a new track)

        mMusicTrackFadeLevelTarget[mCurrentWorld] = 
            ( highestRowWithFullMatch + 1 ) / 9.0;
        
        if( highestRowWithFullMatch != 8 ) {
            // not completely full
            
            // let fullness of row above last full row tweak volume of
            // track up a bit more
            
            // so track doesn't turn on suddenly when row is finally full

            // also, gives player *some* feedback during completion of 
            // first row.

            mMusicTrackFadeLevelTarget[mCurrentWorld] +=
                ( numMatched[ highestRowWithFullMatch + 1 ] / 3.0 ) / 9.0;
            
            }
        }
    
    
    double musicTrackFadeLevelTarget = 
        mMusicTrackFadeLevelTarget[ mCurrentWorld ];
    
    if( mTransitionGrid != NULL ) {
        // in transition to next world

        // volume is down, so we can jump instantly to new track fade level

        musicTrackFadeLevelTarget = 
            mMusicTrackFadeLevelTarget[ mNextWorld ];
        
        musicTrackFadeLevel = musicTrackFadeLevelTarget;
        }
    


    // move fade level toward target
    if( musicTrackFadeLevel != musicTrackFadeLevelTarget ) {
        int dir = -1;
        if( musicTrackFadeLevelTarget > musicTrackFadeLevel ) {
            dir = 1;
            }
        musicTrackFadeLevel += dir * 0.005;
        
        if( ( dir == -1 && musicTrackFadeLevel < musicTrackFadeLevelTarget )
            ||
            ( dir == 1 && musicTrackFadeLevel > musicTrackFadeLevelTarget ) ) {
            musicTrackFadeLevel = musicTrackFadeLevelTarget;
            }
        }

    // move loudness adjust toward target
    if( loudnessAdjust != loudnessAdjustTarget ) {
        int dir = -1;
        if( loudnessAdjustTarget > loudnessAdjust ) {
            dir = 1;
            }
        loudnessAdjust += dir * 0.05;
        
        if( ( dir == -1 && loudnessAdjust < loudnessAdjustTarget )
            ||
            ( dir == 1 && loudnessAdjust > loudnessAdjustTarget ) ) {
            loudnessAdjust = loudnessAdjustTarget;
            }
        }
    
    
    // overall loudness (not track selection) based on distance from tower 
    
    double xDistance = fabs( mPlayer->mX - towerX );    

    // assume we are 1 unit in front of tower (so volume = 1 when 
    //   xDistance = 0 )

    // multiplying by fraction makes it wider
    double widthFactor = 0.5;
    
    double distance = sqrt( 1 + widthFactor * xDistance * xDistance );
    
    double loudness = 1 / ( distance * distance );
    
    setMusicLoudness( loudness * loudnessAdjust );
    

    

    // fade constructor hint in if constructor full of blocks

    Block *constructorBlocks[4];
        
    constructorBlocks[0] = mBlockGrids[mCurrentWorld].getBlock( -1, 2 );
    constructorBlocks[1] = mBlockGrids[mCurrentWorld].getBlock( 0, 2 );
    constructorBlocks[2] = mBlockGrids[mCurrentWorld].getBlock( -1, 1 );
    constructorBlocks[3] = mBlockGrids[mCurrentWorld].getBlock( 0, 1 );
    
    char allNonNull = true;
    for( int b=0; b<4; b++ ) {
        if( constructorBlocks[b] == NULL ) {
            allNonNull = false;
            }
        }
    if( allNonNull && mNextWorld == -1 ) {
        
        mBuilderHint->mFadeFactor += 0.02;
        
        if( mBuilderHint->mFadeFactor > 1 ) {
            mBuilderHint->mFadeFactor = 1;
            }
        }
    else {
        mBuilderHint->mFadeFactor -= 0.02;
        
        if( mBuilderHint->mFadeFactor < 0 ) {
            mBuilderHint->mFadeFactor = 0;
            }
        }
    


    // shimmer state of constructor targets

    numBlocks = mBuilderTargets[mCurrentWorld].size();
    
    for( i=0; i<numBlocks; i++ ) {
        BuilderTargetBlock *b = 
            *( mBuilderTargets[mCurrentWorld].getElement( i ) );
        
        Block *realBlock = 
            mBlockGrids[mCurrentWorld].getBlock( (int)( b->mX ),
                                                 (int)( b->mY ) );
        if( realBlock == NULL ) {
            b->setState( 0 );
            }
        else {
            // block present
            b->setState( 2 );
            }
        }




    if( mCountingServerSteps ) {
        mLastServerLatencyNumSteps ++;
        }
    

    mGoingToSleepSteps ++;
    }



void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( mNextWorld != -1 || mWakingUp || mGoingToSleep ) {
        return;
        }

    
    // check if world changed
    int initWorldChange = false;
    
    
    if( mGUIVisible ) {

        if( mMainPanel->contains( mTitlePanel ) ) {
            // q or escape
            if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                exit( 0 );
                }
            
            // any other key advances from title
            mMainPanel->remove( mCurrentPanel );
            
            mMainPanel->add( mInitialRequestPanel );
            mCurrentPanel = mInitialRequestPanel;
            mInitialRequestPanelShownAtTime = Time::getCurrentTime();
            }
        else if( mMainPanel->contains( mInitialRequestPanel ) ) {
            // q or escape
            if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                exit( 0 );
                }
            }
        else if( mMainPanel->contains( mSelectPanel ) ) {
            
            // q or escape
            if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                exit( 0 );
                }
    
            // any key press triggers selection

            GUIPanelGL *panelToShow = NULL;
            
            switch( mSelectHighlightIndex ) {
                case 0:
                    panelToShow = mWaitFriendPanel;
                    mEnterGameIDField->setFocus( false );
            
                    setLabelString( mWaitFriendLabel, "creatingGame" );
                    setLabelString( mGameIDLabel, placeholderGameIDString );
            
                    mClient->createGame();
                    
                    break;
                case 1:
                    panelToShow = mJoinFriendPanel;
                    
                    setLabelString( mEnterGameIDLabel, "enterCode" );
                    mEnterGameIDField->setFocus( true );                    
                    break;
                case 2:
                    panelToShow = mWaitStrangerPanel;
                    mEnterGameIDField->setFocus( false );
                    
                    mClient->joinGame( NULL );
                    break;
                }
            
            if( panelToShow != NULL ) {
                mMainPanel->remove( mSelectPanel );
                mMainPanel->add( panelToShow );
                mCurrentPanel = panelToShow;
                }
            }
        else {
            
            if( mMainPanel->contains( mJoinFriendPanel ) ) {
                // look for enter
                if( inKey == 13 && mEnterGameIDField->isFocused() ) {
                    
                    char *enteredID = mEnterGameIDField->getText();
                    
                    if( strlen( enteredID ) > 0 ) {
                        // disable further input
                        mEnterGameIDField->setFocus( false );
                        
                        setLabelString( mEnterGameIDLabel, 
                                        "joiningWithFriend" );
                        
                        // try again, clearing any old errors
                        mClient->clearError();
                        
                        mClient->joinGame( enteredID );
                        }
                    }
                }
        
            // q or escape pressed in any sub-panel
            if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                
                // back up
                
                // wait for this client to finish before destroying it
                mOldClients.push_back( mClient );

                // fresh client
                mClient = new Client();
                
                
                mMainPanel->remove( mCurrentPanel );

                if( mClient->isClientReady() ) {
                    mMainPanel->add( mSelectPanel );
                    mCurrentPanel = mSelectPanel;
                    }
                else {
                    // still waiting for ID?
                    
                    // clear any error message
                    setLabelString( mInitialRequestLabel, "initialRequest" );
                    

                    mMainPanel->add( mInitialRequestPanel );
                    mCurrentPanel = mInitialRequestPanel;
                    mInitialRequestPanelShownAtTime = Time::getCurrentTime();
                    }
                }    
            }
        
        return;
        }
    
        


    // q or escape
    if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
        if( !showingQuitQuestion ) {
            oldStatusText = stringDuplicate( mStatusLabel->getText() );
            
            showingQuitQuestion = true;
            
            setLabelString( mStatusLabel, "quitQuestion" );
            }
        }
    // answer to quit question?
    else if( showingQuitQuestion && ( inKey == 'y' || inKey == 'Y' ) ) {
        delete [] oldStatusText;
        oldStatusText = NULL;
        
        exit( 0 );
        }
    else if( showingQuitQuestion && ( inKey == 'n' || inKey == 'N' ) ) {
        setLabelString( mStatusLabel, oldStatusText );

        delete [] oldStatusText;
        oldStatusText = NULL;
        
        showingQuitQuestion = false;
        }
    else if( inKey == ' ' ) {
        hintUsedCount[ HINT_SPACE ] ++;
        
        if( mSelectorInBounds ) {
            
            boundSelectorY();

            int x = (int)( mSelector->mX );
            int y = (int)( mSelector->mY );
            
            Block *existingBlock = mBlockGrids[mCurrentWorld].getBlock( x, y );
            
            if( mHoldingBlock != NULL || existingBlock == NULL ) {   
                if( !mBlockGrids[mCurrentWorld].isColumnFull( x ) ) {
                    
                    //printf( "Adding block\n" );
                    
                    Block *b;
                
                    if( mHoldingBlock != NULL ) {
                        b = mHoldingBlock;
                        mWorlds[mCurrentWorld].remove( mHoldingBlock );
                        mHoldingBlock = NULL;
                        }
                    else {
                        Block *palateBlock =
                            (Block *)( 
                                ourAvailableBlocks[ourCurrentBlockIndex] );
                    
                        b = palateBlock ->copy();
                        }
                
                    insertBlock( b );

                    mPlayer->setPage( 0 );
                    }
                }
            else {
                //printf( "Removing block\n" );

                mHoldingBlock = mBlockGrids[mCurrentWorld].removeBlock( x, y );
            
                mHoldingBlock->mX = mPlayer->mX;
                mHoldingBlock->mY = mPlayer->mY + baseScale * 11;
                mHoldingBlock->mDepthLayer = 1;
            
                mWorlds[ mCurrentWorld ].add( mHoldingBlock );

                mPlayer->setPage( 1 );
                }
            }
        
        }
    else if( inKey == 'w' || inKey == 'W' ) {
        mPlayerXDelta = 0;
        
        if( mHoldingBlock != NULL ) {
            mWorlds[mCurrentWorld].remove( mHoldingBlock );
            
            boundSelectorY();
            
            insertBlock( mHoldingBlock );
            mHoldingBlock = NULL;
            mPlayer->setPage( 0 );
            }

        oldConstructedBlock = (Block*)( ourAvailableBlocks[3] );
        

        // build new available block out of blocks in constructor
        Block *constructorBlocks[4];
        
        constructorBlocks[0] = mBlockGrids[mCurrentWorld].getBlock( -1, 2 );
        constructorBlocks[1] = mBlockGrids[mCurrentWorld].getBlock( 0, 2 );
        constructorBlocks[2] = mBlockGrids[mCurrentWorld].getBlock( -1, 1 );
        constructorBlocks[3] = mBlockGrids[mCurrentWorld].getBlock( 0, 1 );
            
        char allNonNull = true;
        for( int b=0; b<4; b++ ) {
            if( constructorBlocks[b] == NULL ) {
                allNonNull = false;
                }
            }
        if( allNonNull ) {
            newConstructedBlock = new Block( constructorBlocks );
            }


        // move on to next world
        mWorldTransitionDirection = +1;
        
        mNextWorld = mCurrentWorld + 1;
        if( mNextWorld >= mNumWorlds ) {
            mNextWorld = 0;
            }
        initWorldChange = true;

        
        // fade out while we wait for server
        // time the fade to the last server time
        mStepsSinceAnimationFrame = 0;
        if( mLastServerLatencyNumSteps > 0 ) {
            mStepsPerSleepFrame = mLastServerLatencyNumSteps / numSleepFrames;
            
            if( mStepsPerSleepFrame < minSleepAnimationFrameSteps ) {
                mStepsPerSleepFrame = minSleepAnimationFrameSteps;
                }
            }
        mPlayerFadeDelta = -1.0 / ( mStepsPerSleepFrame * numSleepFrames );
        mPlayerFadeLowerLimit = 0.0;
        
        mRubbingEyes = true;
        mRubbingDirection = 1;
        mPlayer->mSpriteFrame = 13;
        
        // wake up to wherever you went to sleep last in the next world
        // ( leave mLastPlayerX alone for new world )
        }
    else if( inKey == 's' || inKey == 'S' ) {
        mPlayerXDelta = 0;

        if( mHoldingBlock != NULL ) {
            mWorlds[mCurrentWorld].remove( mHoldingBlock );
            
            boundSelectorY();
            
            insertBlock( mHoldingBlock );
            mHoldingBlock = NULL;
            mPlayer->setPage( 0 );
            }

        oldConstructedBlock = (Block *)( ourAvailableBlocks[3] );
        newConstructedBlock = NULL;
        

        mWorldTransitionDirection = -1;
        
        mNextWorld = mCurrentWorld - 1;        
        if( mNextWorld < 0 ) {
            mNextWorld = mNumWorlds - 1;
            }
        initWorldChange = true;
        
        // run lie down animation while we wait for server
        mGoingToSleep = true;
        mStepsSinceAnimationFrame = 0;
        mPlayer->mSpriteFrame = 0;
        mPlayer->setPage( 2 );
        
        // don't time animation to server response time
        // instead, run animation at fixed speed and let player lie there
        // to wait for server
        mStepsPerSleepFrame = minSleepAnimationFrameSteps;
        
        // make sure we wait enough steps total (lying there for a while
        // if necessary)
        mGoingToSleepSteps = 0;
        mTargetGoingToSleepSteps = mLastServerLatencyNumSteps;
        

        // when going to sleep, you go to default starting location
        // for the world
        mLastPlayerX[ mNextWorld ] = 0;
        }
    else if( inKey == 'n' || inKey == 'N' ) {
        hintUsedCount[ HINT_N ] ++;
        
        ourCurrentBlockIndex ++;
        if( ourCurrentBlockIndex > 3 || 
            ourAvailableBlocks[ourCurrentBlockIndex] == NULL ) {
            ourCurrentBlockIndex = 0;
            }        
        }
    else if( inKey == 'b' || inKey == 'B' ) {
        hintUsedCount[ HINT_B ] ++;

        ourCurrentBlockIndex --;
        if( ourCurrentBlockIndex < 0 ) {
            
            if( ourAvailableBlocks[3] != NULL ) {
                ourCurrentBlockIndex = 3;
                }
            else {
                ourCurrentBlockIndex = 2;
                }
            }        
        }
    


    if( initWorldChange ) {
        // fade music loudness at start of world change
        loudnessAdjustTarget = 0;
        
        
        // save position in this world
        mLastPlayerX[ mCurrentWorld ] = mPlayer->mX;
                
        int sharedWorldIndex = mClient->getSharedWorldIndex( mNumWorlds );
        // printf( "Shared world = %d\n", sharedWorldIndex );
        
        if( mCurrentWorld == sharedWorldIndex ) {
            // leaving shared world
            // post move
            //printf( "Posting move\n" );
            
            char requestSent = 
                mClient->postMove( &( mBlockGrids[sharedWorldIndex] ) );
            mBlockGrids[sharedWorldIndex].clearDirtyFlags();

            if( requestSent ) {
                // measure latency
                mCountingServerSteps = true;
                mLastServerLatencyNumSteps = 0;
                }
            }
        
        else if( mNextWorld == sharedWorldIndex ) {
            // entering shared world

            // get changes

            //printf( "Fetching changes\n" );
            mClient->getChanges( &( mBlockGrids[sharedWorldIndex] ) );

            // measure latency
            mCountingServerSteps = true;
            mLastServerLatencyNumSteps = 0;
            }

        
        
        }
    
    }



void GameSceneHandler::keyReleased(
	unsigned char inKey, int inX, int inY ) {

    }



void GameSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {

    // keep key states for later, whether we act on them now or not
    switch( inKey ) {
        case MG_KEY_LEFT:
            mLeftKeyDown = true;
            break;
        case MG_KEY_RIGHT:
            mRightKeyDown = true;
            break;
        }
    

    if( mNextWorld != -1 || mGoingToSleep ) {
        return;
        }
    
    if( mWakingUp ) {
        // allow vertical motion of selector only
        switch( inKey ) {
            case MG_KEY_UP:
                hintUsedCount[ HINT_UP ] ++;
                
                mSelector->mY += 1;
                break;
            case MG_KEY_DOWN:
                hintUsedCount[ HINT_DOWN ] ++;

                mSelector->mY -= 1;
                break;
            }
        return;
        }
    

    switch( inKey ) {
        case MG_KEY_LEFT:
            mPlayerXDelta = - playerSpeed;
            mPlayer->setFlipHorizontal( false );
            mPlayer->setHorizontalOffset( 0 );
            mPlayer->mSpriteFrame = 0;
            break;
        case MG_KEY_RIGHT:
            mPlayerXDelta = + playerSpeed;
            mPlayer->setFlipHorizontal( true );
            mPlayer->setHorizontalOffset( -1 );
            mPlayer->mSpriteFrame = 0;
            break;
        case MG_KEY_UP:
            hintUsedCount[ HINT_UP ] ++;

            mSelector->mY += 1;

            if( mGUIVisible &&
                mMainPanel->contains( mSelectPanel ) ) {
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( false );
                
                mSelectHighlightIndex --;
                if( mSelectHighlightIndex < 0 ) {
                    mSelectHighlightIndex = 2;
                    }
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( true );
                }
            break;
        case MG_KEY_DOWN:
            hintUsedCount[ HINT_DOWN ] ++;
            
            mSelector->mY -= 1;
            
            if( mGUIVisible &&
                mMainPanel->contains( mSelectPanel )  ) {
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( false );
                
                mSelectHighlightIndex ++;
                if( mSelectHighlightIndex > 2 ) {
                    mSelectHighlightIndex = 0;
                    }
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( true );
                }
            break;
        }
    
            
    
	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

    // keep key states for later, whether we act on them now or not
    switch( inKey ) {
        case MG_KEY_LEFT:
            mLeftKeyDown = false;
            break;
        case MG_KEY_RIGHT:
            mRightKeyDown = false;
            break;
        }


    if( mNextWorld != -1 || mWakingUp || mGoingToSleep ) {
        return;
        }
    

    switch( inKey ) {
        case MG_KEY_LEFT:
            if( mPlayerXDelta == - playerSpeed ) {
                mPlayerXDelta = 0;
                }
            break;
        case MG_KEY_RIGHT:
              if( mPlayerXDelta == + playerSpeed ) {
                mPlayerXDelta = 0;
                }
            break;
        }
	} 




void GameSceneHandler::insertBlock( Block *inBlock ) {
    inBlock->mX = rint( mSelector->mX );
    inBlock->mY = mSelector->mY;
    
    
    int x = (int)( inBlock->mX );
    int y = (int)( inBlock->mY );

    char result = mBlockGrids[mCurrentWorld].add( inBlock, x, y );
    
    if( !result ) {
        delete inBlock;
        }
    else {
        inBlock->fadeIn();
        }
    }



void GameSceneHandler::boundSelectorY() {
    
    int aboveTopBlockY = 1;
    int blockX = (int)( mSelector->mX );
    
    while( mBlockGrids[mCurrentWorld].getBlock( blockX,
                                                aboveTopBlockY ) != NULL ) {
        aboveTopBlockY ++;
        }

        
    char columnFull = mBlockGrids[mCurrentWorld].isColumnFull( blockX );
    
    if( mSelector->mY > aboveTopBlockY ) {
        
        mSelector->mY = aboveTopBlockY;
        }
    if( columnFull ) {
        if( mSelector->mY > aboveTopBlockY - 1 ) {
            mSelector->mY = aboveTopBlockY - 1;
            }
        }
    if( mSelector->mY < 1 ) {
        mSelector->mY = 1;
        }
    
    int canMoveUp = 1;
    int canMoveDown = 1;

    if( mSelector->mY == aboveTopBlockY ||
        ( columnFull && 
          ( mSelector->mY == aboveTopBlockY - 1 ) ) ) {
        
        canMoveUp = 0;
        }
    
    if( mSelector->mY == 1 ) {
        canMoveDown = 0;
        }
    
    // turn them on once and forever after they are first usable
    if( canMoveUp ) {
        mDrawUpHint = true;
        }
    if( canMoveDown ) {
        mDrawDownHint = true;
        }
    
    
    // leave at 0, since we're drawing the other up/down hints now
    // convert to binary digits to index sprite frame
    // mSelector->mSpriteFrame = ( canMoveDown << 1 | canMoveUp );
    
    }

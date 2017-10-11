
#include "Client.h"
#include "common.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include <time.h>


char *Client::sServerURL = NULL;


void Client::staticFree() {
    if( sServerURL != NULL ) {
        delete [] sServerURL;
        sServerURL = NULL;
        }
    }



Client::Client()
        : mPlayerID( NULL ),
          mGameID( NULL ),
          mGamePasscode( NULL ),
          mPlayerNumber( NULL ),
          mGameReady( false ),
          mTimeLastCheckedReady( 0 ),
          // every 5 seconds
          mTimeBetweenChecks( 5 ),
          mTimeLastReportedAlive( 0 ),
          // every 5 minutes
          mTimeBetweenReports( 300 ), 
          mSecondsSincePartnerActed( 0 ),
          mLastStateSeen( stringDuplicate( "0" ) ),
          mGridToUpdate( NULL ),
          mError( false ),
          mErrorString( NULL ),
          mCurrentType( NONE ),
          mCurrentRequest( NULL ) {
    
    // default transform
    for( int i=0; i<16; i++ ) {
        mBlockTransform[i] = i;
        }
    

    mServerFinderURL = SettingsManager::getStringSetting( "serverFinderURL" );
    

    if( mServerFinderURL != NULL ) {
        
        if( sServerURL == NULL ) {
            
            QueuedRequest *q = new QueuedRequest( GET_SERVER_URL,
                                                  "GET",
                                                  mServerFinderURL,
                                                  NULL );
            mQueuedRequests.push_back( q );
    
            step();
            }
        }
    else {
        setError( "ERROR:  No serverFinderURL in settings folder." );
        }
    
    


    mPlayerID = SettingsManager::getStringSetting( "playerID" );

    if( sServerURL != NULL && mPlayerID == NULL ) {
        // request an ID
        QueuedRequest *q = 
            new QueuedRequest( GET_ID,
                               "POST",
                               sServerURL,
                               "action=get_player_id" );
        mQueuedRequests.push_back( q );
        }
    }

        

Client::~Client() {
    if( mServerFinderURL != NULL ) {
        delete [] mServerFinderURL;
        }
    
    if( mCurrentRequest != NULL ) {
        delete mCurrentRequest;
        }

    if( mPlayerID != NULL ) {
        delete [] mPlayerID;
        }
    
    if( mGameID != NULL ) {
        delete [] mGameID;
        }
    if( mGamePasscode != NULL ) {
        delete [] mGamePasscode;
        }
    if( mPlayerNumber != NULL ) {
        delete [] mPlayerNumber;
        }
    
    delete [] mLastStateSeen;
    

    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }
    
    
    for( int i=0; i<mQueuedRequests.size(); i++ ) {
        delete *( mQueuedRequests.getElement( i ) );
        }
    }



char Client::step() {
    
    // check if it's time to auto-generate new requests


    unsigned long curTime = time( NULL );
    if( mGameID != NULL
        &&
        ! mGameReady 
        &&
        curTime - mTimeLastCheckedReady > mTimeBetweenChecks ) {
        
        mTimeLastCheckedReady = curTime;
        

        char *actionString = 
            autoSprintf( "action=is_partner_ready"
                         "&game_id=%s"
                         "&game_passcode=%s"
                         "&player_number=%s",
                         mGameID, mGamePasscode, mPlayerNumber );
        
        QueuedRequest *q = new QueuedRequest( IS_PARTNER_READY,
                                              "POST",
                                              sServerURL,
                                              actionString );
        delete [] actionString;
        
        mQueuedRequests.push_back( q );
        }
    else if( mGameID != NULL
        &&
        mGameReady 
        &&
        curTime - mTimeLastReportedAlive > mTimeBetweenReports ) {
        
        // set new time here (even though response not received yet)
        // to avoid sending multiple reports before first response seen.
        mTimeLastReportedAlive = curTime;
        

        char *actionString = 
            autoSprintf( "action=report_alive"
                         "&game_id=%s"
                         "&game_passcode=%s"
                         "&player_number=%s",
                         mGameID, mGamePasscode, mPlayerNumber );
        
        QueuedRequest *q = new QueuedRequest( REPORT_ALIVE,
                                              "POST",
                                              sServerURL,
                                              actionString );
        delete [] actionString;
        
        mQueuedRequests.push_back( q );
        }
        
        
    


    // now process requests that have already been created
    if( mCurrentRequest != NULL ) {
        int stepResult = mCurrentRequest->step();
        
        if( stepResult == -1 ) {
            // error
            setError( "ERROR:  Failed to get a response from server." );
            
            delete mCurrentRequest;
            mCurrentRequest = NULL;
            mCurrentType = NONE;
            
            return step();
            }
        else if( stepResult == 0 ) {
            // still waiting
            return true;
            }
        else if( stepResult == 1 ) {
            // done
            
            char *result = mCurrentRequest->getResult();

            //printf( "Result from server: %s\n", result );
            
                
            SimpleVector<char *> *tokens = tokenizeString( result );
            
            char resultWellFormatted = true;
            

            if( strstr( result, "ERROR:" ) != NULL ) {
                setError( result );
                }
            else if( mCurrentType == GET_SERVER_URL ) {
                if( tokens->size() == 1 ) {
                    char *returnedURL = *( tokens->getElement( 0 ) );
                
                    if( sServerURL != NULL ) {
                        delete [] sServerURL;
                        }
                    
                    sServerURL = stringDuplicate( returnedURL );

                    
                    // we have a server URL
                    
                    // send a request for a player ID, if needed
                    if( mPlayerID == NULL ) {
                        // request an ID
                        QueuedRequest *q = 
                            new QueuedRequest( GET_ID,
                                               "POST",
                                               sServerURL,
                                               "action=get_player_id" );
                        mQueuedRequests.push_back( q );
                        }

                    

                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == GET_ID ) {
                if( tokens->size() == 1 ) {
                    char *returnedID = *( tokens->getElement( 0 ) );
                
                    mPlayerID = stringDuplicate( returnedID );

                    SettingsManager::setSetting( "playerID", mPlayerID );
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == CREATE ) {
                if( tokens->size() == 4 ) {
                    mGameID = stringDuplicate( *( tokens->getElement( 0 ) ) );
                    mGamePasscode = 
                        stringDuplicate( *( tokens->getElement( 1 ) ) );
                    mPlayerNumber = 
                        stringDuplicate( *( tokens->getElement( 2 ) ) );

                    setBlockTransformFromString(
                        *( tokens->getElement( 3 ) ) );
                    

                    // wait before checking
                    mTimeLastCheckedReady = time( NULL );
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == JOIN_FRIEND ||
                     mCurrentType == JOIN_STRANGER ) {
                if( tokens->size() == 4 ) {
                    mGameID = 
                        stringDuplicate( *( tokens->getElement( 0 ) ) );
                    mGamePasscode = 
                        stringDuplicate( *( tokens->getElement( 1 ) ) );
                    mPlayerNumber = 
                        stringDuplicate( *( tokens->getElement( 2 ) ) );
                    
                    setBlockTransformFromString(
                        *( tokens->getElement( 3 ) ) );
                    
                    if( mCurrentType == JOIN_FRIEND ) {
                        // ready now
                        mGameReady = true;
                        }
                    else {
                        // still need to check for ready
                        mTimeLastCheckedReady = time( NULL );
                        }
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == IS_PARTNER_READY ) {
                if( tokens->size() == 1 ) {
                    char *readyFlag = *( tokens->getElement( 0 ) );
                
                    if( readyFlag[0] == '1' ) {
                        mGameReady = true;
                        }

                    mTimeLastCheckedReady = time( NULL );
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == REPORT_ALIVE ) {
                if( tokens->size() == 1 ) {
                    char *successFlag = *( tokens->getElement( 0 ) );
                
                    if( successFlag[0] != '1' ) {
                        setError( "ERROR:  Reporting alive status failed." );
                        }
                    
                    else {
                        // server know's we're alive
                        mTimeLastReportedAlive = time( NULL );
                        }
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == POST_MOVE ) {
                if( tokens->size() == 1 ) {
                    char *successFlag = *( tokens->getElement( 0 ) );
                
                    if( successFlag[0] != '1' ) {
                        setError( "ERROR:  Posting move failed." );
                        }
                    else {
                        // server know's we're alive
                        mTimeLastReportedAlive = time( NULL );
                        }
                    
                    }
                else {
                    resultWellFormatted = false;
                    }
                }
            else if( mCurrentType == GET_CHANGES ) {
                resultWellFormatted = false;
                
                if( tokens->size() > 2 ) {
                    
                    int numRead = sscanf( *( tokens->getElement( 0 ) ),
                                          "%lu", &mSecondsSincePartnerActed );
                    if( numRead == 1 ) {

                        delete [] mLastStateSeen;
                        
                        mLastStateSeen = 
                            stringDuplicate( *( tokens->getElement( 1 ) ) );
                
                        int numChangedColumns;
                        
                        int numRead = sscanf( *( tokens->getElement( 2 ) ),
                                              "%d", &numChangedColumns );
                    
                        if( numRead == 1 ) {
                        
                            if( tokens->size() >= 3 + numChangedColumns ) {
                            
                                resultWellFormatted = true;
                                
                                // server know's we're alive
                                mTimeLastReportedAlive = time( NULL );


                                SimpleVector<char *> dirtyColumns;
                            
                                int i;
                                
                                for( i=0; i<numChangedColumns; i++ ) {
                                
                                    char *columnString = 
                                        *( tokens->getElement( i + 3 ) );
                                

                                    if( strcmp( mPlayerNumber, "2" ) == 0 ) {
                                        // player 2 uses opposite colors on 
                                        // server
                                        flipColorCodes( columnString );
                                        }
                                
                                    char *transformedString = 
                                        transformColumnString( columnString );
                                
                                    dirtyColumns.push_back( 
                                        transformedString );
                                    }

                                mGridToUpdate->setColumns( &dirtyColumns );
                                
                                mGridToUpdate->clearDirtyFlags();

                            
                                for( i=0; i<numChangedColumns; i++ ) {
                                    delete [] 
                                        *( dirtyColumns.getElement( i ) );
                                    }
                                }
                            }
                        }                    
                    }
                }
            
            
            if( !resultWellFormatted ) {
                setError( "ERROR:  Badly formatted response from server." );
                
                printf( "Response:  %s\n", result );
                }
            
            for( int i=0; i<tokens->size(); i++ ) {
                delete [] *( tokens->getElement( i ) );
                }
            delete tokens;
            
            delete [] result;
            
            // clear current request for next step
            delete mCurrentRequest;
            mCurrentRequest = NULL;
            mCurrentType = NONE;
            
            return step();
            }
        }
    else {
        if( mQueuedRequests.size() > 0 ) {
            QueuedRequest *q = *( mQueuedRequests.getElement( 0 ) );
            mQueuedRequests.deleteElement( 0 );
            
            mCurrentType = q->mType;
            mCurrentRequest = new WebRequest( q->mMethod,
                                              q->mURL,
                                              q->mBody );
            delete q;
            
            return step();
            }
        }

    // done with all work
    return false;
    }



char Client::isClientReady() {
    // do we have a server URL yet?
    if( sServerURL != NULL ) {
        
        // do we have an ID yet?
        if(  mPlayerID != NULL ) {
            return true;
            }
        }
    return false;
    }
        
        
        
void Client::createGame() {

    char *actionString = autoSprintf( "action=create_for_friend&"
                                      "player_id=%s", 
                                      mPlayerID );

    QueuedRequest *q = new QueuedRequest( CREATE,
                                          "POST",
                                          sServerURL,
                                          actionString );
    delete [] actionString;
    
    mQueuedRequests.push_back( q );
    
    step();
    }



void Client::joinGame( char *inGameID ) {
    char *actionString;
    RequestType t;
    
    if( inGameID != NULL ) {
        actionString = autoSprintf( "action=join_with_friend&"
                                    "player_id=%s&game_id=%s", 
                                    mPlayerID,
                                    inGameID );
        t = JOIN_FRIEND;
        }
    else {
        actionString = autoSprintf( "action=join_with_stranger&"
                                    "player_id=%s", 
                                    mPlayerID );
        t = JOIN_STRANGER;
        }
    

    QueuedRequest *q = new QueuedRequest( t,
                                          "POST",
                                          sServerURL,
                                          actionString );
    delete [] actionString;
    
    mQueuedRequests.push_back( q );
    
    step();
    }



char Client::postMove( BlockGrid *inGrid ) {

    char returnVal = false;
    
    SimpleVector<char*> *dirtyColumns = inGrid->getDirtyColumns();
    

    // do nothing if there are no dirty columns to send
    int numDirty = dirtyColumns->size();
    
    if( numDirty > 0 ) {
        

        char *actionString = autoSprintf( "action=post_move&game_id=%s"
                                          "&game_passcode=%s&player_number=%s"
                                          "&num_dirty_columns=%d",
                                          mGameID, mGamePasscode, 
                                          mPlayerNumber,
                                          numDirty );
        for( int i=0; i<numDirty; i++ ) {
            
            char *columnString = *( dirtyColumns->getElement( i ) );
            
            if( strcmp( mPlayerNumber, "2" ) == 0 ) {
                // player 2 uses opposite colors on server
                flipColorCodes( columnString );
                }
        
            char *transformedString = 
                transformColumnString( columnString );
            
            delete [] columnString;
            columnString = transformedString;
        
        

            // add to actionString
            char *newActionString = autoSprintf( "%s&dirty_column_%d=%s",
                                                 actionString, i, 
                                                 columnString );
            delete [] actionString;
            
            actionString = newActionString;
            
            
            delete [] columnString;
            }

        QueuedRequest *q = new QueuedRequest( POST_MOVE,
                                              "POST",
                                              sServerURL,
                                              actionString );
        delete [] actionString;
        
        mQueuedRequests.push_back( q );
        
        step();
        returnVal = true;
        }
    
    
    
    delete dirtyColumns;

    return returnVal;
    }



void Client::getChanges( BlockGrid *inGrid ) {
    mGridToUpdate = inGrid;
    
    char *actionString = autoSprintf( "action=get_changed_columns&game_id=%s"
                                      "&game_passcode=%s&player_number=%s"
                                      "&last_state_seen=%s",
                                      mGameID, mGamePasscode, mPlayerNumber,
                                      mLastStateSeen );
    
    QueuedRequest *q = new QueuedRequest( GET_CHANGES,
                                          "POST",
                                          sServerURL,
                                          actionString );
    delete [] actionString;
    
    mQueuedRequests.push_back( q );
    
    step();    
    }



char *Client::getGameID() {
    return mGameID;
    }



unsigned long Client::getSecondsSincePartnerActed() {
    return mSecondsSincePartnerActed;
    }



char Client::isGameReady() {
    return mGameReady;
    }



int Client::getSharedWorldIndex( int mNumWorlds ) {
    if( !mGameReady ) {
        return 0;
        }
    else {
        // add all chars in ID together and mod them
        int numChars = strlen( mGameID );
        
        int sum = 0;
        for( int i=0; i<numChars; i++ ) {
            sum += mGameID[ i ];
            }
        
        return sum % mNumWorlds;
        }
    }



char Client::isError() {
    return mError;
    }



void Client::clearError() {
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }
    mErrorString = NULL;
    mError = false;
    }



char *Client::getErrorString() {
    return mErrorString;
    }



void Client::setError( const char *inErrorMessage ) {
    mError = true;
    
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }

    mErrorString = stringDuplicate( inErrorMessage );
    }



void Client::setBlockTransformFromString( char *inTransformString ) {
    
    int numParts;
    
    char **parts = split( inTransformString, ",", &numParts );
    

    for( int i=0; i<numParts; i++ ) {
        
        if( i < 16 ) {
            
            int index;
            
            int numRead = sscanf( parts[i], "%d", &index );
            
            if( numRead == 1 && index >=0 && index < 16 ) {
                mBlockTransform[i] = index;
                }
            }
        
        delete [] parts[i];
        }
    delete [] parts;
    }



char *Client::transformColumnString( char *inColumnString ) {
    
    // first part is column number... skip it
    
    int numParts;
    
    char **parts = 
        split( inColumnString, "_", &numParts );

    int i;
    
    for( i=1; i<numParts; i++ ) {
        
        if( strcmp( parts[i], "" ) != 0 ) {
        
            char *decodedBlock = Block::runLengthDecode( parts[ i ] );

            char *transformedBlock = stringDuplicate( decodedBlock );
                
        
            // our transform is 4x4
            // but our block is 16x16

            for( int y=0; y<16; y++ ) {
                // y in 4x4 transform grid
                int ty = y / 4;
                int stringOffsetY = y % 4;
            

                for( int x=0; x<16; x++ ) {
                    // x in 4x4 transform grid
                    int tx = x / 4;
                    int stringOffsetX = x % 4;

                
                    int stringIndex = y * 16 + x;
                
                    int transformIndex = ty * 4 + tx;
                

                    int jumpToTransformIndex = 
                        mBlockTransform[ transformIndex ];
                

                    int jx = jumpToTransformIndex % 4;
                    int jy = jumpToTransformIndex / 4;
                

                    int stringJumpX = jx * 4 + stringOffsetX;
                    int stringJumpY = jy * 4 + stringOffsetY;
                
                    int stringJumpIndex = stringJumpY * 16 + stringJumpX;
                
                    transformedBlock[ stringJumpIndex ] = 
                        decodedBlock[ stringIndex ];                
                    }
                }

            char *encodedBlock = Block::runLengthEncode( transformedBlock );

            delete [] transformedBlock;
            delete [] decodedBlock;
            delete [] parts[i];
        
            parts[i] = encodedBlock;
            }
        }
    
    char *returnString = join( parts, numParts, "_" );

    for( i=0; i<numParts; i++ ) {
        delete [] parts[i];
        }
    delete [] parts;
    
    return returnString;
    }


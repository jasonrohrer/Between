#ifndef CLIENT_INCLUDED
#define CLIENT_INCLUDED


#include "WebRequest.h"
#include "BlockGrid.h"


// request types:
enum RequestType { 
    NONE,
    GET_SERVER_URL,
    GET_ID,
    CREATE,
    JOIN_FRIEND,
    IS_PARTNER_READY,
    JOIN_STRANGER,
    REPORT_ALIVE,
    POST_MOVE,
    GET_CHANGES };


class QueuedRequest {
        
    public:

        QueuedRequest( RequestType inType, const char *inMethod, 
                       const char *inURL,
                       const char *inBody )
                : mType( inType ),
                  mMethod( stringDuplicate( inMethod ) ),
                  mURL( stringDuplicate( inURL ) ),
                  mBody( NULL ) {
             
            if( inBody != NULL ) {
                mBody = stringDuplicate( inBody );
                }
            }
        

        ~QueuedRequest() {
            delete [] mMethod;
            delete [] mURL;
            if( mBody != NULL ) {
                delete [] mBody;
                }
            }
        
        RequestType mType;
        
        char *mMethod;
        char *mURL;
        char *mBody;
    };



            


// encapsulates all game network client operations
class Client {
        

    public:

        // creates client and starts to request a player_id from the server, 
        // if needed
        Client();
        

        ~Client();
        

        // perform another step of any pending network operations
        // returns true if work remains to be done
        // returns false if client is done with all pending work
        char step();
        

        // must return true before starting any operations below
        char isClientReady();
        

        
        // these calls start operations
        
        void createGame();
        
        // NULL to join game with stranger
        void joinGame( char *inGameID );
        
        
        // posts dirty columns to server
        // returns true if some were dirty and a post to the server resulted
        char postMove( BlockGrid *inGrid );


        // gets changes and adds them to inGrid
        void getChanges( BlockGrid *inGrid );
        



        // result not destroyed by caller
        // NULL if not set yet
        char *getGameID();
        
        // useful for detecting a zombie or disconnected partner
        unsigned long getSecondsSincePartnerActed();
        
        

        char isGameReady();


        // gets a random, yet constant-per-game (and same as partner)
        // world index
        // game must be ready before this call works
        int getSharedWorldIndex( int mNumWorlds );
        
        
        
        char isError();
        
        void clearError();
        
        // destroyed internally
        char *getErrorString();
        

        // frees static data for this class
        // must be called at app exit
        static void staticFree();
        

    protected:
        // URL where we fetch server URL from
        char *mServerFinderURL;

        // only fetch once per application instance
        static char *sServerURL;
        
        char *mPlayerID;

        char *mGameID;
        
        char *mGamePasscode;
        
        char *mPlayerNumber;

        // self-reversing transform
        int mBlockTransform[16];
        
        // set from comma-delimited list
        void setBlockTransformFromString( char *inTransformString );
        
        char *transformColumnString( char *inColumnString );
        

        
        char mGameReady;
        
        unsigned long mTimeLastCheckedReady;
        unsigned long mTimeBetweenChecks;
        
        
        // we report our alive status when we're otherwise idle
        unsigned long mTimeLastReportedAlive;
        unsigned long mTimeBetweenReports;
        

        unsigned long mSecondsSincePartnerActed;
        
        
        char *mLastStateSeen;
        
        BlockGrid *mGridToUpdate;
        

        char mError;
        char *mErrorString;

        // inErrorMessage copied internally
        // must be destroyed by caller
        void setError( const char *inErrorMessage );
        
        


        RequestType mCurrentType;
        WebRequest *mCurrentRequest;
        
        SimpleVector<QueuedRequest*> mQueuedRequests;
        
    };



#endif

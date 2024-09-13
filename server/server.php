<?php



global $gs_version;
$gs_version = "0.3";



// edit settings.php to change server' settings
include( "settings.php" );




// no end-user settings below this point




// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );



// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>Game 7 Server Web-based setup</TITLE></HEAD>
<BODY BGCOLOR=#FFFFFF TEXT=#000000 LINK=#0000FF VLINK=#FF0000>

<CENTER>
<TABLE WIDTH=75% BORDER=0 CELLSPACING=0 CELLPADDING=1>
<TR><TD BGCOLOR=#000000>
<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 CELLPADDING=10>
<TR><TD BGCOLOR=#EEEEEE>";

$setup_footer = "
</TD></TR></TABLE>
</TD></TR></TABLE>
</CENTER>
</BODY></HTML>";





// ensure that magic quotes are on (adding slashes before quotes
// so that user-submitted data can be safely submitted in DB queries)
if( ! function_exists( "get_magic_quotes_gpc" ) || !get_magic_quotes_gpc() ) {
    // force magic quotes to be added
    $_GET     = array_map( 'gs_addslashes_deep', $_GET );
    $_POST    = array_map( 'gs_addslashes_deep', $_POST );
    $_REQUEST = array_map( 'gs_addslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'gs_addslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
gs_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly


gs_checkForFlush();



// grab POST/GET variables
$action = "";
if( isset( $_REQUEST[ "action" ] ) ) {
    $action = $_REQUEST[ "action" ];
    }

$debug = "";
if( isset( $_REQUEST[ "debug" ] ) ) {
    $debug = $_REQUEST[ "debug" ];
    }



if( $action == "version" ) {
    global $gs_version;
    echo "$gs_version";
    }
else if( $action == "show_log" ) {
    gs_showLog();
    }
else if( $action == "clear_log" ) {
    gs_clearLog();
    }
else if( $action == "get_player_id" ) {
    gs_getPlayerID();
    }
else if( $action == "create_for_friend" ) {
    gs_createForFriend();
    }
else if( $action == "join_with_friend" ) {
    gs_joinWithFriend();
    }
else if( $action == "join_with_stranger" ) {
    gs_joinWithStranger();
    }
else if( $action == "is_partner_ready" ) {
    gs_isPartnerReady();
    }
else if( $action == "report_alive" ) {
    gs_reportAlive();
    }
else if( $action == "post_move" ) {
    gs_postMove();
    }
else if( $action == "get_changed_columns" ) {
    gs_getChangedColumns();
    }
else if( $action == "gs_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Game 7 Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    gs_setupDatabase();

    echo "</TD></TR></TABLE></TD></TR></TABLE></CENTER><BR><BR>";
    
    echo $setup_footer;
    }
else if( preg_match( "/server\.php/", $_SERVER[ "SCRIPT_NAME" ] ) ) {
    // server.php has been called without an action parameter

    // the preg_match ensures that server.php was called directly and
    // not just included by another script
    
    // quick (and incomplete) test to see if we should show instructions
    global $tableNamePrefix;
    
    // check if our "games" table exists
    $tableName = $tableNamePrefix . "games";
    
    $exists = gs_doesTableExist( $tableName );
        
    if( $exists  ) {
        echo "Game 7 server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Game 7 Server Web-based Setup</H2>";
    
        echo "Game 7 Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=gs_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

gs_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function gs_setupDatabase() {
    global $tableNamePrefix;


    $tableName = $tableNamePrefix . "server_globals";
    if( ! gs_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "next_player_id CHAR(7) NOT NULL, ".
            "last_flush_time DATETIME NOT NULL ) ENGINE = INNODB;";

        $result = gs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    $tableName = $tableNamePrefix . "log";
    if( ! gs_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = gs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "games";
    if( ! gs_doesTableExist( $tableName ) ) {

        // this table contains general info about each game
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";

        $result = gs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    $tableName = $tableNamePrefix . "columns";
    if( ! gs_doesTableExist( $tableName ) ) {

        // this table contains information for each user
        $query =
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL," .
            "column_index TINYINT NOT NULL," .
            "blocks TEXT NOT NULL," .
            "changed_in_state INT NOT NULL, ".
            "PRIMARY KEY( game_id, column_index ) );";
                
        $result = gs_queryDatabase( $query );
        
        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function gs_showLog() {
    $password = "";
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];
        }

    global $logAccessPassword, $tableNamePrefix;
    
    if( $password != $logAccessPassword ) {
        echo "Incorrect password.";

        gs_log( "Failed show_log access with password:  $password" );
        }
    else {
        $query = "SELECT * FROM $tableNamePrefix"."log;";
        $result = gs_queryDatabase( $query );

        $numRows = mysql_numrows( $result );

        echo "$numRows log entries:<br><br><br>\n";
        

        for( $i=0; $i<$numRows; $i++ ) {
            $time = mysql_result( $result, $i, "entry_time" );
            $entry = mysql_result( $result, $i, "entry" );

            echo "<b>$time</b>:<br>$entry<hr>\n";
            }
        }
    }



function gs_clearLog() {
    $password = "";
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];
        }

    global $logAccessPassword, $tableNamePrefix;
    
    if( $password != $logAccessPassword ) {
        echo "Incorrect password.";

        gs_log( "Failed clear_log access with password:  $password" );
        }
    else {
        $query = "DELETE FROM $tableNamePrefix"."log;";
        $result = gs_queryDatabase( $query );

        if( $result ) {
            echo "Log cleared.";
            }
        else {
            echo "DELETE operation failed?";
            }
        }
    }




function gs_getPlayerID() {
    
    // disable autocommit so that FOR UPDATE actually works
    gs_queryDatabase( "SET AUTOCOMMIT = 0;" );

    global $tableNamePrefix;
    
    $query = "SELECT next_player_id FROM $tableNamePrefix"."server_globals ".
        "FOR UPDATE;";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    // default if none in DB yet
    // note that lucky first player gets this ID
    $id_to_return = "0000000";
    
    if( $numRows > 0 ) {
        // get the one from DB
        $id_to_return = mysql_result( $result, 0, "next_player_id" );
        }
    else {
        // no rows
        // create one
        $query = "INSERT INTO $tableNamePrefix"."server_globals VALUES ( " .
            "'$id_to_return', CURRENT_TIMESTAMP );";
        $result = gs_queryDatabase( $query );
        }

    echo "$id_to_return";
    
    
    // inser a new one by hashing this one
    $next_player_id = md5( $id_to_return );
    
    $next_player_id = strtoupper( $next_player_id );
    
    $next_player_id = substr( $next_player_id, 0, 7 );
        
    $query = "UPDATE $tableNamePrefix"."server_globals SET " .
        "next_player_id = '$next_player_id';";
    
    $result = gs_queryDatabase( $query );

    
    gs_queryDatabase( "COMMIT;" );

    gs_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }



function gs_createForFriend() {
    $player_id = "";
    if( isset( $_REQUEST[ "player_id" ] ) ) {
        $player_id = $_REQUEST[ "player_id" ];
        }

    $player_id = strtoupper( $player_id );
    
    gs_createGame( $player_id, 1 );
    }



// inPrivate is 0 or 1
function gs_createGame( $inPlayerID, $inPrivate ) {
    global $tableNamePrefix;

    $found_unused_id = 0;
    $salt = 0;
    
    
    while( ! $found_unused_id ) {

        $randVal = rand();
        
        $hash = md5( $inPlayerID . uniqid( "$randVal"."$salt", true ) );

        $hash = strtoupper( $hash );
        
        
        $game_id = substr( $hash, 0, 7 );


        
        // make code more human-friendly (alpha only)

        $digitArray =
            array( "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" );
        $letterArray =
            array( "W", "H", "J", "K", "X", "M", "N", "P", "T", "Y" );

        $game_id = str_replace( $digitArray, $letterArray, $game_id );
                
                    

        
        $game_passcode = $hash;

        $player_transforms = array( array(), array() );

        for( $i=0; $i<16; $i++ ) {
            array_push( $player_transforms[0], $i );
            array_push( $player_transforms[1], $i );
            }
        
        // do random swaps to make transforms
        for( $i=0; $i<15; $i++ ) {
            for( $t=0; $t<2; $t++ ) {
                
                if( $player_transforms[ $t ][ $i ] == $i ) {
                    // not swapped yet
                    
                    // find an unswapped partner
                    $indB = rand( $i+1, 15 );
                    
                    while( $player_transforms[$t][ $indB ] != $indB ) {
                        // value at indB already swapped
                        $indB = rand( $i+1, 15 );
                        }
            
                    $player_transforms[$t][ $i ] =
                        $player_transforms[$t][ $indB ];
                    $player_transforms[$t][ $indB ] = $i;
                    }
                }
            }

        $player_1_transform = implode( ",", $player_transforms[0] );
        $player_2_transform = implode( ",", $player_transforms[1] );
        
            
            
        /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";
        */


        $query = "INSERT INTO $tableNamePrefix". "games VALUES ( " .
            "'$game_id', '$game_passcode', ".
            "'$inPrivate', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, ".
            "'$inPlayerID', '0000000', ".
            "'$player_1_transform', '$player_2_transform', '1', '0', ".
            "CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '0' );";


        $result = mysql_query( $query );

        if( $result ) {
            $found_unused_id = 1;

            // build empty columns in table
            global $blockColumns;

            for( $c=0; $c<$blockColumns; $c++ ) {
                $query = "INSERT INTO $tableNamePrefix". "columns VALUES ( " .
                    "'$game_id', '$c', '', 0 );";
                
                gs_queryDatabase( $query );
                }

            
            $gameTypeString = "Stranger";
            if( $inPrivate ) {
                $gameTypeString = "Friend";
                }
            
            gs_log( "$gameTypeString game $game_id created by ".
                    "player $inPlayerID" );

            
            echo "$game_id\n";
            echo "$game_passcode\n";
            echo "1\n";
            echo "$player_1_transform";
            
            }
        else {
            global $debug;
            if( $debug == 1 ) {
                echo "Duplicate ids?  Error:  " . mysql_error() ."<br>";
                }
            // try again
            $salt += 1;
            }
        }

    }



function gs_joinWithFriend() {
    $game_id = "";
    if( isset( $_REQUEST[ "game_id" ] ) ) {
        $game_id = $_REQUEST[ "game_id" ];
        }

    $game_id = strtoupper( $game_id );
    

    $player_id = "";
    if( isset( $_REQUEST[ "player_id" ] ) ) {
        $player_id = $_REQUEST[ "player_id" ];
        }

    $player_id = strtoupper( $player_id );

    
    global $tableNamePrefix;

    

    /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";
    */

    
    $query = "SELECT * FROM $tableNamePrefix"."games ".
        "WHERE game_id = '$game_id' AND player_1_id != '$player_id' ".
        "AND player_2_ready = '0';";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );
        
        $game_passcode = $row[ "game_passcode" ];
        $player_2_transform = $row[ "player_2_transform" ];
        
        
        $query = "UPDATE $tableNamePrefix"."games SET " .
            "touch_date = CURRENT_TIMESTAMP, " .
            "player_2_id = '$player_id', " .
            "player_2_ready = '1', " .
            "player_2_touch_date = CURRENT_TIMESTAMP " .
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
        
        $result = gs_queryDatabase( $query );

        gs_log( "Friend game $game_id joined by player $player_id" );

        
        echo "$game_id\n";
        echo "$game_passcode\n";
        echo "2\n";
        echo "$player_2_transform";
        }
    else {
        gs_operationError(
            "Could not find joinable game matching  " . $game_id );
        }    
    }



function gs_joinWithStranger() {

    $player_id = "";
    if( isset( $_REQUEST[ "player_id" ] ) ) {
        $player_id = $_REQUEST[ "player_id" ];
        }

    $player_id = strtoupper( $player_id );

    
    
    // first, look for non-private games that aren't full

    // sort by change date (prefer ones that have been checked by waiting
    // opponent most recently)
    
    // ignore those that haven't been checked for 20 seconds or more
    // (abandoned?)
    
    // disable autocommit so that FOR UPDATE actually works
    gs_queryDatabase( "SET AUTOCOMMIT = 0;" );

    global $tableNamePrefix;
    
    $query = "SELECT * FROM $tableNamePrefix"."games ".
        "WHERE private = '0' AND player_2_ready = '0' ".
        "AND player_1_id != '$player_id' ".
        "AND touch_date > SUBTIME( CURRENT_TIMESTAMP, '0 0:0:20.00' ) ".
        "ORDER BY touch_date DESC ".
        "FOR UPDATE;";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows > 0 ) {
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $game_id = $row[ "game_id" ];
        $game_passcode = $row[ "game_passcode" ];
        $player_2_transform = $row[ "player_2_transform" ];
        
        
        $query = "UPDATE $tableNamePrefix"."games SET " .
            "touch_date = CURRENT_TIMESTAMP, " .
            "player_2_id = '$player_id', " .
            "player_2_ready = '1', " .
            "player_2_touch_date = CURRENT_TIMESTAMP " .
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
        
        $result = gs_queryDatabase( $query );

        gs_log( "Stranger game $game_id joined by player $player_id" );
        
        
        echo "$game_id\n";
        echo "$game_passcode\n";
        echo "2\n";
        echo "$player_2_transform";
        }
    else {
        // no empty games found
        
        // create a new public game
        gs_createGame( $player_id, 0 );
        }

    gs_queryDatabase( "COMMIT;" );

    gs_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }



function gs_isPartnerReady() {
    $game_id = "";
    if( isset( $_REQUEST[ "game_id" ] ) ) {
        $game_id = $_REQUEST[ "game_id" ];
        }
    $game_passcode = "";
    if( isset( $_REQUEST[ "game_passcode" ] ) ) {
        $game_passcode = $_REQUEST[ "game_passcode" ];
        }
    $player_number = "";
    if( isset( $_REQUEST[ "player_number" ] ) ) {
        $player_number = $_REQUEST[ "player_number" ];
        }

    $game_id = strtoupper( $game_id );
    $game_passcode = strtoupper( $game_passcode );
    
    global $tableNamePrefix;

    

    /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";
    */

    
    $query = "SELECT * FROM $tableNamePrefix"."games ".
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $otherReady = 0;
        
        if( $player_number == 1 ) {
            $otherReady = $row[ "player_2_ready" ];
            }
        else {
            $otherReady = $row[ "player_1_ready" ];
            }

        echo "$otherReady";

        // touched, update timestamp
        $query = "UPDATE $tableNamePrefix"."games SET " .
            "touch_date = CURRENT_TIMESTAMP " .
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
        
        $result = gs_queryDatabase( $query );
        }
    else {
        gs_operationError( "No game matched ID and passcode" );
        }

    /*
?action=is_partner_ready&game_id=0a4d516&game_passcode=0a4d516e820579bb030f3eb66d0f3035&player_number=1
     */

    }



function gs_reportAlive() {
    $game_id = "";
    if( isset( $_REQUEST[ "game_id" ] ) ) {
        $game_id = $_REQUEST[ "game_id" ];
        }
    $game_passcode = "";
    if( isset( $_REQUEST[ "game_passcode" ] ) ) {
        $game_passcode = $_REQUEST[ "game_passcode" ];
        }
    $player_number = "";
    if( isset( $_REQUEST[ "player_number" ] ) ) {
        $player_number = $_REQUEST[ "player_number" ];
        }

    $playerTouchDateName = "player_1_touch_date";
    if( $player_number == "2" ) {
        $playerTouchDateName = "player_2_touch_date";
        }

    global $tableNamePrefix;
    
    $query = "UPDATE $tableNamePrefix"."games SET " .
        "touch_date = CURRENT_TIMESTAMP, " .
        "$playerTouchDateName = CURRENT_TIMESTAMP " .
        "WHERE game_id = '$game_id' ".
        "AND game_passcode = '$game_passcode';";

    $result = gs_queryDatabase( $query );

    echo "1";
    }



function gs_postMove() {
    $game_id = "";
    if( isset( $_REQUEST[ "game_id" ] ) ) {
        $game_id = $_REQUEST[ "game_id" ];
        }
    $game_passcode = "";
    if( isset( $_REQUEST[ "game_passcode" ] ) ) {
        $game_passcode = $_REQUEST[ "game_passcode" ];
        }
    $player_number = "";
    if( isset( $_REQUEST[ "player_number" ] ) ) {
        $player_number = $_REQUEST[ "player_number" ];
        }
    $num_dirty_columns = "";
    if( isset( $_REQUEST[ "num_dirty_columns" ] ) ) {
        $num_dirty_columns = $_REQUEST[ "num_dirty_columns" ];
        }

    $game_id = strtoupper( $game_id );
    $game_passcode = strtoupper( $game_passcode );

    
    global $tableNamePrefix;

    
    /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";
    */

    
    // disable autocommit so that FOR UPDATE actually works
    gs_queryDatabase( "SET AUTOCOMMIT = 0;" );
    
    
    $query = "SELECT * FROM $tableNamePrefix"."games ".
        "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode' ".
        "FOR UPDATE;";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );
        
        $state_number = $row[ "state_number" ];

        // new state
        $state_number ++;
        
        
        for( $c=0; $c<$num_dirty_columns; $c++ ) {
            $paramName = "dirty_column_$c";

            $columnData = "";
            
            if( isset( $_REQUEST[ $paramName ] ) ) {
                $columnData = $_REQUEST[ $paramName ];
                }
            else {
                gs_operationError( "Column missing from input" );
                }
            //echo "column data = $columnData<br>";
            
            $dataParts = explode( "_", $columnData );

            /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL," .
            "column_index TINYINT NOT NULL," .
            "blocks TEXT NOT NULL," .
            "changed_in_state INT NOT NULL, ".
            "PRIMARY KEY( game_id, column_index ) );";
            */
            
            $column_index = array_shift( $dataParts );

            if( $player_number == 2 ) {
                // flip left-to-right
                global $blockColumns;
                
                $column_index = $blockColumns - 1 - $column_index;
                }
            
            
            $numParts = count( $dataParts );
            
            //echo "numParts = $numParts<br>";
            
            // put remaining blocks back together
            $blocks = implode( "_", $dataParts );

            //echo "blocks = $blocks<br>";
            
            $query = "UPDATE $tableNamePrefix"."columns SET " .
                "blocks = '$blocks', " .
                "changed_in_state = '$state_number' " .
                "WHERE game_id = '$game_id' AND ".
                "column_index = '$column_index';";
            $result = gs_queryDatabase( $query );            
            }

        $playerTouchDateName = "player_1_touch_date";
        if( $player_number == "2" ) {
            $playerTouchDateName = "player_2_touch_date";
            }
        

        $query = "UPDATE $tableNamePrefix"."games SET " .
            "touch_date = CURRENT_TIMESTAMP, " .
            "$playerTouchDateName = CURRENT_TIMESTAMP, " .
            "state_number = '$state_number' " .
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
        
        $result = gs_queryDatabase( $query );


        echo "1";
        }
    else {
        gs_operationError( "No game matched ID and passcode" );
        }

    /*
http://localhost/jcr13/game7/server.php?action=post_move&game_id=fac4e7c&game_passcode=fac4e7c03cf367f954ace1a3480966fe&player_number=1&num_dirty_columns=1&dirty_column_0=5_16A
     */

    // unlock rows that were locked by FOR UPDATE above
    gs_queryDatabase( "COMMIT;" );
    
    gs_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }



function gs_getChangedColumns() {
    $game_id = "";
    if( isset( $_REQUEST[ "game_id" ] ) ) {
        $game_id = $_REQUEST[ "game_id" ];
        }
    $game_passcode = "";
    if( isset( $_REQUEST[ "game_passcode" ] ) ) {
        $game_passcode = $_REQUEST[ "game_passcode" ];
        }
    $player_number = "";
    if( isset( $_REQUEST[ "player_number" ] ) ) {
        $player_number = $_REQUEST[ "player_number" ];
        }
    $last_state_seen = "";
    if( isset( $_REQUEST[ "last_state_seen" ] ) ) {
        $last_state_seen = $_REQUEST[ "last_state_seen" ];
        }

    $game_id = strtoupper( $game_id );
    $game_passcode = strtoupper( $game_passcode );

    
    global $tableNamePrefix;

    
    /*
            "CREATE TABLE $tableName(" .
            "game_id CHAR(7) NOT NULL PRIMARY KEY," .
            "game_passcode CHAR(32) NOT NULL," .
            "private TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "touch_date DATETIME NOT NULL," .
            "player_1_id CHAR(7) NOT NULL," .
            "player_2_id CHAR(7) NOT NULL," .
            "player_1_transform CHAR(37) NOT NULL," .
            "player_2_transform CHAR(37) NOT NULL," .
            "player_1_ready TINYINT NOT NULL," .
            "player_2_ready TINYINT NOT NULL," .
            "player_1_touch_date DATETIME NOT NULL," .
            "player_2_touch_date DATETIME NOT NULL," .
            "state_number INT NOT NULL ) ENGINE = INNODB;";
    */

    
    $query = "SELECT * FROM $tableNamePrefix"."games ".
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $state_number = $row[ "state_number" ];


        $partnerTouchDate = $row[ "player_1_touch_date" ];
        if( $player_number == 1 ) {
            $partnerTouchDate = $row[ "player_2_touch_date" ];
            }

        // compute seconds since partner touched
        $query = "SELECT TIME_TO_SEC( TIMEDIFF( CURRENT_TIMESTAMP, ".
                                              " '$partnerTouchDate' ) );";
        

        $result = gs_queryDatabase( $query );
        
        $secondsSincePartnerActed = mysql_result( $result, 0, 0 );
        

        

        $query = "SELECT * FROM $tableNamePrefix"."columns ".
            "WHERE game_id = '$game_id' AND ".
            "changed_in_state > $last_state_seen;";

        $result = gs_queryDatabase( $query );

        $numChangedColumns = mysql_numrows( $result );

        echo "$secondsSincePartnerActed\n";
        echo "$state_number\n";
        echo "$numChangedColumns\n";
        
        
        for( $i=0; $i<$numChangedColumns; $i++ ) {
            
            $blocks = mysql_result( $result, $i, "blocks" );
            $column_index = mysql_result( $result, $i, "column_index" );

            if( $player_number == 2 ) {
                // flip left-to-right
                global $blockColumns;
                
                $column_index = $blockColumns - 1 - $column_index;
                }
            
            echo "$column_index" . "_" . "$blocks\n";
            }

        
        $playerTouchDateName = "player_1_touch_date";
        if( $player_number == "2" ) {
            $playerTouchDateName = "player_2_touch_date";
            }

        

        $query = "UPDATE $tableNamePrefix"."games SET " .
            "touch_date = CURRENT_TIMESTAMP, " .
            "$playerTouchDateName = CURRENT_TIMESTAMP " .
            "WHERE game_id = '$game_id' AND game_passcode = '$game_passcode';";
        gs_queryDatabase( $query );
        }
    else {
        gs_operationError( "No game matched ID and passcode" );
        }

    /*
?action=get_changed_columns&game_id=0e86e62&game_passcode=0e86e6239c67bcc56b4b6955a8fd501c&player_number=1&last_state_seen=1
     */

    }



// check if we should flush stale games from the database
// do this every 5 minutes
function gs_checkForFlush() {
    global $tableNamePrefix;

    $tableName = "$tableNamePrefix"."server_globals";
    
    if( !gs_doesTableExist( $tableName ) ) {
        return;
        }
    
    
    gs_queryDatabase( "SET AUTOCOMMIT = 0;" );

    
    $query = "SELECT last_flush_time FROM $tableName ".
        "WHERE last_flush_time < SUBTIME( CURRENT_TIMESTAMP, '0 0:05:0.000' ) ".
        "FOR UPDATE;";

    $result = gs_queryDatabase( $query );

    if( mysql_numrows( $result ) > 0 ) {

        // last flush time is old


    
        global $tableNamePrefix;

        $gamesTable = $tableNamePrefix . "games";
        $columnsTable = $tableNamePrefix . "columns";
    

        // Games that haven't been fully joined yet
        //   need to be checked by the creator (player 1) every 5 seconds.
        // If they linger 20 seconds without being checked, they are ignored
        //   by strangers looking to join.
        // If they linger 40 seconds without being checked, we drop them.
        $query = "DELETE $gamesTable, $columnsTable ".
            "FROM $gamesTable JOIN $columnsTable ".
            "WHERE player_2_ready = '0' ".
            "AND $gamesTable.touch_date < ".
            "    SUBTIME( CURRENT_TIMESTAMP, '0 0:00:40.00' ) ".
            "AND $gamesTable.game_id = $columnsTable.game_id;";
    

        $result = gs_queryDatabase( $query );

        $numRowsRemoved = mysql_affected_rows();

        gs_log( "Flush operation on unstarted games removed $numRowsRemoved".
                " rows." );
        

        // games that have been joined / started can linger for a long
        // time with no server action (while players are thinking, etc).

        // also, players can quit playing and resume playing later
        // (hmmm... do we want to support this?)

        // for now, give them 2 hours of idle time

        $query = "DELETE $gamesTable, $columnsTable ".
            "FROM $gamesTable JOIN $columnsTable ".
            "WHERE player_2_ready = '1' ".
            "AND $gamesTable.touch_date < ".
            "    SUBTIME( CURRENT_TIMESTAMP, '0 2:00:0.00' ) ".
            "AND $gamesTable.game_id = $columnsTable.game_id;";
    

        $result = gs_queryDatabase( $query );


        $numRowsRemoved = mysql_affected_rows();

        gs_log( "Flush operation on started games removed $numRowsRemoved".
                " rows." );

        global $enableLog;
        
        if( $enableLog ) {
            // count remaining games for log
            $query = "SELECT COUNT(*) FROM $gamesTable;";

            $result = gs_queryDatabase( $query );

            $count = mysql_result( $result, 0, 0 );

            gs_log( "After flush, $count games left." );
                        }
        
        // set new flush time

        $query = "UPDATE $tableName SET " .
            "last_flush_time = CURRENT_TIMESTAMP;";
    
        $result = gs_queryDatabase( $query );

    
        }

    gs_queryDatabase( "COMMIT;" );

    gs_queryDatabase( "SET AUTOCOMMIT = 1;" );
    }






// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function gs_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName;
    
    
    mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
        or gs_fatalError( "Could not connect to database server: " .
                       mysql_error() );
    
	mysql_select_db( $databaseName )
        or gs_fatalError( "Could not select $databaseName database: " .
                       mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function gs_closeDatabase() {
    mysql_close();
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function gs_queryDatabase( $inQueryString ) {

    $result = mysql_query( $inQueryString )
        or gs_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                       mysql_error() );

    return $result;
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function gs_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = gs_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function gs_log( $message ) {
    global $enableLog, $tableNamePrefix;

    $slashedMessage = addslashes( $message );
    
    if( $enableLog ) {
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = gs_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function gs_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    gs_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function gs_operationError( $message ) {
    
    // for now, just print error message
    echo( "ERROR:  $message" );
    die();
    }


/**
 * Recursively applies the addslashes function to arrays of arrays.
 * This effectively forces magic_quote escaping behavior, eliminating
 * a slew of possible database security issues. 
 *
 * @inValue the value or array to addslashes to.
 *
 * @return the value or array with slashes added.
 */
function gs_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'gs_addslashes_deep', $inValue )
          : addslashes( $inValue ) );
    }



/**
 * Recursively applies the stripslashes function to arrays of arrays.
 * This effectively disables magic_quote escaping behavior. 
 *
 * @inValue the value or array to stripslashes from.
 *
 * @return the value or array with slashes removed.
 */
function gs_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'sb_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }

?>

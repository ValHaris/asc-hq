/** file: ASC_PBEM.h
   * author: Jade Rogalski
   * license: GPL
   *  warranty: none
   *
   * interface between the game asc by Marting Bickel,
   *  and the asc pbem server by Jade Rogalski
   *
   *
   * example usages:
   *
   *  *** account erstellen
   *  
   *  ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
   *  if( asc_pbem.isUsable() && asc_pbem.createAccount( "user", "password", "a@b.c" ) )
   *  {
   *     std::cout << "account erstellt, mail unterwegs\n";
   *  }else
   *  {
   *     std::cout << "account erstellung fehlgeschlagen\n";
   *  }
   *
   *
   * *** upload eines spielstandes;
   *  
   *  char* daten = "miau";
   *  int size = strlen( "miau" ) * sizeof( char );
   *  ASCString fileName = "duell-A-1.ascpbm";
   *  int gameID = 1;
   *  
   *  ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
   *  if( asc_pbem.isUsable() && asc_pbem.login( "user", "pw" ) )
   *  {
   *     if( asc_pbem.uploadFile( fileName, daten, size, gameID ) )
   *        std::cout << "uploadFile erfolgreich\n";
   *     else
   *        std::cout << "uploadFile fehlgeschlagen\n";
   *  }
   *  
   *  ??? TODO:
   *     - main funktion rausschmeissen
   *     - einbauen in asc
   */


#ifndef __ASC_PBEM_H
#define __ASC_PBEM_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <vector>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "../ascstring.h"

class ASC_PBEM_FileUploadControl // just a better struct so we can use const data
{
   public:
      int step;
      int sent_step;
      const int size;
      const char* data;
      ASCString fileName;
      ASCString boundary;
      
      std::vector<ASCString> parameters;
      
      ASC_PBEM_FileUploadControl( const char* dataParam, const int sizeParam ): data( dataParam ), size( sizeParam ){};
      
};

 struct TUserData
{
   ASCString userName;
   int userID;
};

 struct TFileData
{
   ASCString fileName;
   char* fileData;
   int fileSize;
};

 struct TGameInfo
{
   int gameID;
   ASCString name;
   int projectID;
   int currentSlot;
   int turn;
   ASCString currentSaveGameName;
   ASCString lastActive;
   ASCString currentPlayerName;
};

class ASC_PBEM
{
   private:
      ASCString serverBase;
      bool usable;
      bool loggedIn;
      CURL *curl_handle;
      
      std::vector< ASCString > header;
      std::vector< ASCString > body;
      int statusCode;
      
      ASCString sessionID;

      // server data
      int serverVersion;
      
      // serverVersion has to be greater or equal to this
      static const int MIN_SERVER_VERSION = 1; 

      // serverVersion has to be less than this; 
      // so, MAX_SERVER_VERSION = 0.02 means that _starting_ with version 0.2, this client wont work
      static const int MAX_SERVER_VERSION = 2; 
      
   public:
   
      static const char ROLE_SUPERVISOR = 's';
      static const char ROLE_PLAYER = 'p';
      static const char ROLE_HIDDEN = 'h';
   
      /** default constructor:
         * @param serverBase: default url syntax,
         *     example: http://localhost:8080/ascServer/
         *     note: currently, the trailing slash is _required_
         *     ??? todo: fix that ;)
         */
      ASC_PBEM( ASCString serverBase );
      ~ASC_PBEM();
      
      /** important: check this first after instantiation this class
         * will be false after any @see request where anything went wrong
         *  if false, no request will be send out again, and this instance 
         *  is basically done for
         */
      bool isUsable();
      
      /** @return true if login to pbem server procceded ok,
         *  false if anything went wrong (most likely wrong user/password,
         *  but could also be wrong server base or inactive account
         */
      bool login( ASCString user, ASCString passwd );
      
      /** @return true if the account could be created
         *     false if anything went wrong, which is most likely due to one of these reasons:
         *     - wrong server
         *     - username already taken
         *     - invalid characters for either username or password
         */
      bool createAccount( ASCString user, ASCString passwd, ASCString email );
      
      
      /** @return true if the account could be activated
         *     false if anything went wrong, which is most likely due to one of these reasons:
         *     - wrong server
         *     - wrong code
         */
      bool activateAccount( ASCString user, ASCString code );

      
      /** @return true if upload went ok, false on failure
         */
      bool uploadFile( ASCString fileName, const char* data, const int size, const int gameID );

      /** @return a vector of TUserData structs containing the ascpbem server accounts
         * @param activeOnly if true, only return data about active (activated) players
         */
      std::vector<TUserData> getUserList( bool activeOnly = true );
      
      /** @return true if game creation went ok, false on failure
         */
      bool createGame( ASCString fileName, const char* data, const int size, ASCString gameName, ASCString fileNamePattern, char* roles, int* players, int projectID = -1, int turn = 1, int currentSlot = 1);
      
      /** @return a vector of TGameInfo structs containing the current games of the logged in player
         * @param myTurnOnly if true, only return info about games where it's the turn of the currenty logged in player
         */
      std::vector<TGameInfo> getCurrentGamesInfo( bool myTurnOnly );
      
      /** @return the file data of a ascpbm game, or NULL if not logged in or an error occured
         */
      TFileData* downloadGame( TGameInfo game );
      
      /** @return true if logout went ok, false on failure
         * failure most likely means one of two things:
         *  - wrong server url
         * - not logged in (session timeout?)
         */
      bool logout();
      
      // guess what ;)
      bool isLoggedIn();
      
      // get _http_ header from last request
      ASCString getHeader();
      
      // get _http_ body from last request
      ASCString getBody();
      
      // get http status code from last request, 
      // as determined by checking of the header
      // may be -1 if the last request header wasn't parsed,
      // or no status code could be determined in the header
      int getStatusCode();

   protected:
   
   private:
      
      // simple wrapper for bool request( ASCString url, std::vector<ASCString> parameters );
      bool request( ASCString url );
      
      /** @param parameters even amount of parameters _required_, in the format [0]paramName [1]value [2]paramName [3]value ...
         *     may contain 0 entries
         *
         *  @param url: the _relative_ url to load (will be prefixed with the serverbase internally)
         */
      bool request( ASCString url, std::vector<ASCString> parameters );
      
      // similar to above, but for file uploads
      bool request( ASCString url, ASC_PBEM_FileUploadControl* fileUploadControl );
      
      // breaks down the string into lines
      void parseHeader( ASCString headerString );
      
      // parse a singular header line
      void parseHeaderLine( ASCString line );
      
      // parse a singular game info line
      TGameInfo parseGameInfoLine( ASCString line );

      // parse a singular game info line
      TUserData parseUserInfoLine( ASCString line );
      
      
};

#endif

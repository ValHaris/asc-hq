

#include "pbem-server-interaction.h"



/** file: ASC_PBEM.cpp
   * author: Jade Rogalski
   * license: GPL
   *  warranty: none
   *
   * interface between the game asc by Marting Bickel,
   *  and the asc pbem server by Jade Rogalski
   */





size_t ASC_PBEM_writeInternal( void* inboundData, size_t size, size_t nmemb, void *vector )
{
   if( size != sizeof( char ) ) return 0;
   char* buffer = new char[ nmemb+1 ];
   memset( buffer, '\0', nmemb+1 );
   for( int i=0; i<nmemb; i++ )
   {
      buffer[ i ] = ((char*) inboundData )[ i ];
   }
   ( (std::vector<ASCString> *) vector )->push_back( buffer );
   delete buffer;
   return nmemb;
}


int ASC_PBEM_writeBuffer( char* buffer, const char* source, int startPos, int bufferSize, int sourceSize )
{
   int i=0;
   for( ; i<bufferSize && i<(sourceSize-startPos); i++ )
   {
      buffer[ i ] = source[ startPos + i ];
      //std::cout << buffer[ i ];
   }
   
   return i;
}

size_t ASC_PBEM_readInternal( char* outboundBuffer, size_t size, size_t nitems, void *infoStruct )
{
   ASC_PBEM_FileUploadControl * control = (ASC_PBEM_FileUploadControl*) infoStruct;
   
   switch( control->step )
   {
      case 0: // header of first part
      {
         ASCString header = ASCString("--") + control->boundary + "\n" +
            "Content-Disposition: form-data; name=\"file\"; filename=\"" +
            control->fileName + "\"\n" +
            "Content-Type: application/octet-stream\n \n";
         // end of header has to be a \n<space>\n for the server to reliably identify it as empty line
         int sent = ASC_PBEM_writeBuffer( outboundBuffer, header.c_str(), control->sent_step, size*nitems, header.size() );
         if( sent+control->sent_step == header.size() )
         {
            control->sent_step = 0;
            control->step++;
         }else
         {
            control->sent_step += sent;
         }
         
         return sent;
      }
      
      case 1: // first part
      {
         int sent = ASC_PBEM_writeBuffer( outboundBuffer, control->data, control->sent_step, size*nitems, control->size );
         if( sent+control->sent_step == control->size )
         {
            control->sent_step = 0;
            control->step++;
         }else
         {
            control->sent_step += sent;
         }
         
         return sent;
      }
      
      case 2: // footer: "--" + boundary + "--"
      {
         // keine ahnung warum da 2x\n nÃ¶tig sind... wird aber sonst nicht korrekt geparsed
         ASCString header = ASCString("\n\n--") + control->boundary;
         if( control->parameters.size() == 0 ) 
            header += "--\n";
         else
            header += "\n";
            
         int sent = ASC_PBEM_writeBuffer( outboundBuffer, header.c_str(), control->sent_step, size*nitems, header.size() );
         if( sent+control->sent_step == header.size() )
         {
            control->sent_step = 0;
            control->step++;
         }else
         {
            control->sent_step += sent;
         }
         
         return sent;
      }
      default: // footer: "--" + boundary + "--"
      {
         // parameters
         int parameterIndex = (control->step - 3) * 2;
         if( control->parameters.size() > parameterIndex )
         {
            ASCString name = control->parameters[ parameterIndex ];
            ASCString value = control->parameters[ parameterIndex + 1 ];

            ASCString data = ASCString("Content-Disposition: form-data; name=\"") + name + "\"\n\n" + value +
               "\n\n--" + control->boundary;
               
            if( control->parameters.size() > parameterIndex + 2 )
            {
               data += "\n";
            }else
            {
               data += "--\n";
            }

            int sent = ASC_PBEM_writeBuffer( outboundBuffer, data.c_str(), control->sent_step, size*nitems, data.size() );
            if( sent+control->sent_step == data.size() )
            {
               control->sent_step = 0;
               control->step++;
            }else
            {
               control->sent_step += sent;
            }
            
            return sent;
         }else
         {
            return 0;
         }
      }
   }
   return 0;
}


ASC_PBEM::ASC_PBEM( ASCString serverBase )
{
   this->serverBase = serverBase;
   if( serverBase[ serverBase.size() - 1 ] != '/' )
   {
      this->serverBase = serverBase + "/";
   }
   curl_handle = curl_easy_init();
   
   usable = true;
   statusCode = -1;
   loggedIn = false;
   serverVersion = 0;

   request( "" );
   
   usable = false;
   
   if( 
      statusCode == 200 && 
      serverVersion >= MIN_SERVER_VERSION && 
      serverVersion < MAX_SERVER_VERSION
   )
   {
      usable = true;
   }
   
}


ASC_PBEM::~ASC_PBEM()
{
   curl_easy_cleanup( curl_handle );
}

bool ASC_PBEM::isUsable() 
{
   return usable;
}

bool ASC_PBEM::isLoggedIn() 
{
   return loggedIn;
}

bool ASC_PBEM::request( ASCString url )
{
   std::vector<ASCString> parameters;
   return request( url, parameters );
}

bool ASC_PBEM::request( ASCString url, std::vector<ASCString> parameters )
{
   if( ! usable ) return false;
   
   statusCode = -1;
   header.clear();
   body.clear();
   curl_easy_reset( curl_handle );
   ASCString urlInternal = serverBase + url;
   curl_easy_setopt( curl_handle, CURLOPT_URL, urlInternal.c_str() );
   curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &ASC_PBEM_writeInternal );
  curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header );
  curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &body );

  // post parameters
   // struct curl_httppost* post = NULL;
   // struct curl_httppost* last = NULL;

   ASCString encodedRequest; // lifetime considerations require this parameter to be _outside_ the loop
   if( parameters.size() > 0 )
   {
      for( int i=0; i<parameters.size(); i++ ) 
      {
         ASCString name = parameters[ i ];
         i++;
         ASCString value = parameters[ i ];
         if( encodedRequest.length() > 0 ) encodedRequest += "&"; 
         encodedRequest += name + "=" + value;
      }
      curl_easy_setopt( curl_handle, CURLOPT_POSTFIELDS, encodedRequest.c_str() );
   }
   
  // additional headers; currently only sessionid
   struct curl_slist *headers=NULL;
   ASCString cookieString = "cookie: JSESSIONID=" + sessionID + ";"; // lifetime considerations require this parameter to be _outside_ the loop
   ASCString acceptString = "Accept: text/plain"; // lifetime considerations require this parameter to be _outside_ the loop
   if( sessionID.length() > 0 )
   {
      headers = curl_slist_append( headers, cookieString.c_str() );  
   }
   headers = curl_slist_append( headers, acceptString.c_str() );  
   curl_easy_setopt( curl_handle, CURLOPT_HTTPHEADER, headers );
   
   // actual perform the request
   usable = curl_easy_perform( curl_handle ) == 0;
   
   // parse header for status code and session id
   if( usable )
   {
      ASCString headerString;
      for( int i=0; i<header.size(); i++ ) headerString += header[ i ];
      parseHeader( headerString );
   }
   
   // clean up
   curl_slist_free_all( headers );

   return usable;
}


bool ASC_PBEM::request( ASCString url, ASC_PBEM_FileUploadControl *fileUploadControl )
{
   if( ! usable ) return false;
   
   statusCode = -1;
   header.clear();
   body.clear();
   curl_easy_reset( curl_handle );
   ASCString urlInternal = serverBase + url;
   curl_easy_setopt( curl_handle, CURLOPT_URL, urlInternal.c_str() );
   curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &ASC_PBEM_writeInternal );
  curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header );
  curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &body );

   curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, &ASC_PBEM_readInternal);
   curl_easy_setopt(curl_handle, CURLOPT_INFILE, fileUploadControl);
   curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, true );

  // additional headers; sessionid + content type
   struct curl_slist *headers=NULL;
   ASCString cookieString = "cookie: JSESSIONID=" + sessionID + ";"; // lifetime considerations require this parameter to be _outside_ the loop
   ASCString contentString = "content-type: multipart/form-data; boundary=" + fileUploadControl->boundary;
   ASCString acceptString = "Accept: text/plain"; // lifetime considerations require this parameter to be _outside_ the loop
   if( sessionID.length() > 0 )
   {
      headers = curl_slist_append( headers, cookieString.c_str() );  
   }
   headers = curl_slist_append( headers, contentString.c_str() );  
   headers = curl_slist_append( headers, acceptString.c_str() );  
   curl_easy_setopt( curl_handle, CURLOPT_HTTPHEADER, headers );
   
   // actual perform the request
   usable = curl_easy_perform( curl_handle ) == 0;
   
   // parse header for status code and session id
   if( usable )
   {
      ASCString headerString;
      for( int i=0; i<header.size(); i++ ) headerString += header[ i ];
      parseHeader( headerString );
   }
   
   // clean up
   curl_slist_free_all( headers );

   return usable;
}

void ASC_PBEM::parseHeader( ASCString headerString )
{
   int currentIndex = 0;
   while( currentIndex < headerString.size() )
   {
      int lineEnd = headerString.find( '\n', currentIndex );
      ASCString line;
      if( lineEnd != ASCString::npos )
      {
         line = headerString.substr( currentIndex, lineEnd-currentIndex );
         parseHeaderLine( line );
      }else
      {
         line = headerString.substr( currentIndex );
         parseHeaderLine( line );
         break;
      }
      currentIndex = lineEnd + 1;
   }
}

void ASC_PBEM::parseHeaderLine( ASCString line )
{
   if( line.find( "HTTP" ) == 0 )
   {
      // http status line... grab code
      int space = line.find( " " );
      if( space == ASCString::npos ) return;
      space++;
      int end = line.find( " ", space );
      if( end == ASCString::npos ) return;
      
      ASCString codeStr = line.substr( space, end-space );
      statusCode = atoi( codeStr.c_str() );
   }else if( line.find( "Set-Cookie:" ) == 0 )
   {
      if( line.find( "JSESSIONID" ) != ASCString::npos )
      {
         ASCString sessionID = line.substr( line.find( "JSESSIONID" ) );
         if( sessionID.find( "=" ) == ASCString::npos ) return;
         sessionID = sessionID.substr( sessionID.find( "=" ) + 1 );
         if( sessionID.find( ";" ) == ASCString::npos ) return;
         sessionID = sessionID.substr( 0, sessionID.find( ";" ) );
         this->sessionID = sessionID;
      }
   }else if( line.find( "X-Servlet:" ) == 0 )
   {
      if( line.find( "ASC_PBEM_Server/" ) != ASCString::npos )
      {
         serverVersion = atoi( line.substr( line.find( "ASC_PBEM_Server/" ) + 16 ).c_str() );
      }else
      {
         serverVersion = -1;
      }
   }
}

bool ASC_PBEM::logout()
{
   // don't check for isLoggedIn() on logout... if for any bug
   // this flag isn't set, we should still be able to invalidate 
   // this session
   
   // whatever happens, clean up the flag
   loggedIn = false;
   
   if( ! request( "tools/logout" ) ) return false;
   
   return statusCode == 200;
}

bool ASC_PBEM::login( ASCString user, ASCString passwd )
{
   if( loggedIn ) return false;
   
   std::vector< ASCString > parameters;
   parameters.push_back( "name" );
   parameters.push_back( user );
   parameters.push_back( "passwd" );
   parameters.push_back( passwd );
   
   if( ! request( "tools/login", parameters ) ) return false;
   
   loggedIn = statusCode == 202;
   
   return loggedIn;
}

bool ASC_PBEM::createAccount( ASCString user, ASCString passwd, ASCString email )
{
   if( loggedIn ) return false;
   
   std::vector< ASCString > parameters;
   parameters.push_back( "name" );
   parameters.push_back( user );
   parameters.push_back( "passwd" );
   parameters.push_back( passwd );
   parameters.push_back( "passwd2" );
   parameters.push_back( passwd );
   parameters.push_back( "email" );
   parameters.push_back( email );
   
   if( ! request( "tools/create", parameters ) ) return false;
   
   return statusCode == 201;
}

bool ASC_PBEM::activateAccount( ASCString user, ASCString code )
{
   if( loggedIn ) return false;
   
   std::vector< ASCString > parameters;
   parameters.push_back( "name" );
   parameters.push_back( user );
   parameters.push_back( "code" );
   parameters.push_back( code );
   
   if( ! request( "tools/activate", parameters ) ) return false;
   
   return statusCode == 202;
}

bool ASC_PBEM::uploadFile( ASCString fileName, const char* data, const int size, const int gameID )
{
   if( ! loggedIn ) return false;
   
   
   ASC_PBEM_FileUploadControl control( data, size );
   control.step = 0;
   control.sent_step = 0;
   control.fileName = fileName;
   control.boundary = "--------------------ASCPBEM65834626956";
   // ??? todo: a better boundary id would be nice... perhapse a random id
   
   char gameIDString[ 30 ];
   sprintf( gameIDString, "%i", gameID );
   
   control.parameters.push_back( "gameID" );
   control.parameters.push_back( gameIDString );
   
   if( ! request( "tools/upload", &control ) ) return false;
   
   return statusCode == 201;
}

bool ASC_PBEM::createGame( ASCString fileName, const char* data, const int size, ASCString gameName, ASCString fileNamePattern, char* roles, int* players, int projectID, int turn, int currentSlot )
{
   if( ! loggedIn ) return false;
   
   
   ASC_PBEM_FileUploadControl control( data, size );
   control.step = 0;
   control.sent_step = 0;
   control.fileName = fileName;
   control.boundary = "--------------------ASCPBEM65834626956";
   // ??? todo: a better boundary id would be nice... perhapse a random id
   
   char projectIDString[ 30 ];
   sprintf( projectIDString, "%i", projectID );

   char turnString[ 30 ];
   sprintf( turnString, "%i", turn );

   char currentSlotString[ 30 ];
   sprintf( currentSlotString, "%i", currentSlot );
   
   control.parameters.push_back( "projectID" );
   control.parameters.push_back( projectIDString );
   control.parameters.push_back( "turn" );
   control.parameters.push_back( turnString );
   control.parameters.push_back( "currentSlot" );
   control.parameters.push_back( currentSlotString );
   control.parameters.push_back( "gameName" );
   control.parameters.push_back( gameName );
   control.parameters.push_back( "pattern" );
   control.parameters.push_back( fileNamePattern );
   
   for( int i=0; i<8; i++ )
   {
      char playerString[ 10 ];
      char playerIDString[ 10 ];
      sprintf( playerString, "player_%i", i );
      sprintf( playerIDString, "%i", players[ i ] );
      
      char roleString[ 10 ];
      char roleIDString[ 10 ];
      sprintf( roleString, "role_%i", i );
      sprintf( roleIDString, "%c", roles[ i ] );

      control.parameters.push_back( playerString );
      control.parameters.push_back( playerIDString );
      control.parameters.push_back( roleString );
      control.parameters.push_back( roleIDString );
   }
   
   if( ! request( "tools/newgame", &control ) ) return false;
   
   return statusCode == 201;
}


std::vector<TGameInfo> ASC_PBEM::getCurrentGamesInfo( bool myTurnOnly )
{
   std::vector<TGameInfo> games;
   
   if( !loggedIn ) return games;
   
   std::vector< ASCString > parameters;
   parameters.push_back( "myTurnOnly" );
   if( myTurnOnly )
      parameters.push_back( "1" );
   else
      parameters.push_back( "0" );
   
   if( request( "tools/currentGameList", parameters ) )
   {
      if( statusCode == 200 )
      {
         int currentIndex = 0;
         ASCString bodyString = getBody();
         while( currentIndex < bodyString.size() )
         {
            int lineEnd = bodyString.find( '\n', currentIndex );
            ASCString line;
            if( lineEnd != ASCString::npos )
            {
               line = bodyString.substr( currentIndex, lineEnd-currentIndex );
               games.push_back( parseGameInfoLine( line ) );
            }else
            {
               line = bodyString.substr( currentIndex );
               games.push_back( parseGameInfoLine( line ) );
               break;
            }
            currentIndex = lineEnd + 1;
         }
         
      }
   }
   
   return games;
   
}


std::vector<TUserData> ASC_PBEM::getUserList( bool activeOnly )
{
   std::vector<TUserData> users;
   
   if( !loggedIn ) return users;
   
   std::vector< ASCString > parameters;
   parameters.push_back( "activeOnly" );
   if( activeOnly )
      parameters.push_back( "1" );
   else
      parameters.push_back( "0" );
   
   if( request( "tools/userList", parameters ) )
   {
      if( statusCode == 200 )
      {
         int currentIndex = 0;
         ASCString bodyString = getBody();
         while( currentIndex < bodyString.size() )
         {
            int lineEnd = bodyString.find( '\n', currentIndex );
            ASCString line;
            if( lineEnd != ASCString::npos )
            {
               line = bodyString.substr( currentIndex, lineEnd-currentIndex );
               users.push_back( parseUserInfoLine( line ) );
            }else
            {
               line = bodyString.substr( currentIndex );
               users.push_back( parseUserInfoLine( line ) );
               break;
            }
            currentIndex = lineEnd + 1;
         }
         
      }
   }
   
   return users;
}

std::vector<ASCString> ASC_PBEM::listPlayers()
{
   std::vector<ASCString> newList;
   std::vector<TUserData> list = getUserList();

   for ( std::vector<TUserData>::iterator i = list.begin(); i != list.end(); ++i )
      newList.push_back( i->userName );

   return newList;
}



TFileData* ASC_PBEM::downloadGame( TGameInfo game )
{
   if( ! loggedIn ) return NULL;
   /*
   std::vector< ASCString > parameters;
   
   parameters.push_back( "name" );
   parameters.push_back( user );
   parameters.push_back( "code" );
   parameters.push_back( code );
   */
   char gameIDString[ 30 ];
   sprintf( gameIDString, "%i",  game.gameID );
   char* fileNameEscaped = curl_escape( game.currentSaveGameName.c_str(), 0 );
   
   ASCString requestString = "tools/download/";
   requestString += fileNameEscaped;
   requestString += "?gameID=";
   requestString += gameIDString; 
   
   if( ! request( requestString ) )
   {
      curl_free( fileNameEscaped );
      return NULL;
   }
   curl_free( fileNameEscaped );
   
   if( statusCode == 200 )
   {
      ASCString bodyString = getBody();
      
      TFileData *data = new TFileData;
      data->fileName = game.currentSaveGameName;
      data->fileSize = bodyString.size();
      data->fileData = new char[ bodyString.size() ];
      ASC_PBEM_writeBuffer( data->fileData, bodyString.c_str(), 0, bodyString.size(), bodyString.size() );
      
      return data;
   }
   
   return NULL;
}



TUserData ASC_PBEM::parseUserInfoLine( ASCString line )
{
   TUserData info;
   
   info.userID = atoi( line.substr( 0, line.find( '\t' ) ).c_str() );
   int currentIndex = line.find( '\t' ) + 1;
   info.userName = line.substr( currentIndex );
   
   return info;
}

TGameInfo ASC_PBEM::parseGameInfoLine( ASCString line )
{
   TGameInfo info;
   
   info.gameID = atoi( line.substr( 0, line.find( '\t' ) ).c_str() );
   int currentIndex = line.find( '\t' ) + 1;
   info.name = line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.projectID = atoi( line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex ).c_str() );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.currentSlot = atoi( line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex ).c_str() );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.turn = atoi( line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex ).c_str() );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.currentSaveGameName = line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.lastActive = line.substr( currentIndex, line.find( '\t', currentIndex ) - currentIndex );
   currentIndex = line.find( '\t', currentIndex ) + 1;
   info.currentPlayerName = line.substr( currentIndex );
   
   return info;
}

ASCString ASC_PBEM::getHeader()
{
   ASCString headerString;
   for( int i=0; i<header.size(); i++ ) 
      headerString += header[ i ];
   return headerString;
}

ASCString ASC_PBEM::getBody()
{
   ASCString bodyString;
   for( int i=0; i<body.size(); i++ ) 
      bodyString += body[ i ];
   return bodyString;
}

int ASC_PBEM::getStatusCode()
{
   return statusCode;
}

int nmain(int argc, char *argv[])
{
   std::cout << "start" << std::endl;
  curl_global_init(CURL_GLOBAL_ALL);
   
   if( false )
   {
      // schnelle Ã¼bersicht, was man wie machen kann
      // wichtig: curl_global_init(CURL_GLOBAL_ALL); nicht vergessen ;)
      // weiter unten sind dann die sachen, die ich zum testen verwendet habe, 
      // ist halt unklarer geschrieben ;)
      
      /** account erstellen
      
         ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
         if( asc_pbem.isUsable() && asc_pbem.createAccount( "user", "password", "a@b.c" ) )
         {
            std::cout << "account erstellt, mail unterwegs\n";
         }else
         {
            std::cout << "account erstellung fehlgeschlagen\n";
         }
      */
      
      /** account aktivieren
      
         ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
         if( asc_pbem.isUsable() && asc_pbem.activateAccount( "user", "code" ) )
         {
            std::cout << "account aktiviert, erwarte freischaltung\n";
         }else
         {
            std::cout << "account aktivierung fehlgeschlagen\n";
         }
      */
      
      /** upload eines spielstandes;
      
         char* daten = "miau";
         int size = strlen( "miau" ) * sizeof( char );
         ASCString fileName = "duell-A-1.ascpbm";
         int gameID = 1;
         
         ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
         if( asc_pbem.isUsable() && asc_pbem.login( "user", "pw" ) )
         {
            if( asc_pbem.uploadFile( fileName, daten, size, gameID ) )
            std::cout << "account aktiviert, erwarte freischaltung\n";
         }else
         {
            std::cout << "account aktivierung fehlgeschlagen\n";
         }
      */
      
      /** download eines spielstandes

         ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
         if( asc_pbem.isUsable() )
         {
            std::vector<TGameInfo> allGames = asc_pbem.getCurrentGamesInfo( true );
            TFileData *download = asc_pbem.downloadGame( allGames[ 0 ] );
            
            if( download == NULL )
            {
               std::cout << "download failed" << std::endl;
            }else
            {
               std::cout << "download worked:" << std::endl;
               delete download;
            }
         }
      */
   }
   
   ASC_PBEM asc_pbem( "http://localhost:8080/ascServer/" );
   
   if( asc_pbem.isUsable() )
   {
      std::cout << "primary isUsable" << std::endl;
      /*
      if( asc_pbem.createAccount( "codeUser", "codePW", "jade@Rhiow.Errant" ) )
      {
         std::cout << "create codeUser worked" << std::endl;
         
         std::cout << "header:" << asc_pbem.getHeader() << std::endl;
         std::cout << "body:" << asc_pbem.getBody() << std::endl;
         std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
         
      }else
      {
         std::cout << "create codeUser failed" << std::endl;
      }
      */
      /*
      if( asc_pbem.activateAccount( "codeUser", "8670616" ))
      {
         std::cout << "activate codeUser worked" << std::endl;
      }else
      {
         std::cout << "activate codeUser failed" << std::endl;
      }
      */
      
      if( asc_pbem.login( "miau", "aaaa" ) )
      {
         std::cout << "login worked" << std::endl;
      }else
      {
         std::cout << "login failed" << std::endl;
      }
      /*
      ASCString uploadData = "a\ns\nd\nmiau\n";
      int gameID = 3;
      //std::cout << uploadData.size() << std::endl;
      if( asc_pbem.uploadFile( "some filename.ascpbm", uploadData.c_str(), uploadData.size(), gameID ) )
      {
         std::cout << "uploadFile worked" << std::endl;*/
         /*
         std::cout << "header:" << asc_pbem.getHeader() << std::endl;
         std::cout << "body:" << asc_pbem.getBody() << std::endl;
         std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
         */
         
         /*
      }else
      {
         std::cout << "uploadFile failed" << std::endl;
         
         std::cout << "header:" << asc_pbem.getHeader() << std::endl;
         std::cout << "body:" << asc_pbem.getBody() << std::endl;
         std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
         
      }*/
      
      std::vector<TGameInfo> allGames = asc_pbem.getCurrentGamesInfo( false );
      std::cout << "header:" << asc_pbem.getHeader() << std::endl;
      std::cout << "body:" << asc_pbem.getBody() << std::endl;
      std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
      std::cout << "allGames.size() = " << allGames.size() << std::endl;

      for( int i=0; i<allGames.size(); i++ )
      {
         TGameInfo game = allGames[ i ];
         std::cout << "Game " << game.gameID << std::endl;
         std::cout << "name " << game.name << std::endl;
         std::cout << "projectID " << game.projectID << std::endl;
         std::cout << "currentSlot " << game.currentSlot << std::endl;
         std::cout << "turn " << game.turn << std::endl;
         std::cout << "currentSaveGameName " << game.currentSaveGameName << std::endl;
         std::cout << "lastActive " << game.lastActive << std::endl;
         std::cout << "currentPlayerName " << game.currentPlayerName << std::endl;
      }
      
      /*
      char roles[ 8 ];
      int players[ 8 ];
      for( int i=0; i<8; i++ )
      {
         roles[ i ] = asc_pbem.ROLE_PLAYER;
         players[ i ] = -1;
      }
      
      players[ 0 ] = 11;
      players[ 1 ] = 11;
      
      int projectID = -1;
      int turn = 1;
      int currentSlot = 1;
      ASCString gameName = "evil isle";
      ASCString fileName = "evilisle-A-1.ascpbm";
      ASCString fileNamePattern = "evilisle-$p-$t.ascpbm";
      
      if( asc_pbem.createGame( fileName, fileName.c_str(), fileName.size(), gameName, fileNamePattern, roles, players, projectID, turn, currentSlot ) )
      {
         std::cout << "game created" << std::endl;
         std::cout << "header:" << asc_pbem.getHeader() << std::endl;
         std::cout << "body:" << asc_pbem.getBody() << std::endl;
         std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
      }else
      {
         std::cout << "game creation failed" << std::endl;
         std::cout << "header:" << asc_pbem.getHeader() << std::endl;
         std::cout << "body:" << asc_pbem.getBody() << std::endl;
         std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
      }
      */
      
      
      /*
      for( int i=0; i<allGames.size(); i++ )
      {
         TGameInfo game = allGames[ i ];
         
         if( game.gameID == 3 )
         {
            TFileData *download = asc_pbem.downloadGame( game );
            
            if( download == NULL )
            {
               std::cout << "download failed" << std::endl;
            }else
            {
               std::cout << "download worked:" << std::endl;
               std::cout << download->fileName << std::endl;
               std::cout << download->fileSize << std::endl;
               std::cout << download->fileData << std::endl;
               
               delete download;
            }
         }
         
         
      }
      */
/*
      std::vector<TUserData> users = asc_pbem.getUserList();
      std::cout << "users.size() = " << users.size() << std::endl;

      for( int i=0; i<users.size(); i++ )
      {
         TUserData user = users[ i ];
         std::cout << "userName " << user.userName << std::endl;
         std::cout << "userID " << user.userID << std::endl;
      }
*/
      if( asc_pbem.logout() )
      {
         std::cout << "logout worked" << std::endl;
      }else
      {
         std::cout << "logout failed" << std::endl;
      }
   }else
   {
      std::cout << "primary NOT isUsable" << std::endl;
      
      std::cout << "header:" << asc_pbem.getHeader() << std::endl;
      std::cout << "body:" << asc_pbem.getBody() << std::endl;
      std::cout << "status:" << asc_pbem.getStatusCode() << std::endl;
   }
   return 0;    
}



#include "scannerwrapper.h"
#include "parserbase.h"
#include "scanner.h"
#include "parser.h"


class ScannerPrivateData {
   public:
      yyscan_t scanner;
};

Scanner::Scanner()
{
   data = new ScannerPrivateData();
   yylex_init( &data->scanner );
   yyset_out( NULL, data->scanner );
}

int Scanner::yylex()
{
   return ::yylex( data->scanner);
}
Scanner::~Scanner()
{
   yylex_destroy( data->scanner );
   delete data;
}

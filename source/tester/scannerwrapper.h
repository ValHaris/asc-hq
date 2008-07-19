
#ifndef ScannerH
#define ScannerH

class ScannerPrivateData;

class Scanner {
      
      ScannerPrivateData* data;
   public:
      Scanner();
      int yylex();
      ~Scanner();
};


#endif

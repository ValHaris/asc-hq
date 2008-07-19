%filenames parser
%scanner scannerwrapper.h
%parsefun-source parser.cpp

%debug
%error-verbose

%token MOVEUNIT LOADMAP TESTCASE

%%

asctextfile  : blocks
;
      
blocks       : block      
             | block blocks
;
            
block        : testblock      
;
      
testblock    : TESTCASE '{' commands '}' TESTCASE
;

commands     : command ';'
             | command commands 
;
      
command      : MOVEUNIT 
             | LOADMAP  
;


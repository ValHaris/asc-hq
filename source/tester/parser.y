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
             | blocks block
;
            
block        : testblock      
;
      
testblock    : TESTCASE '{' commands '}' TESTCASE
;

commands     : command ';'
             | commands command  
;
      
command      : MOVEUNIT 
             | loadmapcommand  
;

loadmapcommand : LOADMAP '(' filename ')'
;

filename    : 
;
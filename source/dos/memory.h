#ifndef _DOS_memory_h_included_

 #define _DOS_memory_h_included_
 extern  void emergency_new_handler ( void );
 extern  void new_new_handler ( void );
 extern  void initmemory( void ) ;

 extern int maxavail( void );
 extern int memavail( void );

#endif


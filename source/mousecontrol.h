class cmousecontrol {
         protected:
            int mousestat;
         public:
            cmousecontrol( void ) : mousestat ( 0 ) {};
            void chkmouse ( void );
            void reset ( void );
};


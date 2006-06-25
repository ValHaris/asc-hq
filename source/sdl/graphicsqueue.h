#ifndef graphicsqueueH
#define graphicsqueueH

#include <list>
#include <SDL.h>

 #include "../libs/loki/Functor.h"

class GraphicsQueueOperation {
   public:
      virtual void execute() = 0;
      virtual ~GraphicsQueueOperation() {};
   };
   
   class UpdateRectOp : public GraphicsQueueOperation {
         SDL_Surface *screen;
         Sint32 x, y, w, h;
      public:
         UpdateRectOp( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) {
               this->x = x;
               this->y = y;
               this->w = w;
               this->h = h;
               this->screen = screen;
          };
          
          void execute();
      };

   class UpdateRectsOp : public GraphicsQueueOperation {
         SDL_Surface *screen;
         int numrects;
         SDL_Rect *rects;
      public:
         UpdateRectsOp( SDL_Surface *screen, int numrects, SDL_Rect *rects);
          
          void execute();
          ~UpdateRectsOp();
      };
      
   class MouseVisibility : public GraphicsQueueOperation {
         bool visible;
      public:
         MouseVisibility( bool  visi )  : visible( visi ) { };
          void execute() { SDL_ShowCursor(visible); };
      };



   class InitScreenOp : public GraphicsQueueOperation {
         int x,y,depth,flags;
      public:
         typedef Loki::Functor<void, TYPELIST_1(SDL_Surface*) > ScreenRegistrationFunctor;
      private:
         ScreenRegistrationFunctor srf;
      public:

         InitScreenOp( int x, int y, int depth, int flags, ScreenRegistrationFunctor screenRegistrationFunctor ) 
         {
            this->x = x;
            this->y = y;
            this->depth = depth;
            this->flags = flags;
            srf = screenRegistrationFunctor;
         };
         void execute();
      };

 extern void queueOperation( GraphicsQueueOperation* gqo, bool wait = false );

#endif

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef containerbasefunctionsH
 #define containerbasefunctionsH

 #include <sigc++/sigc++.h>

 #include "typen.h"
 #include "containerbase.h"
 #include "graphics/surface.h"
 #include "mapalgorithms.h"


class MatterConverter : public ContainerBase::Work
{
      ContainerBase* bld;
      int percentage;
   public:
      MatterConverter( ContainerBase* _bld ) ;
      virtual bool finished();
      virtual bool run();
      virtual Resources getPlus();
      virtual Resources getUsage();
};


class ResourceSink : public ContainerBase::Work
{
   ContainerBase* bld;
   Resources toGet;
   public:
      ResourceSink( ContainerBase* _bld ) ;
      virtual bool finished();
      virtual bool run();
      virtual Resources getPlus();
      virtual Resources getUsage();
};

class RegenerativePowerPlant : public ContainerBase::Work
{
   protected:
      ContainerBase* bld;
      Resources toProduce;
   public:
      RegenerativePowerPlant( ContainerBase* _bld ) ;
      virtual bool finished();
      virtual bool run();
      virtual Resources getUsage();
};

class WindPowerplant : public RegenerativePowerPlant
{
   public:
      WindPowerplant( ContainerBase* _bld ) : RegenerativePowerPlant ( _bld )
      {
         toProduce = getPlus();
      };
      virtual Resources getPlus();
};

class SolarPowerplant : public RegenerativePowerPlant
{
   public:
      SolarPowerplant( ContainerBase* _bld ) : RegenerativePowerPlant ( _bld )
      {
         toProduce = getPlus();
      };
      virtual Resources getPlus();
};

class BiResourceGeneration: public RegenerativePowerPlant
{
   public:
      BiResourceGeneration ( ContainerBase* bld_ ) : RegenerativePowerPlant ( bld_ )
      {
         toProduce = getPlus();
      };
      virtual Resources getPlus();
};

class MiningStation : public ContainerBase::Work, protected SearchFields
{
   ContainerBase* bld;
   bool justQuery;
   bool hasRun;
   Resources toExtract_thisTurn;
   Resources spaceAvail;
   Resources powerAvail;
   Resources actuallyExtracted; // with increasing distance this gets lower and lower

   float consumed[3];
   float usageRatio[3];
   protected:
      void testfield ( const MapCoordinate& mc );
   public:
      MiningStation( ContainerBase* _bld, bool justQuery_ ) ;
      virtual bool finished();
      virtual bool run();
      virtual Resources getPlus();
      virtual Resources getUsage();
};



#endif

/*! \file baseaiinterface.h
    \brief Some AI related classes
*/

/***************************************************************************
                          baseaiinterface.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef baseaiinterface_h_included
 #define baseaiinterface_h_included


//! how many different target types are there?
const int aiValueTypeNum = 8;


class AiThreat {
       public:
         int threatTypes;
         int threat[aiValueTypeNum];
         void reset ( void );
         AiThreat ( void ) : threatTypes ( aiValueTypeNum ) { reset(); };
         AiThreat& operator+= ( const AiThreat& t ) {
            for ( int i = 0; i < threatTypes; i++ )
               threat[i] += t.threat[i];
            return *this;
         };

         void read ( tnstream& stream );
         void write ( tnstream& stream );
};



class AiValue {
           int value;
           int addedValue;
        public:
           AiThreat threat;
           int valueType;

           void reset ( int _valueType ) { threat.reset(); value = 0; valueType = _valueType; addedValue = 0; };
           AiValue ( int _valueType ) { reset( _valueType ); };
           int getValue() { return value + addedValue; };
           void setValue ( int _value ) { value = _value; };
           void setAdditionalValue ( int _addedValue ) { addedValue = _addedValue; };
           void resetAdditionalValue (  ) { addedValue = 0; };

           void read ( tnstream& stream );
           void write ( tnstream& stream );
        };

class AiParameter : public AiValue {
           pvehicle unit;
        public:
           enum Task { tsk_nothing, tsk_tactics, tsk_tactwait, tsk_stratwait, tsk_wait, tsk_strategy, tsk_serviceRetreat, tsk_move } task;
           enum Job { job_undefined, job_fight, job_supply, job_conquer, job_build, job_recon, job_guard } job;
           int lastDamage;
           tgametime damageTime;

           MapCoordinate3D dest;
           int dest_nwid;
           int data;

           void reset ( pvehicle _unit );
           void resetTask ( );
           AiParameter ( pvehicle _unit );

           void read ( tnstream& stream );
           void write ( tnstream& stream );
 };

class BaseAI {
       public:
         virtual void run ( void ) = 0;
         virtual bool isRunning ( void ) = 0;
         virtual int getVision ( void ) = 0;
         virtual void read ( tnstream& stream ) = 0;
         virtual void write ( tnstream& stream ) const = 0;
         virtual ~BaseAI () {};
      };



#endif

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

/** the threat that a unit poses against other units. Since a given unit may
    usually not attack all kinds of units ( satellites, submarines, etc ), there are
    several different groups of unitTypes, with a different threat rating each
    \see aiValueTypeNum
*/
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


/** The value of a unit for the AI. The value consists of a base value calculated from
    the unit type, damage etc and an additional value. The additional value is used for
    example if a unit is trying to capture one of your buildings, of if the mission
    goals say that this unit must be protected at all cost (if it is yours) or destroyed
    (if it is the enemies') */
class AiValue {
        #ifdef karteneditor
        public:
        #endif
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

//! All parameters the AI stores persistently about a unit.
class AiParameter : public AiValue {
           pvehicle unit;
        public:
           static const int taskNum = 8;
           static const int jobNum = 7;
           enum Task { tsk_nothing, tsk_tactics, tsk_tactwait, tsk_stratwait, tsk_wait, tsk_strategy, tsk_serviceRetreat, tsk_move };
           enum Job { job_undefined, job_fight, job_supply, job_conquer, job_build, job_recon, job_guard };
           typedef vector<AiParameter::Job> JobList;

           Task getTask ( ) { return task; };
           void setTask ( Task t ) { task = t; };

           Job getJob ( ) { if ( jobPos < jobs.size() ) return jobs[jobPos]; else return job_undefined; };
           void addJob ( Job j, bool front = false );

           void setNextJob();
           void restartJobs();
           void clearJobs();
           void setJob ( const JobList& jobs );


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
        private:
           Task task;
           JobList jobs;
           int jobPos;
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

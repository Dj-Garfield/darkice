/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell Reporter

   File     : Reporter.h
   Version  : $Revision$
   Author   : $Author$
   Location : $Source$
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

------------------------------------------------------------------------------*/
#ifndef REPORTER_H
#define REPORTER_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "configure.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#error need unistdt.h
#endif


#include <iostream.h>


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class for reporting events. All objects of this class share
 *  the same verbosity level. Typical usage is to inherit this class
 *  and use the report() function to report events. Only reports
 *  which are of suffucient importance are really reported.
 *
 *  The default verbosity is 1, and the default ostream is cout.
 * 
 *  Known problems: this class is not thread-safe.
 * 
 *  @author  $Author$
 *  @version $Revision$
 */
class Reporter
{
    
    
    private:

        /**
         *  Verbosity level shared among all Reporter objects
         */
        static unsigned int     verbosity;

        /**
         *  The output stream to report to.
         */
        static ostream        * os;

        
    protected:


    public:

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~Reporter ( void )                                  throw ()
        {
            (Reporter::os)->flush();
        }

        /**
         *  Set the verbosity level. This sets the verbosity for all
         *  Reporter objects.
         *
         *  @param verbosity the new verbosity level.
         */
        inline void
        setReportVerbosity ( unsigned int     verbosity )   throw ()
        {
            Reporter::verbosity = verbosity;
        }

        /**
         *  Get the verbosity level.
         *
         *  @return the current verbosity level.
         */
        inline unsigned int
        getReportVerbosity ( void )                         throw ()
        {
            return Reporter::verbosity;
        }

        /**
         *  Set the output stream to report to. This setting effects all
         *  Reporter objects.
         *
         *  @param os the output stream
         */
        inline void
        setReportOutputStream ( ostream   & os )            throw ()
        {
            Reporter::os = &os;
        }

        /**
         *  Get the output stream to report to.
         *
         *  @return the output stream
         */
        inline ostream &
        getReportOutputStream ( void )                      throw ()
        {
            return *(Reporter::os);
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param message the message to report.
         */
        inline void
        reportEvent ( unsigned int  verbosity,
                      const char  * message )               throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                (*(Reporter::os)) << getpid() << ": "
                                  << message <<  endl;
            }
        }

        /**
         *  Report an event with a given verbosity.
         *
         *  @param verbosity the importance of the event, with 0 being
         *                   the most important.
         *  @param message1 the message to report part 1.
         *  @param message2 the message to report part 2.
         */
        template<class T>
        inline void
        reportEvent ( unsigned int  verbosity,
                      const char  * message1,
                      const T       t )                     throw ()
        {
            if ( Reporter::verbosity >= verbosity ) {
                (*(Reporter::os)) << getpid() << ": "
                                  << message1 << " " << t <<  endl;
            }
        }

};


    

/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* REPORTER_H */


/*------------------------------------------------------------------------------
 
  $Source$

  $Log$
  Revision 1.1  2000/11/16 08:48:43  darkeye
  added multiple verbosity-level event reporting and verbosity command
  line option


  
------------------------------------------------------------------------------*/


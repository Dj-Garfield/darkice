/*------------------------------------------------------------------------------

   Copyright (c) 2000 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : SinkLoop.h
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
#ifndef SINK_LOOP_H
#define SINK_LOOP_H

#ifndef __cplusplus
#error This is a C++ include file
#endif


/* ============================================================ include files */

#include "Sink.h"

#include <stdio.h>


/* ================================================================ constants */


/* =================================================================== macros */


/* =============================================================== data types */

/**
 *  Class representing a looping sink-forwarder
 *
 *  @author  $Author$
 *  @version $Revision$
 */
class SinkLoop : public Sink
{
    private:

        /**
         *  The sink to pass the data to
         */
        Ref<Sink>       targetSink;

      	unsigned int limit;
      	unsigned int written;
      	//if seconds > 0 then break whenever time() % seconds == 0
      	unsigned int seconds;
      	int prevModulus;
      	int firstPass;

        /**
         *  Initalize the object.
         *
         *  @param targetSink the sink to pass the data to.
         *  @exception Exception
         */
        inline void
        init (  Sink          * targetSink,
        		    unsigned int  dataLimit,
        		    unsigned int  breakSeconds )        throw ( Exception )
        {
            this->targetSink = targetSink;
  	        limit = dataLimit;
  	        if(limit <= 0) throw Exception(__FILE__,__LINE__);
  	        written = 0;
  	        seconds = breakSeconds;
  	        prevModulus = -1;
  	        firstPass = 1;
        }

        /**
         *  De-initalize the object.
         *
         *  @exception Exception
         */
        inline void
        strip ( void )                              throw ( Exception )
        {
            if ( isOpen() ) {
                close();
            }
        }


    protected:

        /**
         *  Default constructor. Always throws an Exception.
         *
         *  @exception Exception
         */
        inline
        SinkLoop ( void )                            throw ( Exception )
        {
            throw Exception( __FILE__, __LINE__);
        }


    public:

        /**
        *  Constructor.
        *
        *  @param targetSink the sink to send all data to
        *  @param dataLimit the limit (in bytes) of data to be written
        *         before we start checking if the time is right
        *         (usually 15 seconds less than the time)
        *  @param breakSeconds Duration of clips in seconds,
        *         if non-zero breaks occur when time() % breakSeconds == 0
        *  @exception Exception
        */
        inline
        SinkLoop (  Sink          * targetSink,
            		    unsigned int  dataLimit,
                    unsigned int  breakSeconds)                    throw ( Exception )
                  : Sink()
        {
            init( targetSink,dataLimit,breakSeconds );
        }

        /**
         *  Constructor.
         *
         *  @param targetSink the sink to send all data to
	     *  @param dataLimit the limit (in bytes) of data to be written
         *  @exception Exception
         */
        inline
        SinkLoop (  Sink          * targetSink,
		                unsigned int  dataLimit )                     throw ( Exception )
                  : Sink()
        {
            init( targetSink,dataLimit,0 );
        }

        /**
         *  Copy constructor.
         *
         *  @param cs the SinkLoop to copy.
         */
        inline
        SinkLoop(   const SinkLoop &    cs )        throw ( Exception )
        {
            init( cs.targetSink.get(),cs.limit,cs.seconds );
        }

        /**
         *  Destructor.
         *
         *  @exception Exception
         */
        inline virtual
        ~SinkLoop( void )                           throw ( Exception )
        {
            strip();
        }

        /**
         *  Assignment operator.
         *
         *  @param cs the SinkLoop to assign this to.
         *  @return a reference to this SinkLoop.
         *  @exception Exception
         */
        inline virtual SinkLoop &
        operator= ( const SinkLoop &    cs )        throw ( Exception )
        {
            if ( this != &cs ) {
                strip();
                init( cs.targetSink.get(),cs.limit,cs.seconds );
            }
            return *this;
        }

        /**
         *  Open the SinkLoop.
         *
         *  @return true if opening was successfull, false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        open ( void )                               throw ()
        {
  	        if(isOpen()) {
  		          flush();
  	            targetSink->close();
  	        }

            //On the first pass we should be checking time() constantly
  	        //so that we're aligned to start with, so make it look like
  	        //we've already written enough to hit the limit...
  	        if(firstPass && seconds > 0)
  		          written = limit +1;
  	        else
  		          written = 0;
  	        firstPass = 0;
  	        prevModulus = -1;
  	        return targetSink->open();
        }

        /**
         *  Check if the SinkLoop is open.
         *
         *  @return true if the target sink is open, false otherwise.
         */
        inline virtual bool
        isOpen ( void ) const                       throw ()
        {
            return targetSink->isOpen();
        }

        /**
         *  Check if the SinkLoop is ready to accept data.
         *  Blocks until the specified time for data to be available.
         *
         *  @param sec the maximum seconds to block.
         *  @param usec micro seconds to block after the full seconds.
         *  @return true if the SinkLoop is ready to accept data,
         *          false otherwise.
         *  @exception Exception
         */
        inline virtual bool
        canWrite (     unsigned int    sec,
                       unsigned int    usec )       throw ( Exception )
        {
      	    /*No need to check here, since we check after every write,
      	     *should never get here with written >= limit..
      	     */
      	    return targetSink->canWrite( sec, usec);
        }

        /**
         *  Write data to the SinkLoop.
         *
         *  @param buf the data to write.
         *  @param len number of bytes to write from buf.
         *  @return the number of bytes written (may be less than len).
         *  @exception Exception
         */
        inline virtual unsigned int
        write (        const void    * buf,
                       unsigned int    len )        throw ( Exception )
        {
            unsigned int ret = targetSink->write( buf, len);
  	        written += ret;
  	        if(written >= limit) {
  		          if(seconds > 0)	{
    		            int modulus = time(NULL) % seconds;
    		            if(modulus < prevModulus) {
			                  open();
	                  } else {
  			                prevModulus = modulus;
                    }
  		          } else {
    		            //Force a close & re-open
    		            open();
                }
	          }
	          return ret;
        }

        /**
         *  Flush all data that was written to the SinkLoop to the server.
         *
         *  @exception Exception
         */
        inline virtual void
        flush ( void )                              throw ( Exception )
        {
            return targetSink->flush();
        }

        /**
         *  Close the SinkLoop.
         *
         *  @exception Exception
         */
        inline virtual void
        close ( void )                              throw ( Exception )
        {
	        firstPass =1;
            return targetSink->close();
        }

  	    inline virtual void
  	    cut ( void )				    throw ()
  	    {
  	        return targetSink->cut();
  	    }
};


/* ================================================= external data structures */


/* ====================================================== function prototypes */



#endif  /* FILE_CAST_H */

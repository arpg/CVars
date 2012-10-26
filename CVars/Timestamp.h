/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.
   
    $Id: Timestamp.h 162 2010-02-15 19:11:05Z gsibley $

 */


// cross platform timing utility
#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>

#ifndef WIN32
#    include <sys/time.h>
#endif

class TimeStamp
{
	public:
	TimeStamp();
	void Stamp();
	double Elapsed(); //in seconds since last "stamp"
	double TotalElapsed(); //total since first stamp
	int ElapsedFrames(double frameTime, double factor=1.0); //given target time in millisecs per frames

	#ifdef WIN32
	//allow timer to be pauses in between "stamps"
	void Pause();
	//unpause the timer...
	void UnPause();
	#endif

	private:
	int start;
	
    #ifndef WIN32
	  struct timeval prevTime, startTime;
	  struct timezone tz;
	#else
	  double prevTime, startTime;
	  double pauseTime;
	  bool isPaused;
	#endif
	
	double overflow;

};

#endif

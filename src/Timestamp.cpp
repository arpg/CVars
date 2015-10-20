/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: Timestamp.cpp 162 2010-02-15 19:11:05Z gsibley $

 */

#include <time.h>
#include <iostream>
#include <stdio.h>
#include <cvars/config.h>

#ifdef _WIN_
#    include <windows.h>
#endif

#include <cvars/Timestamp.h>

TimeStamp::TimeStamp()
{
	start = 1;
	overflow = 0;

	#ifdef _WIN_
	isPaused = false;
	#endif
}

#ifndef _WIN_
void TimeStamp::Stamp()
{
    gettimeofday(&prevTime, &tz);
    //overflow = 0;
    if(start == 1)
    {
    	start = 0;
    	startTime = prevTime;
    }
}
#endif

#ifdef _WIN_
void TimeStamp::Stamp()
{
	if(isPaused)
		UnPause();

	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
    prevTime = (double) time.QuadPart / (double) freq.QuadPart;

    //overflow = 0;
    if(start == 1)
    {
    	start = 0;
    	startTime = prevTime;
    }
}
#endif

#ifndef _WIN_
double TimeStamp::TotalElapsed()
{
	if(start == 1)
	{
	   start = 0;
	   return 0; 
	}
	
	//get current time
	struct timeval currTime;
	struct timezone currTz;
	gettimeofday(&currTime, &currTz);
	
	double t1 = (double)startTime.tv_sec + (double) startTime.tv_usec/(1000*1000);
	double t2 =  (double)currTime.tv_sec + (double)currTime.tv_usec/(1000*1000);
	return t2-t1;
}
#endif

#ifdef _WIN_
double TimeStamp::TotalElapsed()
{
	if(start == 1)
	{
	   start = 0;
	   return 0; 
	}
	
	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;

	return currTime-startTime;
}
#endif

#ifndef _WIN_
//returns very precise time in seconds since last "stamp"
double TimeStamp::Elapsed() 
{
	if(start == 1)
	{
	   start = 0;
	   return 0;
 
	}
	
	//get current time
	struct timeval currTime;
	struct timezone currTz;
	gettimeofday(&currTime, &currTz);
	
	double t1 = (double)prevTime.tv_sec + (double) prevTime.tv_usec/(1000*1000);
	double t2 =  (double)currTime.tv_sec + (double)currTime.tv_usec/(1000*1000);
	return t2-t1;
}
#endif

#ifdef _WIN_
double TimeStamp::Elapsed() 
{
	if(start == 1)
	{
	   start = 0;
	   return 0;
 
	}
	

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;
	
	double elapsed;
	if(isPaused)
	{
		UnPause();
		elapsed = currTime-prevTime;
		Pause();
	}
	else
		elapsed = currTime-prevTime;

	return elapsed;
}
#endif

//returns the # of frames that have elapsed since the last "stamp"
//frameTime is the time per frame in milliseconds
//factor is the scaling factor used to speed and slow the timer
int TimeStamp::ElapsedFrames(double frameTime, double factor)
{
  //double elapSec = Elapsed();
  
  double total =  ((Elapsed() / (frameTime/1000)) + overflow)*factor;
  int result = (int) total;
  overflow = total - result;
  
  return result;
}

#ifdef _WIN_
//allow timer to be pauses in between "stamps"
void TimeStamp::Pause()
{
	if(isPaused)
		return;

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);

	pauseTime = (double)time.QuadPart / (double) freq.QuadPart;
	isPaused = true;
}

//unpause the timer...
void TimeStamp::UnPause()
{
	if(!isPaused)
		return;

	//get current time
	LARGE_INTEGER time, freq;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&freq);
	double currTime = (double)time.QuadPart / (double) freq.QuadPart;

	prevTime += currTime - pauseTime;
	isPaused = false;
}
#endif

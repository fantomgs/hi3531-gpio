/*
 * =====================================================================================
 *
 *       Filename:  serial_demon.c
 *
 *    Description: this file is for open a thread to listen a GPIO COM  
 *
 *        Version:  1.0
 *        Created:  2015-07-13 01:43:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Mike) 
 *        Company: 	RunWell 
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

#include <pthread.h>
#include <string.h>
#include <stdarg.h>
//#####################   Logger ############################### 
#ifdef ENABLE_LOG
#define TRACE_LOG			trace_log
#else
#define TRACE_LOG     1 ? (void)0 : trace_log
#endif
#define LOG_FILE			"./serial.log"
#define BKUP_FILE		"./log.log.bak"
#define MAX_LOGSIZE		4194304		// 4 MB

unsigned long GetTickCount()
{
	static signed long long begin_time = 0;
	static signed long long now_time;
	struct timespec tp;
	unsigned long tmsec = 0;
	if ( clock_gettime(CLOCK_MONOTONIC, &tp) != -1 )
	{
		now_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
	}
	if ( begin_time = 0 )
		begin_time = now_time;
	tmsec = (unsigned long)(now_time - begin_time);
	return tmsec;
}

static const char* time_stamp()
{
	static char timestr[64];
#ifdef linux
	char *p = timestr;
	struct timeval tv;
	gettimeofday( &tv, NULL );

	strftime( p, sizeof(timestr), "%y/%m/%d %H:%M:%S", localtime( &tv.tv_sec ) );
	sprintf( p + strlen(p), ".%03lu", tv.tv_usec/1000 );

#else
		SYSTEMTIME	tnow;
		GetLocalTime( &tnow );
		sprintf( timestr, "%04d/%02d/%02d %02d:%02d:%02d.%03d", 
					tnow.wYear, tnow.wMonth, tnow.wDay, 
					tnow.wHour, tnow.wMinute, tnow.wSecond, tnow.wMilliseconds );
#endif
	return timestr;
}

static void trace_log(const char *fmt,...)
{
	va_list		va;
	char		str[1024 ] = "";
	char		file[128];
	FILE *fp;
	
	va_start(va, fmt);
	vsprintf(str, fmt, va);
	va_end(va);

	fp = fopen( LOG_FILE, "a" );
	if ( fp!=NULL && ftell(fp) > MAX_LOGSIZE )
	{
		fclose(fp);
		remove( BKUP_FILE );
		rename( LOG_FILE, BKUP_FILE );
		fp = fopen( LOG_FILE, "a" );
	}
	if ( fp != NULL )
	{
		if ( fmt[0] != '\t' )
			fprintf(fp, "[%s] %s", time_stamp(), str );
		else
			fprintf(fp, "%26s%s", " ", str+1 );
		fclose(fp);
	}
}

//#####################  End Of Logger ############################### 

typedef unsigned char BYTE;
typedef void (*pcallback)(int event);

//#typedef enum{
//#	FALSE = 0,
//#	TRUE = 1
//#}BOOL;

#define GPION_0 (1<<2)
#define GPION_1 (1<<3)
#define GPION_2 (1<<4)
#define GPION_3 (1<<5)
#define GPION_4 (1<<6)
#define GPION_5 (1<<7)
#define GPION_6 (1<<8)
#define GPION_7 (1<<9)

typedef struct NodeList serialNode;
struct NodeList{
	char name[8];

}












typedef struct {
	char head;
	BOOL b_run;
	pthread_t  h_Thread;
	pthread_mutex_t h_Mutex;
	pcallback pcbfuc;

}HMYOBJ,*PHMYOBJ;
HMYOBJ theObj;

// now the below code wiould the base code for the implement for the thread
// and all the code should be static mode .
// because these code should protect not show in the other appplication

static void *work_thread_fuc(void* p)
{
	PHMYOBJ myobj = (PHMYOBJ)p;
	BOOL b_Run = FALSE;
	theObj.b_run = b_Run;		
	for(;!theObj.b_run;)
	{
		// working thread code 
		// if want to exit then make theObj.run = true;
		// what you do should save in the struct myobj

		







	}

}


//    now below code will be the API Functions;
int API_fucnt()
{

}

int API_Init_Object( void *fmt)
{
	memset(&theObj,0,sizeof(theObj));
	// the other struct init code ...
	
//	pthread_mutexattr_destroy(&(theObj.h_Mutex));
	pthread_mutex_init(&(theObj.h_Mutex),NULL);
	pthread_create((pthread_t*)&(theObj.h_Thread),NULL,work_thread_fuc,&theObj);
}


#define STAT_EXIT TRUE

int API_TerminateOject()
{
     if(theObj.h_Thread)
     {
        theObj.b_run = STAT_EXIT;

        pthread_cancel(theObj.h_Thread);
        pthread_join((theObj.h_Thread),0);
     }
}

void API_setcallback( void *call_back_pointer)
{
	if(theObj.pcbfuc)
	{
		theObj.pcbfuc = call_back_pointer;
	}
}
















// ************************* for the Test Application **********************
#define event_1 1
#define event_2 2
#define event_3 3

void event_handle(int event)
{
	switch (event)
	{
		case event_1:
			break;
		case event_2:
			break;
		case event_3:
			break;
		default:
			break;
	}
}

#define Code1 0
#define Code2 1
#define Code3 2
void api_1(){};
void api_2(){};
void api_3(){};




int main(int argc, char *argv[])
{

	BOOL b_RUN = TRUE;
	char cv;
//	API_Init_Object( fmt ..);

	API_setcallback( event_handle );
	for(;b_RUN;)
	{
		if(1)				//... get the char you input )
			; // do some
			switch (cv)
			{
				case Code1: api_1();
					break;
				case Code2: api_2();
					break;
				case Code3: api_3();
					break;
				default:
					break;
			}

	}
	API_TerminateOject();
	return 0;
}



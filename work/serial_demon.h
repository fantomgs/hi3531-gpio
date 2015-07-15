/*
 * =====================================================================================
 *
 *       Filename:  serial_demon.h
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


#ifndef __SERIAL_THREAD_
#define __SERIAL_THREAD_

#ifdef __cplusplus 
extern "C" {
#endif

// int COM_API_INIT();
int GPIO_MonitorStart( void (*notice)(int, int), int period );
int ALB_Open();
int ALB_Close();

#ifdef __cplusplus 
}
#endif

#endif 



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

void GPIO_Init();
int GPIO_MonitorStart( void (*notice)(int, int), int period );
void ALB_Open();
void ALB_Close();
void Siren_On();
void Siren_Off();

void GPIO_MonitorStop();

#ifdef __cplusplus 
}
#endif

#endif 



/*
 * =====================================================================================
 *
 *       Filename:  platform_goio.h
 *
 *    Description:  与实现相关的底层板级控制模快
 *
 *        Version:  1.0
 *        Created:  2015-07-15 07:16:27 PM
 *       Revision:  none
 *       Compiler:  arm-hisiv100-gcc
 *
 *         Author:  (mike), 
 *        Company: 	RunWell
 *
 * =====================================================================================
 */

#ifndef __RUNWELL_PLATFORM__
#define __RUNWELL_PLATFORM__

#include "serial_demon.h"

int	gpio1_0_set_init(serialNode *m);
int gpio1_0_run_cmd(serialNode *m);
int gpio1_0_get_status(serialNode *m);
int gpio_1_0_set_on(serialNode *m);
int gpio_1_0_set_off(serialNode *m);
#endif 


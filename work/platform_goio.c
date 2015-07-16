/*
 * =====================================================================================
 *
 *       Filename:  platform_goio.c
 *
 *    Description:  与平台相关的端口状态获取模块
 *
 *        Version:  1.0
 *        Created:  2015-07-15 07:10:54 PM
 *       Revision:  none
 *       Compiler:  arm-hisiv100-gcc
 *
 *         Author:   (mike), 
 *        Company:  RunWell
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
#include "platform_goio.h"

// 假设端口1_0 为设置端口 
// 端口1_1 为扫描端口
int	gpio1_0_set_init(serialNode *m)
{
	// set mulit reg
	VOID *pMem = NULL;
	U32 ulOld,ulNew; 
	// set multi register
	pMem = memmap((0x200F0000+0xB8),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (0<<0); // set out put 
	*(U32*)pMem = ulNew;
	// set dir register
	pMem = memmap((0x20160000+0x400),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<0);
	*(U32*)pMem = ulNew;
	// set data register	
	pMem = memmap((0x20160000+0x400),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<2); // set data for 1
	*(U32*)pMem = ulNew;
	return 0;
}

int gpio1_0_run_cmd(serialNode *m)
{

	return 0;
}

// #define GPIO_MSG_1_1_ON		1
//#define GPIO_MSG_1_1_OFF	2
int gpio1_0_get_status(serialNode *m)
{
	U32 ulOld,ulNew;
	VOID *pMem;	
	pMem = memmap((0x200F0000+0xB8),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (0<<0); // set out put 
	int reg_old = m->reg_value;
	int reg_default = m->reg_default;
	if((reg_old & (1<<2)) ^ (ulOld & (1<<2)))  
	{
		if((reg_default & (1<<2)) ^ (ulOld & (1<<2)))  
		{
			if(m->cmd_type == CMD_RUN)
			{
				m->reg_value = ulOld;
				return 0;
			}
			return GPIO_MSG_1_1_ON; 
		}
		return GPIO_MSG_1_1_OFF;
	}
	return 0;
}



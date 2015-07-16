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

// 海思平台寄存器处理
// DAT寄存器对应的bit位为 Data[9..2]
//




#define GPIO_REG_FULL 0xffffffff
#define GPIO_DATA_N_X(x) (1<<(2+(x)))
#define GPIO_DIR_N_X(x) (1<<(x))

#define GPIO_REG_OFFSET(x) (1<<(x))
// ############################ GPIO_1_0 ####################################
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
	pMem = memmap((0x20160000+GPIO_DATA_N_X(0)),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | GPIO_DATA_N_X(0); // set data for 1
	*(U32*)pMem = ulNew;
	return 0;
}

int gpio1_0_run_cmd(serialNode *m)
{
	return 0;
}

void gpio1_0_set_on()
{

}

void gpio1_0_set_off()
{

}


//#
//# 监测端口返回值逻辑
// 这里我们使用宏来定义某个端口的默认值，使用初始化时读取的值具有一定的
// 不确定性，因为从端口读取的值有高低两种可能
#define GPIO_MSG_ORIGIN_BIT (1<<2)
int gpio1_0_get_status(serialNode *m)
{
	U32 ulOld,ulNew;
	VOID *pMem;	
	pMem = memmap((0x200F0000+0xB8),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (0<<0); // set out put 
	int reg_old = m->reg_value;
	int reg_default = GPIO_MSG_ORIGIN_BIT;
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



//########################## GPIO_1_1 ############################################

// 假设端口1_0 为设置端口 
// 端口1_1 为扫描端口
int	gpio1_1_set_init(serialNode *m)
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
	pMem = memmap((0x20160000+GPIO_DATA_N_X(0)),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | GPIO_DATA_N_X(0); // set data for 1
	*(U32*)pMem = ulNew;
	return 0;
}

int gpio1_1_run_cmd(serialNode *m)
{
	return 0;
}

int gpio1_1_set_on(serialNode *m)
{
	VOID *pMem = NULL;
	U32 ulOld,ulNew; 

	pMem = memmap((0x20160000+GPIO_DATA_N_X(0)),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<0);
	*(U32*)pMem = ulNew;

}

int gpio1_1_set_off(serialNode *m)
{
	VOID *pMem = NULL;
	U32 ulOld,ulNew; 

	pMem = memmap((0x20160000+0x400),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<0);
	*(U32*)pMem = ulNew;

}

//#
//# 监测端口返回值逻辑
// 这里我们使用宏来定义某个端口的默认值，使用初始化时读取的值具有一定的
// 不确定性，因为从端口读取的值有高低两种可能
#define GPIO_MSG_ORIGIN_BIT (1<<2)
int gpio1_1_get_status(serialNode *m)
{
	U32 ulOld,ulNew;
	VOID *pMem;	
	pMem = memmap((0x200F0000+0xB8),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (0<<0); // set out put 
	int reg_old = m->reg_value;
	int reg_default = GPIO_MSG_ORIGIN_BIT;
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



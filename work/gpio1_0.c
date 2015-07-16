/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : himc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/1
  Last Modified :
  Description   : HI Memory Modify
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

typedef struct{
	char name[128];
	U32 reg;
	U32 value;
	U32 len;
}RegInfo;

#define DEFAULT_LEN 16
static RegInfo reginfo[]={
	{
		.name = "Set Muilt Reg out",
		.reg  = 0x200F00B8,
		.value = 0x00, 
		.len = DEFAULT_LEN,
	},
	{
		.name = "Set Dir Register output",
		.reg  = 0x20160400,
		.value = 0xFF,
		.len = DEFAULT_LEN,
	},
	{
		.name = "light:",
		.reg  = 0x20160004,
		.value = 0x00,
		.len = DEFAULT_LEN,
	},
};
#define true	1
#define false	0

void signal_handle(int signo)
{
    if (SIGINT == signo || SIGTSTP == signo)
    {
       printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }
	 exit(0);
}


int main(int argc , char* argv[])
{
    U32 ulAddr = 0;
	U32 ulOld,uLNew;
   	VOID *pMem = NULL; 
	int i,j;

	signal(SIGINT, signal_handle);
	signal(SIGTERM, signal_handle);

	for(i=0; i<(sizeof(reginfo)/sizeof(RegInfo)); i++)
	{
		if(reginfo[i].reg > 0)
		{
			pMem = memmap(reginfo[i].reg,reginfo[i].len);
			ulOld = *(U32*)pMem;
			*(U32*)pMem = reginfo[i].value;
			ulOld = *(U32*)pMem;
			printf("Reg %x New value:%x\n",reginfo[i].reg,ulOld);
		}
	}
	int ok = true;	
	printf("Bgin to mamap memory\n");
	while(1)
	{
		if(reginfo[2].reg > 0)
		{
			pMem = memmap(reginfo[2].reg,reginfo[2].len);
			ulOld = *(U32*)pMem;
			if(ok){
				ok = false;
				*(U32*)pMem = 0xFF;
			}else{
				ok = true;
				*(U32*)pMem = 0x00;
			}
			printf("%s %s",reginfo[2].name,ok?"on\n":"off\n");
			ulOld = *(U32*)pMem;
			printf("Reg %x New value:%x\n",reginfo[i].reg,ulOld);
		}
		sleep(1);
	}	
    
    return 0;
}




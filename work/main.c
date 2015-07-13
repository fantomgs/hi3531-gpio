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
		.name = "GPIO1_0",
		.reg  = 0x200f00B8,
		.value = 1, 
		.len = DEFAULT_LEN,
	},
	{
		.name = "GPIO1_1",
		.reg  = 0x200f00BC,
		.value = 1,
		.len = DEFAULT_LEN,
	},
};

int main(int argc , char* argv[])
{
    U32 ulAddr = 0;
	U32 ulOld,uLNew;
   	VOID *pMem = NULL; 
	U32 i;
	for(i=0; i<(sizeof(reginfo)/sizeof(RegInfo)); i++)
	{
		if(reginfo[i].reg > 0)
		{
			pMem = memmap(reginfo[i].reg,reginfo[i].len);
			ulOld = *(U32*)pMem;
			printf("Reg %x value:%x\n",reginfo[i].reg,ulOld);
			*(U32*)pMem = reginfo[i].value;
			ulOld = *(U32*)pMem;
			printf("Reg %x New value:%x\n",reginfo[i].reg,ulOld);
		}
	}
    
    return 0;
}




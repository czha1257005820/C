/*
 ============================================================================
 Name        : main.c
 Author      : corvin.ke
 Version     :
 Copyright   : Your copyright notice
 Description : test
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>

#include"ProcessInfo.h"

// test program
int disp_pro_stat_info(char *procName)
{
	int pid= -1;
	float mempercent=0;
	float cpupercent=0;
	int handlenum=-1;

	if (NULL == procName)
	{
		printf("procName is NULL\n");
		return 0;
	}

	pid=get_PidByName(procName);
	if(pid > 0)
	{
		printf("ProcessName:%s\n",procName);
		printf("ProcessId:%d\n",pid);
	}
	else
	{
		printf("error:process %s not exit \n",procName);
		return 0;
	}

	mempercent = get_pmem(pid);
	if(mempercent<0)
	{
		printf("error:the mempercent \n");
	}
	else
	{
		printf("mempercent:%.6f\n",mempercent);
	}

	cpupercent = get_pcpu(pid);
	if(cpupercent<0)
	{
		printf("error:the cpupercent \n");
	}
	else
	{
		printf("cpupercent:%.6f\n",cpupercent);
	}

	handlenum = get_FdHandleNum(pid);
	if(cpupercent<0)
	{
		printf("error:the handlenum \n");
	}
	else
	{
		printf("handlenum:%d\n",handlenum);
	}
	return 0;
}


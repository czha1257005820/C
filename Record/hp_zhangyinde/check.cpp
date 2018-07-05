#include<stdio.h>
#include<stdlib.h>

void checkipclash(char* argv[])
{
	//传入参数为1，执行上下行网速检测
	if(1 == atoi(argv[1]))
	{
		system("ifstat -a");
	}
	//传入参数为2，执行ip冲突查询
	else if(2 == atoi(argv[1]))
	{
		system("arp -a");
	}
}

int main(int argc,char* argv[])
{
	if(argc > 1)
	{
		checkipclash(argv);
	}
}

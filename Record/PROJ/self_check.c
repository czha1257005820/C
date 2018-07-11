/*
    author: zhangzhuoya
*/

#include "util.h"
#include "stat_sys_cpu.h"
#include "stat_sys_disk.h"
#include "stat_net_conn.h"
#include "stat_net_check.h"
#include "stat_pro_info.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_NAME_LENT(x)    ((int)strlen(x) - 1)
#define CMD_NAME_MAX_LEN    10
#define MAX_PROCESS_NAME_LEN 256

typedef enum
{
    CMD_NONE,
    CMD_SYS,
    CMD_NET,
    CMD_PROC,
    CMD_STOP,
    CMD_HELP,
} CMD_ID_E;

typedef struct
{
    CMD_ID_E id;
    char    *name;
    void (*run)(void);
} CMD_ATTR_S;

static void input_dialog(void);
static void sys_menu_init();
static void disp_help_menu();
static void disp_invalid(void);
static void disp_sys_exit(void);

static CMD_ATTR_S s_CmdArr[] = 
{
    {CMD_NONE,  " ",       disp_invalid},
    {CMD_SYS,   "sysstat",  NULL},
    {CMD_NET,   "netstat",  NULL},
    {CMD_PROC,  "prostat",  NULL},
    {CMD_STOP,  "stop",     disp_sys_exit},
    {CMD_HELP,  "help",     disp_help_menu},
};


static void input_dialog(void)
{
    printf("\n");
    printf(">>>");
}

static void sys_menu_init()
{
    printf("This is System Self-Check Tool, plese choose the task you want: \n");
    printf("\n");
    printf("***************************************************************\n");
    printf("\n");
    printf("Input 'sysstat' to check system state\n");
    printf("Input 'netstat' to check network state\n");
    printf("Input 'prostat' to check process state\n");
    printf("Input 'stop' to quit system\n");
    printf("Input 'help' to get instructions\n");
    printf("***************************************************************\n");
    printf("\n");
    printf(">>>");
}

static void disp_help_menu()
{
    printf("Input 'sysstat' to check system state\n");
    printf("Input 'netstat' to check network state\n");
    printf("Input 'prostat' to check process state\n");
    printf("Input 'stop' to quit system\n");
    printf("Input 'help' to get instructions\n");
}

static void disp_invalid(void)
{
    printf("Invalid input, you can enter 'help' to get instructions.\n");
}

static void disp_sys_exit(void)
{
    printf("System exit sucess!\n");
    exit(0);
}

void disp_sys_stat(void)
{
    disp_sys_stat_cpu();
    disp_sys_stat_disk();
}

void disp_net_stat(void)
{
    disp_net_stat_conn();
    disp_net_stat_check();
}

void disp_pro_stat(void)
{
    char processName[MAX_PROCESS_NAME_LEN];
    
    printf("Please input Process Name:\n");
    scanf("%s", processName);
    disp_pro_stat_info(processName);
}

 
void sys_opt_run(char *cmd)
{
    int len = 0;
    int i = 0;

    if (NULL == cmd)
    {
        return ;
    }
    
    //printf("长度=%d\n", ARR_NUM(s_CmdArr)); 
    //sizeof(s_CmdArr)/sizeof(s_CmdArr[0])
    for (i=0; i<ARR_NUM(s_CmdArr); i++)
    {
        len = (int)strlen(s_CmdArr[i].name);
        
        if (0 == strncmp(cmd, s_CmdArr[i].name, len) && len > 0)
        {
            s_CmdArr[i].run();
            break;
        }
    }
    
    if (i == ARR_NUM(s_CmdArr))
    {
        s_CmdArr[CMD_NONE].run();
    }
}

void reg_check_fun(void)
{
    s_CmdArr[CMD_SYS].run = disp_sys_stat;
    s_CmdArr[CMD_NET].run = disp_net_stat;
    s_CmdArr[CMD_PROC].run = disp_pro_stat;
}


int main(void)
{
    char cmd[CMD_NAME_MAX_LEN]; 
    
    sys_menu_init();
    
    reg_check_fun();

    memset(cmd, 0, sizeof(cmd));
    
    for (;;)
    {
        if(NULL != fgets(cmd, sizeof(cmd), stdin) && cmd[0] != '\n')
        {
            sys_opt_run(cmd);            
            input_dialog();
        }
    }
    
    return 0;
}


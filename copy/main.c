#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include "debugtool.h"
#include "msgtool.h"
#include "multualvar.h"
#include "infoprint.h"


//命令处理
void handle_command(int argc, char* argv[]);

//模拟业务运行
void business_operation();

void* thread_listen_msg(void* arg)
{
    struct msg_st_2i_str data;
    while(1)
    {
        msg_rev_2i_str(msg_key, &data,getpid());

        if(data.extra == 0)//v-cmd-tty-dump命令的消息队列
        {
            if(data.extra2 == 0)
            {
                exec_command_tty_dump(data.buf);
            }
        }else if(data.extra == 1)//v-cmd-tty-recover命令消息队列
        {
            exec_command_tty_recover();
        }else if(data.extra == 2)//v-cmd-tty-multiple
        {
            exec_command_tty_multiple(data.extra2);
            
        }
    
    }
 
}

int main(int argc, char* argv[])
{
    
    signal(SIGINT, sigint_handler);
    //获取tty的路径
    init_self_tty_path();
    handle_command(argc, argv);
}


void handle_command(int argc, char* argv[])
{
    int type = detect_command(argv[0]);
    proc_type= type;
    if(argc == 2 && strcmp(argv[1], "-help") == 0)
    {
        branch_command_help(type);
        return;
    }

    if(type == -1)//第一次运行。运行业务逻辑、命令创建
    {
        create_commands();
        bussiness_msg_listen(thread_listen_msg);
        business_operation();
    }else if(type == 0)//v-cmd-tty-dump
    {
        //处理窗口关闭事件


        if(argc == 1) {
            branch_command_help(type);
            return;
        }
        int pid = parsePid(argv[1]);
        argc_pid = pid;
        if(pid == 0)
        {
            perror("pid有误");
            exit(1);
        }else
        {
            branch_command_tty_dump(msg_key, argc_pid);
        }
        
    }else if(type == 1)//v-cmd-tty-recover
    {
        if(argc == 1) {
            branch_command_help(type);
            return;
        }
        int pid = parsePid(argv[1]);
        argc_pid = pid;
        if(pid == 0)
        {
            perror("pid有误");
            exit(1);
        }else
        {
            branch_command_tty_recover(msg_key, pid);
        }
    }else if(type == 2)//v-cmd-tty-multiple
    {
        if(argc == 1) {
            branch_command_help(type);
            return;
        }
        int pid = parsePid(argv[1]);
        argc_pid = pid;
        if(pid == 0)
        {
            perror("pid有误");
            exit(1);
        }else
        {
            output_info("加载中...");
            branch_command_tty_mutiple(msg_key, pid);

            struct msg_st_2i_str data;
            msg_rev_2i_str(msg_key, &data, getpid());
            
            create_listen_client(data.extra2);
            output_info("加载完毕，将接收重定向信息");
        }
    }
}

void business_operation()
{
    while(1)
    {
        printf("calculating....\n");
        
        long long tmp = 0;
        for(long i=0;i<1000000;i++)
        {
            tmp += i;
        }
        printf("calculate completed.\n");    
    }
}
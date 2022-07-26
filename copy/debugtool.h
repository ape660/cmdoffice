#ifndef _DEBUGTOOL_H_
#define _DEBUGTOOL_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "msgtool.h"
#include "localserver.h"
#include "infoprint.h"



const size_t PATHNAME_MAX = 4096;//linux最大组合路径长度
const char* sys_path = "/bin/";//软链接根路径


const int commands_size = 3;//支持命令数


char* commands[] = 
{
    "v-cmd-tty-dump",
    "v-cmd-tty-recover",
    "v-cmd-tty-multiple"
};//支持命令列表

char* commands_function_overview[] = 
{
    "将原终端的调试信息重定向到当前终端(仅支持单终端)", 
    "将调试信息恢复到原终端",
    "使用tcp服务器将调试信息分发到各个终端"
};

char* commands_help[] = 
{
    "<pid>\n参数说明:\npid: 目标终端的进程号",
    "<pid>\n参数说明:\npid: 目标终端的进程号",
    "<pid>\n参数说明:\npid: 目标终端的进程号"
};

void init_self_tty_path();

//终端信号处理
void sigint_handler(int sig);

//创建命令
void create_commands();
//删除命令
void remove_commands();

//获取程序运行目录
void curent_dir(char *path);

//检测运行哪一个命令。-1表示第一次运行，否则表示索引
int detect_command(char* argv);

//解析pid，不合法则为0
int parsePid(char* param);

//command_tty_dump分支
void branch_command_tty_dump(int msg_key, int pid);
//执行command_tty_dump
void exec_command_tty_dump(char *tty_path);
//command_tty_dump的终端关闭了
void command_tty_dump_close(int msg_key, int pid);

//command_tty_dump分支
void branch_command_tty_recover(int msg_key, int pid);
//执行command_tty_dump
void exec_command_tty_recover();

//执行command_tty_multiple
void exec_command_tty_multiple(int pid);
////command_tty_multiple分支
void branch_command_tty_mutiple(int msg_key, int pid);

void init_self_tty_path()
{
    strcpy(self_tty_path, ttyname(1));
}


void sigint_handler(int sig)
{
    printf("sig");
    if(sig == SIGINT)
    {
        printf("sigint");
        if(proc_type == 2)
        {
            app_stopped =1;
        }
        else if(proc_type == -1)
        {
            printf("-1 close");
            close_tcp_server();
        }

    }
}

void branch_command_help(int i)
{
    printf("用法：%s %s\n", commands[i], commands_help[i]);
}


void create_commands()
{
    
    char sym_path[PATHNAME_MAX];
    char current_path[PATHNAME_MAX];
    curent_dir(current_path);
    for(int i=0;i<commands_size;i++)
    {

        strcpy(sym_path, sys_path);
        strcat(sym_path, commands[i]);
        
        if(symlink(current_path, sym_path)<0)
        {
            //软链接创建失败了
            if(errno == EEXIST)
            {//命令已存在
                if(remove(sym_path)<0)
                {
                    output_error("软链接创建失败");
                    exit(1);
                }
                if(symlink(current_path, sym_path)<0)
                {
                    output_error("软链接创建失败");
                    exit(1);
                }              
            }else{
                output_error("软链接创建失败");
                exit(1);
            }    
        }
    }
    output_info("软链接创建成功");

}

void remove_commands()
{
    char command_path[PATHNAME_MAX];

    for(int i=0;i<commands_size;i++)
    {
        strcpy(command_path, sys_path);
        strcat(command_path, commands[i]);
        int res = remove(command_path);
        if(res<0) perror("删除失败");
    }
}

void curent_dir(char *path)
{
    char str[PATHNAME_MAX];
    char buf[PATHNAME_MAX];

    snprintf(str, sizeof(str), "/proc/%d/exe", getpid ()); 

    readlink(str, buf, sizeof(str));
    strcpy(path, buf);
}

int detect_command(char* argv)
{
    
    int ret = -1;
    for(int i =0;i<commands_size;i++)
    {
        if(strcmp(argv, commands[i]) == 0)
        {
            //找到命令
            ret = i;
            break;
        }
    }
    return ret;
}

int parsePid(char* param)
{
    return strtol(param, NULL, 10);
}

void exec_command_tty_dump(char *tty_path)
{
    // 重定向
    int fd = open(tty_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){

        output_error("目标tty终端设备文件打开失败");
        exit(1);
    }
    if(strlen(self_tty_path) == 0)
    {
        strcpy(self_tty_path, ttyname(1));
    }
    output_info("信息将重定向");
    dup2(fd, STDOUT_FILENO); 
    output_info("信息重定向成功");
    
}

void branch_command_tty_dump(int msg_key, int pid)
{
    msg_send_2i_str(msg_key, pid, 0, 0, ttyname(1));
}

void exec_command_tty_recover()
{
    if(strlen(self_tty_path) == 0) return ;
    // 重定向
    int fd = open(self_tty_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){

        output_error("原终端设备文件打开失败");
        exit(1);
    }
    
    dup2(fd, STDOUT_FILENO); 
    output_info("信息输出已还原");
}

void branch_command_tty_recover(int msg_key, int pid)
{
    msg_send_2i_str(msg_key, pid, 1, 0, "");
}

void command_tty_dump_close(int msg_key, int pid)
{
    msg_send_2i_str(msg_key, pid, 0, 1, ttyname(1));
}

void branch_command_tty_mutiple(int msg_key, int pid)
{
    msg_send_2i_str(msg_key, pid, 2, getpid(), "");
}

void exec_command_tty_multiple(int pid)
{
    if(cur_port == -1)
    {
        build_distribution_server();
        output_info("信息将重定向");
    }
    sleep(1);
    dup2(first_socket, STDOUT_FILENO);
    
    msg_send_2i_str(msg_key, pid, 2, cur_port, "");
}


#endif
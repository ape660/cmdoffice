#ifndef _CMD_HANDLE_H_
#define _CMD_HANDLE_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "msgtool.h"


#define PATHNAME_MAX 4096 //linux最大组合路径长度
#define COMMAND_MAX 100 //最多支持命令数

typedef void (*serv_ptr)(struct document doc);
typedef int (*cmd_ptr)(int argc, char* argv[]);

extern int command_count;
extern char* commands[COMMAND_MAX];
extern serv_ptr sptrs[COMMAND_MAX];
extern cmd_ptr cptres[COMMAND_MAX];
extern cmd_ptr arg_detect_ptrs[COMMAND_MAX];


extern char app_tty_path[PATHNAME_MAX];
extern int app_proc_type;
extern int app_stoped; 
extern int app_pid;

//初始化应用信息
void app_init(char* name);

//获取当前应用名
void get_app_name(char* name);

//创建命令
int create_commands();
//删除命令
void remove_commands();

//获取程序运行目录
void get_current_dir(char *path);

//获取应用当前运行时间
void get_current_time(char *str_time);

//获取命令索引，如果是服务程序返回-1
int get_command_index(char* argv0);

//告知服务程序目前运行的命令
void tell_service_proc_what_cmd_is(int type, int argc, char* argv[]);

//监听消息队列
void listen_msg();

#endif
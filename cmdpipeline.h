#ifndef _CMD_PIPELINE_H_
#define _CMD_PIPELINE_H_
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "officeprint.h"

#define MAX_CMD 999 //最大命令数

typedef void (*cmd_ptr)(int argc, char* argv[]);

extern int cmd_counts;
extern cmd_ptr ptrs[MAX_CMD];
extern char* cmd_names[MAX_CMD];

//初始化应用信息
void app_info_init(char* name);

//在系统目录下创建命令
void create_commands();

//获取程序运行目录
void get_current_dir(char *path);

//获取应用当前运行时间
void get_current_time(char *str_time);

//获取当前应用名
void get_app_name(char* name);



#endif
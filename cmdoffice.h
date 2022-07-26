#ifndef _CMD_OFFICE_H_
#define _CMD_OFFICE_H_

#include "cmdpipeline.h"

//注册命令,失败返回-1
int office_register_command(char *cmd_name, cmd_ptr ptr);

//启动调试辅助工具，传入argv0
void office_work(char *argv0);






#endif
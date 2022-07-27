#ifndef _CMD_OFFICE_H_
#define _CMD_OFFICE_H_

#include <string.h>
#include <pthread.h>

#include "cmdhandle.h"
#include "msgtool.h"



//启动调试工具
int office_work(int argc, char* argv[]);

//注册命令
int office_register_cmd(char* cmd_name, serv_ptr sptr, cmd_ptr cptr);

//关闭调试工具
void office_quit();

//注册服务程序
int office_register_service(cmd_ptr service_ptr);


#endif
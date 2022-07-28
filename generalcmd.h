#ifndef _GENERAL_CMD_H_
#define _GENERAL_CMD_H_

#include <fcntl.h>

#include "cmdoffice.h"
#include "sockettool.h"
#include "msgtool.h"


//打开通用命令dump
void office_register_genneral_cmd_dump();

//通用命令recovery
void office_register_genneral_cmd_recovery();

//通用命令multiple
void office_register_genneral_cmd_multiple();

//重定向回来
extern void sptr_of_recovery(struct document doc);


#endif

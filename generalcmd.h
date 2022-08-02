#ifndef _GENERAL_CMD_H_
#define _GENERAL_CMD_H_

#include <fcntl.h>

#include "cmdoffice.h"
#include "sockettool.h"
#include "msgtool.h"
#include "officeio.h"




//打开通用命令dump
void office_register_genneral_cmd_dump();

//通用命令recovery
void office_register_genneral_cmd_recovery();

//通用命令multiple
void office_register_genneral_cmd_multiple();

//通用命令2file
void office_register_genneral_cmd_2file();

//重定回原来的tty
extern void sptr_of_recovery(struct document doc);


#endif

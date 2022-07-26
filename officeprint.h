#ifndef _OFFICE_PRINT_H_
#define _OFFICE_PRINT_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>

//输出正常信息
void office_info(char *info);

//输出错误，自动处理errno
void office_error(char *info);


#endif
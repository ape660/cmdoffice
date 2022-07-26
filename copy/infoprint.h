#ifndef _INFOPRINT_H_
#define _INFOPRINT_H_

#include <error.h>
#include <string.h>

const char info_prefix[] = "[Auxiliary Debugging Tool]";

const char* info_type[] = {"[info]", "[error]"};

//输出正常信息
void output_info(char *info)
{
    printf("\033[1;34;49m%s%s%s\n\033[0m", info_prefix, info_type[0], info);
}

//输出错误，自动处理errno
void output_error(char *info)
{
    printf("\033[1;31;49m%s%s%s，错误信息：%s\n\033[0m", info_prefix, info_type[1], info, strerror(errno));
}



#endif
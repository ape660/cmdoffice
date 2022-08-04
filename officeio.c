#include "officeio.h"

const char info_prefix[] = "[cmdo]";

const char* info_type[] = {"[info]", "[error]"};


void office_info(char *info)
{
    printf("\033[1;34;49m%s%s%s\n\033[0m", info_prefix, info_type[0], info);
}

void office_error(char* info)
{
    printf("\033[1;31;49m%s%s%s，error info：%s\n\033[0m", info_prefix, info_type[1], info, strerror(errno));
}
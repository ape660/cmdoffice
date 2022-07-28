#include <stdio.h>

#include "cmdoffice.h"
#include "generalcmd.h"


void service_ptr(int argc, char* argv[]);

int main(int argc, char* argv[])
{

    office_register_cmd("测试",NULL, NULL);

    office_register_service(service_ptr);
    
    office_register_genneral_cmd_dump();
    office_register_genneral_cmd_recovery();
    office_register_genneral_cmd_multiple();
    office_register_genneral_cmd_2file();
    
    office_work(argc, argv);
    
}


void service_ptr(int argc, char* argv[])
{
    while(1)
    {
        printf("\ncalculating....\n");
        
        long long tmp = 0;
        for(long i=0;i<1000000000;i++)
        {
            tmp += i;
        }
        printf("calculate completed.\n");    
    }
}
#include <stdio.h>

#include "cmdoffice.h"
#include "generalcmd.h"

void test_2_cptr(int argc, char* argv[]);

void service_ptr(int argc, char* argv[]);

int main(int argc, char* argv[])
{

    office_register_cmd("测试",NULL, NULL);

    office_register_cmd("测试2",NULL, test_2_cptr);

    office_register_service(service_ptr);
    
    office_register_genneral_cmd_dump();
    office_work(argc, argv);

    sleep(20);

    office_quit();
}


void test_2_cptr(int argc, char* argv[])
{
    printf("测试2运行了\n");
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
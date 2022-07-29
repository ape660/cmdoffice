#include <stdio.h>

#include "../cmdoffice.h"
#include "../generalcmd.h"


void service_ptr(int argc, char* argv[]);
void is_leap_sptr(struct document doc);
void is_leap_cptr(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    office_register_cmd("是不是闰年",is_leap_sptr, is_leap_cptr);
    office_register_service(service_ptr);
    office_work(argc, argv);    
}

void is_leap_sptr(struct document doc)
{
    int is_leap = 0, year = 0;
    char s_year[10];
    
    office_get_argvi(doc.argv, s_year, 1);//获取传过来的参数，年数参数在索引为1的位置
    year = atoi(s_year);//str转int
    
    if ((year % 4 == 0 && year % 100 !=0) || year % 400 == 0) {//判断是否为闰年的条件
        is_leap = 1;
    }

    //发送结果
    //doc.self_pid保存发送过来的进程的pid,以次用来指定接收端
    //结果可以放在type里，也可以放在argv里
    office_send_doccument(doc.self_pid, is_leap, 0, NULL);
}

void is_leap_cptr(int argc, char* argv[])
{
    //接受返回结果
    struct document doc;
    office_recv_document(&doc);

    if(doc.type == 1)
    {
        printf("%s 是闰年\n", argv[1]);
    }
    else
    {
        printf("%s 不是闰年\n", argv[1]);
    }
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
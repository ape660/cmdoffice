#include <stdio.h>

#include "../cmdoffice.h"
#include "../generalcmd.h"


void service_ptr(int argc, char* argv[]);
void is_leap_sptr(struct document doc);
int is_leap_cptr(int argc, char* argv[]);
void output_debug_info(struct document doc);

int detect_ptr_of_year(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    office_register_cmd("是不是闰年",is_leap_sptr,detect_ptr_of_year, is_leap_cptr);
    office_register_cmd("输出调试信息", output_debug_info, NULL, NULL);

    office_register_genneral_cmd_dump(); //通用命令dump
    office_register_genneral_cmd_recovery();//重定向恢复
    office_register_genneral_cmd_2file();
    office_register_genneral_cmd_multiple();

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

int is_leap_cptr(int argc, char* argv[])
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

int detect_ptr_of_year(int argc, char* argv[])
{
    if(argc  == 1) return -1;
    if(strspn(argv[1], "0123456789") == strlen(argv[1]))
    {
        return 0;
    }else
    {
        return -1;
    }
}

void output_debug_info(struct document doc)
{
    char* simulation_info[] = 
    {
        "Displaying notes found in: .note.ABI-tag", 
        "Owner                 Data size	Description",
        "GNU                  0x00000010	NT_GNU_ABI_TAG (ABI version tag)",
        "OS: Linux, ABI: 3.2.0",
        "",
        "Displaying notes found in: .note.gnu.build-id",
        "Owner                 Data size	Description",
        "GNU                  0x00000014	NT_GNU_BUILD_ID (unique build ID bitstring)",
        "Build ID: 536cc8d42fa3ed672abc427d4a683313fb902b6b"
    };
    int c = 0;
    while(1)
    {
        printf("%s\n", simulation_info[c]);
        c = (c+1)%9;
    }
}
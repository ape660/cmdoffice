#include "cmdpipeline.h"

#define PATHNAME_MAX 4096 //linux最大组合路径长度

//应用级变量
int app_pid;
char app_name[PATHNAME_MAX];
char app_tty_path[PATHNAME_MAX];

const char* cmd_prefix = "cmd-";//命令前缀
const char* sys_path = "/bin/";//软链接根路径
const char* divide = "-";//分隔符


int cmd_counts = 0;
char* cmd_names[MAX_CMD];
cmd_ptr ptrs[MAX_CMD];

int cmd_unexpectno = -1;
char* cmd_unexpect_info = 
{
    "创建软连接失败"
};

void app_info_init(char* name)
{
    app_pid = getpid();
    get_app_name(name);
    strcpy(app_tty_path, ttyname(1));
}

void create_commands()
{
    char cmd_path[PATHNAME_MAX];
    char current_path[PATHNAME_MAX];
    char current_datetime[20];
    get_current_dir(current_path);
    get_current_time(current_datetime);
    for(int i=0;i<cmd_counts;i++)
    {

        strcpy(cmd_path, sys_path);
        //加上前缀
        strcat(cmd_path, cmd_prefix);
        strcat(cmd_path, cmd_names[i]);
        strcat(cmd_path, divide);
        //加上程序名
        strcat(cmd_path, app_name);
        strcat(cmd_path, divide);
        //加上运行时间
        strcat(cmd_path, current_datetime);
        if(symlink(current_path, cmd_path)<0)
        {
            cmd_unexpectno = 0;
            office_error(cmd_path);
            break;
        }
    }
}



void get_current_dir(char *path)
{
    char str[PATHNAME_MAX];
    char buf[PATHNAME_MAX];

    snprintf(str, sizeof(str), "/proc/%d/exe", getpid ()); 

    readlink(str, buf, sizeof(str));
    strcpy(path, buf);
}


void get_current_time(char *str_time)
{
    time_t timep;
    time(&timep);

    struct tm *p;
    p = gmtime(&timep);

    snprintf(str_time, 20, "%d-%d-%d_%d:%d:%d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, 8 + p->tm_hour, p->tm_min, p->tm_sec);
}

void get_app_name(char* name)
{
    int index_of_slash =-1;
    for(int i = strlen(name); i>=0;i--)
    {
        if(name[i] == '/')
        {
            index_of_slash = i;
            break;
        }
    }

    index_of_slash ++;

    strcpy(app_name, name+index_of_slash);
}
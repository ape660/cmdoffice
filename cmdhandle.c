#include "cmdhandle.h"

const char* cmd_prefix = "cmd-";//命令前缀
const char* sys_path = "/bin/";//软链接根路径
const char* divide = "-";//分隔符，仅允许一个字符。这里是为了运算方便

int app_pid;
char app_name[PATHNAME_MAX];
char app_tty_path[PATHNAME_MAX];


int command_count = 0;
char* commands[COMMAND_MAX];
serv_ptr sptrs[COMMAND_MAX];
cmd_ptr cptres[COMMAND_MAX];
cmd_ptr app_service_ptr;
char* generated_cmd_path[PATHNAME_MAX];//实际生成的命令，用于删除

//监听队列
void* thread_listen_msg(void* argv);


void app_info_init(char* name)
{
    app_pid = getpid();
    get_app_name(name);
    strcpy(app_tty_path, ttyname(1));
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

int create_commands()
{
    char cmd_path[PATHNAME_MAX];
    char current_path[PATHNAME_MAX];
    char current_datetime[20];
    char pid[20];
    snprintf(pid, sizeof(pid), "%d", app_pid);
    get_current_dir(current_path);
    get_current_time(current_datetime);
    for(int i=0;i<command_count;i++)
    {
        
        strcpy(cmd_path, sys_path);
        //加上前缀
        strcat(cmd_path, cmd_prefix);
        strcat(cmd_path, commands[i]);
        strcat(cmd_path, divide);
        //加上程序名
        strcat(cmd_path, app_name);
        strcat(cmd_path, divide);
        //加上运行时间
        strcat(cmd_path, current_datetime);
        strcat(cmd_path, divide);
        //加上pid
        strcat(cmd_path, pid);
        if(symlink(current_path, cmd_path)<0)
        {
            return -1;
        }
        generated_cmd_path[i] = malloc(sizeof(char) * strlen(cmd_path) + 1);
        strcpy(generated_cmd_path[i], cmd_path);
    }
    return 0;
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

//删除命令
void remove_commands()
{
    for(int i=0;i<command_count;i++)
    {
        remove(generated_cmd_path[i]);
    }
}

int get_command_index(char* argv0)
{
    int pre_len = strlen(cmd_prefix);
    int n = -1;
    for(int i=pre_len;i<strlen(argv0);i++)//尝试寻找第一个分隔符
    {
        if(argv0[i] == divide[0]){
            n = i-pre_len;
            break;
        }
    }
    if(n<0) return -1;//如果没找到，则判定为服务程序
    for(int i=0;i<command_count;i++)
    {
        if(strncmp(argv0 + pre_len, commands[i] ,n) == 0)
        {
            return i;
        }
    }
    return -1;
}

void tell_service_proc_what_cmd_is(int type, int argc, char* argv[])
{
    int divide_index = 0;
    for(int i=strlen(argv[0]); i>=0; i--)
    {
        if(argv[0][i] == divide[0])
        {
            divide_index = i;
            break;
        }
    }
    int pid = atoi(argv[0] + divide_index +1);
    printf("pid: %d cmd_index: %d\n", pid, type);

    office_send_doccument(pid, type, app_pid, ttyname(1), argc, argv);
}

void* thread_listen_msg(void* argv)
{
    struct document doc;
    while(1)
    {
        
        int ret = office_recv_document(&doc);
        if(ret != -1)
        {
            
            if(sptrs[doc.cmd_type] != NULL)
            {
                sptrs[doc.cmd_type](doc);
            }
        }
    }
}
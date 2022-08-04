#include <signal.h>

#include "cmdhandle.h"

extern void office_quit();

const char* cmd_prefix = "cmdo-";//命令前缀
const char* sys_path = "/bin/";//软链接根路径
const char* divide = "-";//分隔符，仅允许一个字符。这里是为了运算方便

int app_pid;
char app_name[PATHNAME_MAX];
char app_tty_path[PATHNAME_MAX];
int app_proc_type = -1;//进程的类型
int app_stoped = 0;


int command_count = 0;
char* commands[COMMAND_MAX];
serv_ptr sptrs[COMMAND_MAX];
cmd_ptr cptres[COMMAND_MAX];

char* generated_cmd_path[PATHNAME_MAX];//实际生成的命令，用于删除


//监听队列
void* thread_listen_msg(void* argv);

//并发处理每一个命令
void* thread_deal_with_sptr(void* argv);

//处理ctrl + z信号
void handle_ctrl_c(int sig);

void handle_ctrl_c(int sig)
{
    if(sig == SIGINT)
    {
        app_stoped = 1;
    }
}


void app_init(char* name)
{
    app_pid = getpid();
    get_app_name(name);
    strcpy(app_tty_path, ttyname(1));
    signal(SIGINT, handle_ctrl_c);
    signal(SIGPIPE, SIG_IGN);//防止客户端退出，服务器就关闭
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
    char current_datetime[30];
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

    snprintf(str_time, 30, "%d_%d_%dh%dm%ds", 1 + p->tm_mon, p->tm_mday, 8 + p->tm_hour, p->tm_min, p->tm_sec);
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


    office_send_doccument(pid, type, argc, argv);
}

void listen_msg()
{
    struct document doc;
    while(1)
    {
        
        int ret = office_recv_document(&doc);
        
        if(ret != -1)
        {
            if(sptrs[doc.type] != NULL)
            {
                pthread_t sptr_thread;
                pthread_create(&sptr_thread, NULL, thread_deal_with_sptr, (void*)&doc);
            }
        }
        if(app_stoped) break;
    }
}

void* thread_deal_with_sptr(void* argv)
{
    struct document doc = *(struct document*)argv;
    if(doc.type>=0 && doc.type<command_count)
    {
        sptrs[doc.type](doc);
    }
}
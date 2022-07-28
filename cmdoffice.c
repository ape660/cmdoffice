#include "cmdoffice.h"

int office_work(int argc, char* argv[])
{
    //初始化应用信息
    app_init(argv[0]);
    int cmd_index = get_command_index(argv[0]);
    app_proc_type = cmd_index;
    if(cmd_index == -1)//服务程序
    {
        printf("service proc\n");
        //创建命令
        create_commands();
        //监听消息队列
        pthread_t msg_thread;
        int ret = pthread_create(&msg_thread, NULL, thread_listen_msg, NULL);
        if(ret != 0) return -1;

        if(app_service_ptr != NULL)//运行服务程序
        {
            app_service_ptr(argc, argv);
        }

    }else
    {
        printf("command proc\n");
        tell_service_proc_what_cmd_is(cmd_index, argc, argv);
        if(cptres[cmd_index] != NULL)
        {
            cptres[cmd_index](argc, argv);
        }
    }
    return 0;
}

int office_register_cmd(char* cmd_name, serv_ptr sptr, cmd_ptr cptr)
{
    
    if(command_count == COMMAND_MAX){
        return -1;
    }
    commands[command_count]= cmd_name;
    sptrs[command_count] = sptr;
    cptres[command_count++] = cptr;
    
    return 0;
}

int office_register_service(cmd_ptr service_ptr)
{
    app_service_ptr = service_ptr;
}

void office_quit()
{
    remove_commands();
    close_distribution_server();
}
#include "cmdoffice.h"

extern void register_general_cmd_list();
extern void register_general_cmd_shutdown();

int office_work(int argc, char* argv[])
{
    //初始化应用信息
    app_init(argv[0]);

    //注册默认命令
    register_general_cmd_list();//列表命令
    register_general_cmd_shutdown();//关闭命令

    int cmd_index = get_command_index(argv[0]);
    app_proc_type = cmd_index;
    if(cmd_index == -1)//服务程序
    {

        //创建命令
        if(create_commands()< 0)
        {
            office_error("Failed to create symbolic links.");
            return -1;
        }else
        {
            office_info("Succeeded in creating symbolic links.");
        }
        //监听消息队列
        listen_msg();
        //清除资源
        office_quit();
    }else
    {
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



void office_quit()
{
    remove_commands();
    close_distribution_server();

    office_info("Resources have been released.");
}
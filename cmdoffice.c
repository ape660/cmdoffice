#include "cmdoffice.h"

int office_work(int argc, char* argv[])
{
    //初始化应用信息
    app_init(argv[0]);
    int cmd_index = get_command_index(argv[0]);
    app_proc_type = cmd_index;
    if(cmd_index == -1)//服务程序
    {

        //创建命令
        if(create_commands()< 0)
        {
            office_error("创建命令软连接失败");
            return -1;
        }else
        {
            office_info("创建命令软链接成功");
        }
        //监听消息队列
        listen_msg();

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
}
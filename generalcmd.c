#include "generalcmd.h"

char* general_cmd_dump = "dump";
char* general_cmd_recovery = "recovery";
char* general_cmd_multiple = "multiple";
char* general_cmd_2file = "2file";
char* general_cmd_list = "list";

void sptr_of_dump(struct document doc);
void sptr_of_recovery(struct document doc);
void sptr_of_2file(struct document doc);

void sptr_of_multiple(struct document doc);
void cptr_of_multiple(int argc, char* argv[]);

void cptr_of_list(int argc, char* argv[]);

void office_register_genneral_cmd_list()
{
    office_register_cmd(general_cmd_list, NULL, cptr_of_list);
}

void office_register_genneral_cmd_dump()
{
    office_register_cmd(general_cmd_dump, sptr_of_dump, NULL);
}

void sptr_of_dump(struct document doc)
{
    char tty_path[PATHNAME_MAX];
    strcpy(tty_path, doc.tty_path);

    // 重定向
    int fd = open(tty_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){
        return ;
    }
    office_info("The information output is redirected.");
    dup2(fd, STDOUT_FILENO); 
    office_info("The information is redirected here.");
}

void office_register_genneral_cmd_recovery()
{
    office_register_cmd(general_cmd_recovery, sptr_of_recovery, NULL);
}

void sptr_of_recovery(struct document doc){
    // 重定向回来
    int fd = open(app_tty_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){

        return ;
    }
    office_info("Information output recovery.");
    dup2(fd, STDOUT_FILENO); 
    office_info("Message output has been resumed.");
}


void office_register_genneral_cmd_multiple()
{
    office_register_cmd(general_cmd_multiple, sptr_of_multiple, cptr_of_multiple);
}

void sptr_of_multiple(struct document doc)
{
    int pid = doc.self_pid;
    if(cur_port == -1)
    {
        int ret  = build_distribution_server();    
        if(ret <0)
        {
            office_error("Failed to set up the distributor.");
        }
        
    }
    
    sleep(1);

    dup2(first_socket, 1);

    //发送端口给连接上的客户端
    office_send_doccument(pid, cur_port, 0, NULL);  


}

void cptr_of_multiple(int argc, char* argv[])
{
    struct document doc;
    office_info("Loading...");
    office_recv_document(&doc);

    int client_fd  = create_tcp_client(doc.type);

    char buf[BUFSIZ];//定义一个数组用来存储接收到的数据 
    int ret; 
    while (1) 
    { 
        memset(buf, 0, sizeof(buf)); 
        ret = read(client_fd, buf, sizeof(buf)); 
        if (0 > ret) 
        { 
            break; 
        }//执行while循环读取数据，当 
        else if (0 == ret) 
        { 
            break; 
        } 
        printf("%s", buf);
    }

    close(client_fd); 
}

void office_register_genneral_cmd_2file()
{
    office_register_cmd(general_cmd_2file, sptr_of_2file, NULL);
}

void sptr_of_2file(struct document doc)
{

    char file_path[PATHNAME_MAX];
    office_get_argvi(doc.argv, file_path, 1);

    int fd = open(file_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){
        perror("");
        return ;
    }
    office_info("The information is redirected to the file.");
    dup2(fd, STDOUT_FILENO); 
}

void cptr_of_list(int argc, char* argv[])
{
    printf("Supported Commands As Follow\n");
    for(int i=0;i<command_count;i++)
    {
        printf("%d. %s\n", i+1, commands[i]);

    }
}
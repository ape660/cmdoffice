#include "generalcmd.h"

char* general_cmd_dump = "dump";
char* general_cmd_recovery = "recovery";
char* general_cmd_multiple = "multiple";
char* general_cmd_2file = "2file";

void sptr_of_dump(struct document doc);
void sptr_of_recovery(struct document doc);
void sptr_of_multiple(struct document doc);
void sptr_of_2file(struct document doc);

void cptr_of_multiple(int argc, char* argv[]);


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

    dup2(fd, STDOUT_FILENO); 

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
    dup2(fd, STDOUT_FILENO); 
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
        build_distribution_server();    
    }
    
    sleep(1);
    dup2(first_socket, STDOUT_FILENO);
    
    //发送端口给连接上的客户端
    office_send_doccument(pid, cur_port, 0, "",0 , NULL);  
}

void cptr_of_multiple(int argc, char* argv[])
{
    struct document doc;
    office_recv_document(&doc);
    int client_fd  = create_tcp_client(doc.cmd_type);
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
    printf("get path: %s", file_path);
    int fd = open(file_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){
        perror("");
        return ;
    }
    dup2(fd, STDOUT_FILENO); 
}

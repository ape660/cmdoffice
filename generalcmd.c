#include "generalcmd.h"

char* general_cmd_dump = "dump";
char* general_cmd_recovery = "recovery";

void implement_of_dump(struct document doc);
void implement_of_recovery(struct document doc);


void office_register_genneral_cmd_dump()
{
    office_register_cmd(general_cmd_dump, implement_of_dump, NULL);
}


void implement_of_dump(struct document doc)
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
    office_register_cmd(general_cmd_recovery, implement_of_recovery, NULL);
}

void implement_of_recovery(struct document doc){
    // 重定向回来
    int fd = open(app_tty_path, O_WRONLY|O_CREAT, 00644);
    
    if(fd < 0){

        return ;
    }
    
    dup2(fd, STDOUT_FILENO); 
}
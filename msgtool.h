#ifndef _MSG_TOOL_H_
#define _MSG_TOOL_H_

#define TTY_PATH_SIZE 20
#define ARGV_SIZE 20 //每条命令最大支持参数个数
#define MSGMAX 8192 //ubuntu默认消息队列最大长度


#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern const int msg_key;

struct document
{
    long int msg_type;

    int cmd_type;
    int self_pid;
    char tty_path[TTY_PATH_SIZE];
    char argv[MSGMAX - 2*sizeof(int) - TTY_PATH_SIZE];
};

//用消息队列发送一个document
int office_send_doccument
(
    int pid, int type, int self_pid, char tty[TTY_PATH_SIZE], int argc, char* argv[ARGV_SIZE]
);

//接收消息队列中的消息
int office_recv_document(struct document *doc);

//由于参数被合成为空格连接的数组，因此需要将某个参数提取出来
void office_get_argvi(char *argv, char* rec, int index);


#endif
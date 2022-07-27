#include "msgtool.h"

const int msg_key =2233;

int office_send_doccument
(
    int pid, int type, int self_pid, char tty[TTY_PATH_SIZE], int argc, char* argv[ARGV_SIZE]
)
{
    //访问消息队列（不存在则创建）
	int msgid = msgget(msg_key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		return -1;
	}

    //打包数据	
    struct document doc;
    doc.msg_type = pid;//指定目标程序   
    doc.cmd_type = type;
    doc.self_pid = self_pid;
    strcpy(doc.tty_path, ttyname(1));
    
    for(int i=0;i<ARGV_SIZE && i<argc ;i++)
    {
        doc.argv[i] = malloc(strlen(argv[i])+1);
        strcpy(doc.argv[i], argv[i]);
    }

    //向队列发送数据
    if(msgsnd(msgid, (void*)&doc, sizeof(doc), 0) == -1)
    {
        perror("");
        return -1;
    }

    return 0;
}

int office_recv_document(struct document *doc)
{
    //访问消息队列
	int msgid = msgget(msg_key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		return -1;
	}

    //从队列中获取消息
    if(msgrcv(msgid, (void*)doc, MSGMAX, getpid(), 0) == -1)
    {
        perror("");
        return -1;
    }
    
    return 0;
}
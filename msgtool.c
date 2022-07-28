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
    strcpy(doc.tty_path, tty);
    for(int i=0;i<argc ;i++)
    {
        strcat(doc.argv, argv[i]);
        if(i!= argc-1)
        {
            strcat(doc.argv, " ");
        }
    }
    printf("send: %s", doc.argv);
    //向队列发送数据
    if(msgsnd(msgid, (void*)&doc, MSGMAX, 0) == -1)
    {
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

void office_get_argvi(char *argv, char* rec, int index)
{
    int len = strlen(argv);
    int l = 0, r= -1;
    int c = 0;
    //寻找l

    if(index != 0)
    {
        for(int i=0; i<len; i++)
        {
            if(argv[i] == ' ')
            {
                c ++ ;
                if(c == index)
                {
                    l = i+1;
                    break;
                }
            }
        }
    }

    //寻找r
    for(int i=l;i<len;i++)
    {
        if(argv[i] == ' ')
        {
            r = i;
            break;
        }
    }

    if(r<0) r =len;
    printf("l: %d, r:%d\n", l, r);

    strncpy(rec, argv+l, r-l);

    rec[r-l] = '\0';
}
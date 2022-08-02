#include "msgtool.h"

const int msg_key =2233;

int office_send_doccument(int pid, int type,  int argc, char* argv[])
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

    doc.type = type;

    doc.self_pid = getpid();

    
    strcpy(doc.tty_path, app_tty_path);


    fflush(stderr);
    for(int i=0;i<argc ;i++)
    {
        strcat(doc.argv, argv[i]);
        if(i!= argc-1)
        {
            strcat(doc.argv, " ");
        }
    }


    fflush(stderr);
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

    strncpy(rec, argv+l, r-l);

    rec[r-l] = '\0';
}
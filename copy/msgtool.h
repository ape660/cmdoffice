#ifndef _MSGTOOL_H_
#define _MSGTOOL_H_

#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/*
消息结构体约定：
_2i_str表示除了固定的msg_type外，消息体含2个整型、1个字符数组。

i:整型
str:字符数组

*/
//消息的结构体
struct msg_st_2i_str
{
	long int msg_type;
    int extra;
    int extra2; 
	char buf[BUFSIZ];
    
};

//向msg_key消息队列中发送一条含2个整型，1个字符数组的消息
void msg_send_2i_str(int msg_key, long int type, int ex, int ex2, char *buffer);

//从msg_key消息队列中接收一条含1个整型，1个字符数组的消息
void msg_rev_2i_str(int msg_key,struct msg_st_2i_str *data, long int msgtype);

//第一次运行程序的监听消息队列线程
void bussiness_msg_listen(void *(*thread_listen_msg) (void *));

void msg_send_2i_str(int msg_key, long int type, int ex,int ex2, char *buffer)
{
    //访问消息队列（不存在则创建）
	int msgid = msgget(msg_key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		perror("消息队列创建或访问失败.\n");
		exit(1);
	}

    //打包数据	
    struct msg_st_2i_str data;
    data.msg_type = type;   
    data.extra = ex; 
    data.extra2 = ex2;
    strcpy(data.buf, buffer);
    
    //向队列发送数据
    if(msgsnd(msgid, (void*)&data, BUFSIZ, 0) == -1)
    {
        perror("消息队列发送消息失败.\n");
        exit(1);
    }
}

void msg_rev_2i_str(int msg_key, struct msg_st_2i_str *data, long int msgtype)
{
    //访问消息队列
	int msgid = msgget(msg_key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		perror("消息队列创建失败");
		exit(1);
	}

    //从队列中获取消息
    if(msgrcv(msgid, (void*)data, BUFSIZ, msgtype, 0) == -1)
    {
        perror("消息队列消息接收失败");
        exit(1);
    }
}

void bussiness_msg_listen(void *(*thread_listen_msg) (void *))
{
    //创建一个子线程监听消息队列
    pthread_t pid;
    int ret = pthread_create(&pid,NULL,thread_listen_msg,NULL);
    if(ret != 0)
    {
        perror("线程创建失败");
        return ;
    }
}



#endif
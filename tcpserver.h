#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unistd.h>
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/mman.h>

#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <netinet/tcp.h>
#include <semaphore.h>

//第一个客户端转发数据
void* thread_forwarding_data(void *param);

//服务器监听连接
void * thread_server_listen(void *param);

//获取本地可用端口号，返回0,则分配端口号失败
int get_local_port();

//设置缓冲立即输出
void imediate_flush_buffer(int clientfd);



#endif
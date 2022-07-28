#ifndef _SOCKET_TOOL_H_
#define _SOCKET_TOOL_H_

#include <netinet/tcp.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "msgtool.h"

extern int cur_port;
extern int first_socket;

//建立分发服务器
void build_distribution_server();

//关闭分发服务器，自己判断宣布需要关
void close_distribution_server();

//创建客户端
int create_tcp_client(int port);

#endif
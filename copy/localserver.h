#ifndef _LOCALSERVER_H_
#define _LOCALSERVER_H_

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
#include "multualvar.h"
#include "infoprint.h"

#define MAX_SOCKETS 60000


extern void exec_command_tty_recover();
int app_stopped = 0;

int cur_port = -1;
int server_socket = -1;
int first_socket = -1;

int h_sockets[MAX_SOCKETS];
int h_sockets_size = 0;

//获取一个可用端口
int get_available_port();
//建立一个tcp服务器
void build_tcp_server();
//服务器监听连接
void * server_listen(void *param);
//监听每一个连接的数据传送
void* handle_data(void *param);
//某个连接断开
void socket_disconnect(int socket_fd);
//关闭服务器
void close_tcp_server();
//设置立即输出
void cancel_buffer(int clientfd);
//创建一个客户端，返回sockfd
int create_tcp_client();
//创建一个监听服务器发送的信息的client
void create_listen_client();
//创建一个分发服务器，高级封装接口
void build_distribution_server();

void* handle_data(void *param)
{
    int connfd = *(int*)param;
    char buf[BUFSIZ];//定义一个数组用来存储接收到的数据 
    int ret; 
    while (1) 
    { 
        memset(buf, 0, sizeof(buf)); 
        //read block
        ret = read(connfd, buf, sizeof(buf)); 
        if (0 > ret) 
        { 
            perror("read error"); 
            break; 
        }//执行while循环读取数据，当 
        else if (0 == ret) 
        { 
            
            socket_disconnect(connfd);
            break;
        }else{
            
            for(int i=1;i<h_sockets_size;i++)
            {
                write(h_sockets[i], buf, sizeof(buf));
            }
        }
            
    } 
}


void * server_listen(void *param)
{
    int sockfd = *(int*)param;
    while(1)
    {
        int connfd = accept(sockfd, NULL, NULL);//使用accept从消息队列中获取请求 
        if (connfd < 0) 
        { 
            output_error("服务器accept失败"); 
            exit(1);
        } 
        h_sockets[h_sockets_size] = connfd;
        h_sockets_size = h_sockets_size + 1;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_data, (void *)&connfd);
           
    }
}

void socket_disconnect(int connfd)
{
    int index = -1;
    for(int i=0;i<h_sockets_size;i++)
    {
        if(h_sockets[i] == connfd) index = i;
    }
    if(index != -1)
    {
        close(h_sockets[index]);
        for(int i=index+1;i<h_sockets_size;i++)
        {
            h_sockets[i-1] = h_sockets[i];
        }
        h_sockets_size--;
        //如果只剩一个客户端（第一个客户端），则重定向回原tty
        if(h_sockets_size == 1)
        {
            exec_command_tty_recover();
        }     
        
    } 
}

void close_tcp_server()
{
    if(server_socket != -1)
    {
        for(int i=0;i<h_sockets_size;i++)
        {
            close(h_sockets[i]);
        }
        close(server_socket);
        h_sockets_size = 0;
        cur_port = -1;
        server_socket = -1;
        perror("close server finished");
    }
}


void build_tcp_server()
{
    cur_port = get_available_port();
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字 
    server_socket = server_fd;
    if (server_fd < 0) 
    { 
        output_error("服务器创建失败"); 
        exit(1); 
    } //创建失败的错误处理 

    struct sockaddr_in myaddr; //创建“我的地址”结构体 
    memset(&myaddr, 0, sizeof(myaddr)); //对内存清零（保险起见） 
    myaddr.sin_family       = AF_INET; //选择IPV4地址类型 
    myaddr.sin_port         = htons(cur_port); //选择端口号 
    myaddr.sin_addr.s_addr  = INADDR_ANY;; //选择IP地址 

    if (0 > bind(server_fd, (struct sockaddr*)&myaddr, sizeof(myaddr)))//绑定套接字 
    { 
        output_error("服务器bind失败"); 
        exit(1); 
    } 


    if (0 > listen(server_fd, 8))//调用listen对指定端口进行监听 
    { 
        output_error("服务器listen失败"); 
        exit(1); 
    } 

    //服务器监听线程
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_listen, (void *)&server_fd);
    
    sleep(1); //让服务器监听线程运行
}


int get_available_port() {
    unsigned short port = 0;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return port;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = 0;        // 若port指定为0,则调用bind时，系统会为其指定一个可用的端口号
    int ret = bind(sock, (struct sockaddr *) &addr, sizeof addr);    // 3. 绑定
    do {
        if (0 != ret) {// 4. 利用getsockname获取
            //LOGE("upn bind, %d, %s", errno, strerror(errno))
            break;
        }
        struct sockaddr_in sockaddr;
        int len = sizeof(sockaddr);
        ret = getsockname(sock, (struct sockaddr *) &sockaddr, (socklen_t *) &len); // this function now not be hooked

        if (0 != ret) {
           // LOGE("upn, getsockname:%d, %s", errno, strerror(errno))
            break;
        }
        port = ntohs(sockaddr.sin_port); // 获取端口号
    } while (0);
    //LOGD("UpnConfig::port::%d", port)
    close(sock);

    return port;
}

int create_tcp_client(int port)
{
    int clientfd; 
    if (0 > (clientfd = socket(AF_INET, SOCK_STREAM, 0))) 
    { 
        output_error("套接字创建失败"); 
        exit(1);
    } 

    struct sockaddr_in srv_addr; 
    memset(&srv_addr, 0, sizeof(srv_addr)); 
    srv_addr.sin_family         = AF_INET; 
    srv_addr.sin_port           = htons(port); 
    srv_addr.sin_addr.s_addr    = inet_addr("127.0.0.1"); 
    if (0 > connect(clientfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr))) 
    { 
        output_error("套接字connect失败"); 
        exit(1);
    } 
    return clientfd;
}

void create_listen_client(int port)
{
    int client_fd  = create_tcp_client(port);
    char buf[BUFSIZ];//定义一个数组用来存储接收到的数据 
    int ret; 
    while (1) 
    { 
        memset(buf, 0, sizeof(buf)); 
        ret = read(client_fd, buf, sizeof(buf)); 
        if (0 > ret) 
        { 
            output_error("读失败");
            break; 
        }//执行while循环读取数据，当 
        else if (0 == ret) 
        { 
            output_info("关闭成功");
            break; 
        } 
        printf("%s", buf);
        if(app_stopped == 1) break;
    }

    close(client_fd); 
}

void cancel_buffer(int clientfd)
{
    int opt = 1;
    int status = setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    setvbuf(stdout, NULL, _IONBF, 0);
}

void build_distribution_server()
{
    build_tcp_server();

    //创建第一个客户端
    int clientfd = create_tcp_client(cur_port);
    first_socket = clientfd;
    
    cancel_buffer(clientfd);
    
    
}

#endif
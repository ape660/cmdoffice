#include "tcpserver.h"

#define MAX_SOCKETS 3977 //客户端最大支持连接数 

int server_started = 0; //记录服务器是否开启的变量
int h_sockets[MAX_SOCKETS]; //记录目前连接的客户端
int h_sockets_size ; //记录目前连接数

int unexpectno = -1; //是否发生了预期之外的错误
char* unexpect_info = 
{
    "客户端超过了最大连接数，拒绝连接", "分配端口号失败"
};

sem_t sem; //信号量，


void* thread_forwarding_data(void *param)
{
    int connfd = *(int*)param;
    char buf[BUFSIZ]; 
    int ret; 
    while (1) 
    { 
        memset(buf, 0, sizeof(buf)); 
        
        ret = read(connfd, buf, sizeof(buf)); 
        if (0 > ret) 
        { 
            break; 
        } 
        else if (0 == ret)//客户端退出 
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

void * thread_server_listen(void *param)
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
        pthread_create(&thread, NULL, thread_forwarding_data, (void *)&connfd);
        
        if(h_sockets_size == MAX_SOCKETS) unexpectno = 0;

        sem_wait(&sem) ;  
    }
}

int get_local_port()
{
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
            
            break;
        }
        struct sockaddr_in sockaddr;
        int len = sizeof(sockaddr);
        ret = getsockname(sock, (struct sockaddr *) &sockaddr, (socklen_t *) &len); 

        if (0 != ret) {
           
            break;
        }
        port = ntohs(sockaddr.sin_port); // 获取端口号
    } while (0);
    
    close(sock);

    return port;
}

void imediate_flush_buffer(int clientfd)
{
    int opt = 1;
    int status = setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    setvbuf(stdout, NULL, _IONBF, 0);
}










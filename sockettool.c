#include "sockettool.h"

#define MAX_SOCKETS 200//最大连接数为200

extern void sptr_of_recovery(struct document doc);


int h_sockets[MAX_SOCKETS];
int h_sockets_size = 0;

int cur_port = -1;
int server_socket = -1;
int first_socket = -1;

//获取一个可用端口
int get_available_port();

//设置立即输出
void cancel_buffer(int clientfd);

//建立一个tcp服务器
int build_tcp_server();

//服务器监听连接
void * thread_server_listen(void *param);

//服务器收发数据
void* thread_handle_data(void *param);

//客户端断开连接
void socket_disconnect(int connfd);

void* thread_handle_data(void *param)
{
    int connfd = *(int*)param;
    char buf[BUFSIZ];//定义一个数组用来存储接收到的数据 
    int ret; 
    while (1) //执行while循环读取数据，当 
    { 
        memset(buf, 0, sizeof(buf)); 
        //read block
        ret = read(connfd, buf, sizeof(buf)); 
        if (0 > ret) 
        { 
            break; 
        }
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

void * thread_server_listen(void *param)
{
    int sockfd = *(int*)param;
    while(1)
    {
        int connfd = accept(sockfd, NULL, NULL);//使用accept从消息队列中获取请求 
        if (connfd < 0) 
        { 
            return NULL;
        } 
        h_sockets[h_sockets_size] = connfd;
        h_sockets_size = h_sockets_size + 1;

        pthread_t thread;
        pthread_create(&thread, NULL, thread_handle_data, (void *)&connfd);

    }
}

int build_tcp_server()
{
    cur_port = get_available_port();
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字 
    server_socket = server_fd;
    if (server_fd < 0) 
    { 
        return -1; 
    } //创建失败的错误处理 

    struct sockaddr_in myaddr; //创建“我的地址”结构体 
    memset(&myaddr, 0, sizeof(myaddr)); //对内存清零（保险起见） 
    myaddr.sin_family       = AF_INET; //选择IPV4地址类型 
    myaddr.sin_port         = htons(cur_port); //选择端口号 
    myaddr.sin_addr.s_addr  = INADDR_ANY;; //选择IP地址 

    if (0 > bind(server_fd, (struct sockaddr*)&myaddr, sizeof(myaddr)))//绑定套接字 
    { 
        return -1;
    } 


    if (0 > listen(server_fd, 8))//调用listen对指定端口进行监听 
    { 
        return -1;
    } 

    //服务器监听线程
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, thread_server_listen, (void *)&server_fd);
    
    sleep(1); //让服务器监听线程运行

    return 0;
}

int get_available_port()
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

void cancel_buffer(int clientfd)
{
    int opt = 1;
    int status = setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    setvbuf(stdout, NULL, _IONBF, 0);
}

int build_distribution_server()
{
    int ret = build_tcp_server();
    if(ret < 0)
    {
        return -1;
    }
    
    //创建第一个客户端
    int clientfd = create_tcp_client(cur_port);

    if(clientfd <0 )
    {
        return -1;
    }
    first_socket = clientfd;
    office_info("Setting up the distributor succeeded.");
    cancel_buffer(clientfd);
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
            struct document doc; //仅传递参数需要
            sptr_of_recovery(doc);
        }     
    } 
}

int create_tcp_client(int port)
{
    int clientfd; 
    if (0 > (clientfd = socket(AF_INET, SOCK_STREAM, 0))) 
    { 
        return -1;
    } 

    struct sockaddr_in srv_addr; 
    memset(&srv_addr, 0, sizeof(srv_addr)); 
    srv_addr.sin_family         = AF_INET; 
    srv_addr.sin_port           = htons(port); 
    srv_addr.sin_addr.s_addr    = inet_addr("127.0.0.1"); 
    if (0 > connect(clientfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr))) 
    { 
        return -1;
    } 
    return clientfd;
}

void close_distribution_server()
{
    if(cur_port != -1)
    {
        for(int i=0;i<h_sockets_size;i++)
        {
            shutdown(h_sockets[i], SHUT_RDWR);
        }
        shutdown(server_socket, SHUT_RDWR);
        h_sockets_size = 0;
        cur_port = -1;
        server_socket = -1;
    }
}
# cmdoffice
命令行辅助调试工具

## 介绍
cmdoffice(Command Office)是一个由c语言编写的命令行辅助调试工具，它支持：
+ 实现shell命令行对某个进程信息查看、控制等操作
+ 提供通用命令：单客户端重定向dump、重定向恢复recoery、多客户端重定向multiple、文件重定向2file
+ 支持注册自定义命令
+ 允许服务进程同时运行
## 应用截图
在系统/bin目录下生成的命令<br>
![image](https://user-images.githubusercontent.com/48158080/181439230-aea1bf94-3a0b-468b-9ee2-ad3a389d6295.png)


## 运行环境
编程语言：c/c++、操作系统：ubuntu

## 快速上手
cmdoffice的使用分为4步，注册命令（包括通用和自定义）、注册服务程序、启动调试工具、关闭调试工具。
例如
``` c
// ... 
#include "cmdoffice.h" //导库

int main(int argc, char* argv[])
{
    //注册自定义命令
    office_register_cmd("测试",NULL, NULL);
    //注册服务程序
    office_register_service(service_ptr);
    
    //注册通用命令
    office_register_genneral_cmd_dump();
    office_register_genneral_cmd_recovery();
    office_register_genneral_cmd_multiple();
    
    //启动调试工具
    office_work(argc, argv);
    
    //关闭调试工具
    //你需要在适当的位置调用office_quit()，来删除命令和
    //释放资源，这视你的服务程序而定 
}
```
### 注册自定义命令
注册命令使用接口
``` c
int office_register_cmd(char* cmd_name, serv_ptr sptr, cmd_ptr cptr)
```
第1个参数cmd_name是命令的名称；第2个参数serv_ptr是服务端收到来自消息队列的命令的处理入口。服务程序收到命令户，将调用此函数。serv_ptr的原型如下
``` c
typedef void (*serv_ptr)(struct document doc);
```
它接收一个document作为参数，携带命令所需的参数。第3个参数cmd_ptr指向命令进程要运行的代码段的入口。它的原型是

``` c
typedef void (*cmd_ptr)(int argc, char* argv[]);
``` 
总之，要注册一个自定义命令，需要定义该命令的处理函数和命令进程要运行的代码段。如果有一方无需作处理，传入NULL被允许。他们的调用过程见下图<br>
![image](https://user-images.githubusercontent.com/48158080/181438340-47260e38-c9fe-4d68-a6e5-d7c5f6bbb1e9.png)
<br>服务进程中服务程序和命令处理程序并发执行。处理程序接收到命令后，在全局变量寻找命令入口，执行会返回继续监听。命令程序发送完命令后执行自己的程序，用来接收返回参数等。

### 注册通用命令
cmdoffice预定义了一些通用命令，它们能够通过office_register_genneral_cmd_xxx()接口直接注册。这些命令有：
| 命令名称 | 功能说明 |
| --- | --- |
| dump |  单终端重定向，资源消耗小   |
| multiple | 多终端重定向，资源消耗较高  |
| 2file | 调试信息输出到文件 |

## 更多
### 资源的正确释放
调试工具的资源释放由office_quit()提供，根据服务程序的运行情况，应在结束时调用该函数以正确地删除命令和释放资源。
### 命令默认路径
命令默认创建在系统/bin目录下


# cmdoffice
命令行辅助调试工具

## 介绍
cmdoffice(Command Office)是一个由c语言编写的命令行辅助调试工具，它支持：
+ 实现shell命令行对某个进程信息查看、控制等操作
+ 提供通用命令：单客户端重定向dump、重定向恢复recoery、多客户端重定向multiple、文件重定向2file
+ 支持注册自定义命令
+ 允许服务进程同时运行

它允许通过命令的形式执行特定的操作。库本身提供了良好的扩展性，只需注册相关的操作即可。
## 应用截图
在系统/bin目录下生成的命令<br>
![图片](https://user-images.githubusercontent.com/48158080/182803423-869d252f-0c6a-4bd0-b696-2f6afb3bc086.png)



## 运行环境
编程语言：c/c++、操作系统：ubuntu

## 快速上手
cmdoffice的使用分为3步，注册命令（包括通用和自定义）、启动调试工具、关闭调试工具。创建一个**判断某一年是不是闰年**的例子如下。
<br>导库、注册命令、服务，
``` c
#include "../cmdoffice.h"

int main(int argc, char* argv[])
{
    office_register_cmd("是不是闰年",is_leap_sptr, is_leap_cptr);
    office_work(argc, argv);    
}
```
并对两者进行实现。
``` c
void service_ptr(int argc, char* argv[]);
void is_leap_sptr(struct document doc);
void is_leap_cptr(int argc, char* argv[]);

void is_leap_sptr(struct document doc)
{
    int is_leap = 0, year = 0;
    char s_year[10];
    
    office_get_argvi(doc.argv, s_year, 1);//获取传过来的参数，年数参数在索引为1的位置
    year = atoi(s_year);//str转int
    
    if ((year % 4 == 0 && year % 100 !=0) || year % 400 == 0) {//判断是否为闰年的条件
        is_leap = 1;
    }

    //发送结果
    //doc.self_pid保存发送过来的进程的pid,以此来指定接收端
    //结果可以放在type里，也可以放在argv里
    office_send_doccument(doc.self_pid, is_leap, 0, NULL);
}

void is_leap_cptr(int argc, char* argv[])
{
    //接受返回结果
    struct document doc;
    office_recv_document(&doc);

    if(doc.type == 1)
    {
        printf("%s 是闰年\n", argv[1]);
    }
    else
    {
        printf("%s 不是闰年\n", argv[1]);
    }
}

```
到此。运行效果如下<br>
![image](https://user-images.githubusercontent.com/48158080/181672622-758f81c2-7d5f-427f-bb9b-d23e4d503920.png)


### 注册通用命令
cmdoffice预定义了一些通用命令，它们能够通过office_register_genneral_cmd_xxx()接口直接注册。例如注册通用命令dump，只需在上面的例子中增加一行代码即可。
``` c
int main(int argc, char* argv[])
{
    office_register_cmd("是不是闰年",is_leap_sptr, is_leap_cptr);
    office_register_genneral_cmd_dump(); //通用命令dump
    office_work(argc, argv);    
}

``` 
<br>系统通用命令有：
| 命令名称 | 功能说明 |
| --- | --- |
| dump |  单终端重定向，资源消耗小   |
| multiple | 多终端重定向，资源消耗较高  |
| 2file | 调试信息输出到文件 |
| list | 查看目前已支持命令 |

## 注册自定义命令接口说明
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
要注册一个自定义命令，需要定义该命令的处理函数和命令进程要运行的代码段。如果有一方无需作处理，传入NULL被允许。他们的调用过程见下图<br>
![image](https://user-images.githubusercontent.com/48158080/181724764-4c52403f-fc38-4bdf-9d1c-baa192a7adaf.png)

<br>服务进程中服务程序和命令处理程序并发执行。处理程序接收到命令后，在全局变量寻找命令入口，并发执行。命令程序发送完命令后执行自己的程序，用来接收返回参数等。

## 参数收发接口说明
### 结构体document
document是一次发送/接收的信息单元。对于收方，它携带了命令发送方的信息和命令的参数；对于发方，它携带命令的执行返回结果等信息。一次命令的参数一般放在argv（char*）中，不同参数间用空格分开。
### 接受参数
接收接口
``` c
int office_recv_document(struct document *doc);
``` 
来自命令行的参数放在doc.argv中，使用office_get_argvi()获取第i个位置参数。在快速上手例子中
``` c
    char s_year[10];
    
    office_get_argvi(doc.argv, s_year, 1);//获取传过来的参数，年数参数在索引为1的位置
``` 
### 发送参数
发送接口
``` c
int office_send_doccument(int pid, int type,  int argc, char* argv[]);
``` 
pid是接受方的pid，如果给发送方返回结果，则发送方的pid在doc.self_pid中。type是一个整形参数，可以携带一个整形返回值。argc是字符参数的个数，argv是所有字符参数，参数之间用空格区分。如果返回参数很多，用argv返回。

## 更多
### 命令默认路径
命令默认创建在系统/bin目录下


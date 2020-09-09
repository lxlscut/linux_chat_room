 #ifndef UDPSERVER_H
#define UDPSERVER_H
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <list>
#include <errno.h>
#include <sys/wait.h>
//提供了进程信号的函数
#include<signal.h>
//提供了close函数
#include <unistd.h>


//服务器ip地址
#define SERVER_IP "127.0.0.1"
//服务器端口号
#define SERVER_PORT 8888
//定义通信类型
#define CLIENT_LOGIN    100
#define CLIENT_CHAT     200
#define CLIENT_QUIT     300 
#define SERVER_CHAT     400
#define SERVER_QUIT     500 
#define PRINT_ONLINE    600
#define PRIVATE_CHAT    700

using  namespace std;
//消息结构体
struct message {
	long type;
	char name[20];
	char peer_name[20];
	char mtext[512];
};



 class client
{
public:
    char name[20];
    sockaddr_in addr;
};


class udpserver {
public:
	//连接上的客户端的列表
	list<client> client_list;
	//服务器的通信所用的socket
	int udpsocket;
	//服务器对象的无参数构造函数
	udpserver();
	//接收消息
	void recv_message(int socket,list<client> li);
	//发送消息
	//服务器发送消息
	void send_message(int scoket, sockaddr_in *serverAddr, pid_t a);
	//初始化服务器
	void init();
	//关闭服务器
	void Close();
	//启动服务器
	void start();
	//通过地址列表调用程序
	void brocast_msg(int socket, list<client> li, struct message *msg);
	//创建原始线程
	pid_t pid;
private:
	//服务器的地址信息
	struct sockaddr_in serverAddr;
};


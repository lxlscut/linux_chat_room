#include <cstdio>
#include <iostream>
#include "udpserver.h"
using namespace std;
void father_func(int sig_no);
//udpserver的无参构造函数
udpserver::udpserver() {
	//绑定服务器的物理参数，ip与端口
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//初始化socket
	udpsocket = 0;
}
 //初始化服务器
void udpserver::init() {
	//使用udp协议来创建socket
	if ((udpsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("failed to create socket");
		exit(-1);
	}
	//绑定端口
	if (bind(udpsocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("failed to bind");
		exit(-1);
	}
	cout << "server is ready\n" << endl;
}

void udpserver::Close() {
	if (pid) {
		
	}	
}

void udpserver::start() {
	//建立线程
	if ((pid = fork()) < 0) {
		perror("failed to fork pid");
		exit(-1);
	}
	else if (pid == 0) {
		recv_message(udpsocket, client_list);
	}
	else {
		send_message(udpsocket, &serverAddr, pid);
	}
}

void udpserver::recv_message(int socket, list<client> li) {
	//创建消息结构体，用来存放消息对象
	struct message msg;
	//创建客户端地址结构体
	struct sockaddr_in client_addr;
	//获取客户端地址长度
	socklen_t client_addrlen = sizeof(struct sockaddr);
	//创建客户信息结构体
	client cli;
	//客户端信息
	char clientname[512];
	//当前请求的客户端地址
	sockaddr_in my_addr;
	sockaddr_in nulladdr;
	//私聊地址
	struct sockaddr_in peer_addr;
	//遍历地址1
	list<client>::iterator is;
	//遍历地址2
	list<client>::iterator it;
	//遍历地址3
	list<client>::iterator iz;

	//遍历地址3
	list<client>::iterator iq;
	//进入主循环等待消息发送
	while (1)
	{
		memset(&msg,0,sizeof(msg));
		if (recvfrom(udpsocket, &msg, sizeof(msg), 0, (struct sockaddr *)&client_addr, &client_addrlen) < 0) {
			perror("failed to recieve from client ");
			exit(-1);
		}
		cout << msg.mtext << endl;
		switch (msg.type)
		{
		case CLIENT_LOGIN:
			cout << "****************LOGIN IN*************" << endl;
			cout << msg.name << " IS LOGIN IN\n" << endl;
			cout << "*************************************\n";
			//创建结构体存储登录用户信息
			strcpy(cli.name, msg.name);
			cli.addr = client_addr;
			//有用户登录，将其地址添加进地址链表
			client_list.push_back(cli);
			for (iz = client_list.begin();iz != client_list.end();++iz) {
				cout << iz->name << "address :" <<  endl;
			}
			brocast_msg(udpsocket,client_list,&msg);
			break;
		case CLIENT_CHAT:
			cout << "***************GROUP CHAT**************\n" << endl;
			cout << "name:" << msg.name << "\n" << endl;
			cout << "msg:" << msg.mtext << "\n" << endl;
			cout << "******************************************";
			brocast_msg(udpsocket, client_list, &msg);
			break;
		case CLIENT_QUIT:
			cout << "****************QUIT MSG*******************";
			cout  << msg.name << " is Quit\n" << endl;
			cout << "*******************************************";
			
			for (is = client_list.begin(); is != client_list.end(); ++is) {
				if (is->name == msg.name) {
					is = client_list.erase(is);
				}
			}		
		//	client_list.remove(cli);
			break;
		case PRIVATE_CHAT:
			cout << "*******************private msg**************\n";
			cout << "from " << msg.name << "\n" << endl;
			cout << "to " << msg.peer_name << "\n" << endl;
			cout << "mag:" << msg.mtext << "\n" << endl;
			cout << "*********************************************\n";
			
			//自己不能和自己私聊
			for (it = client_list.begin(); it != client_list.end(); ++it) {
				if (strcmp(it->name, msg.peer_name) == 0) {
					peer_addr = it->addr;
					cout << msg.peer_name;
					break;
				}
			}
			//排除掉自己后开始发送信息
			if (sendto(udpsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&peer_addr, sizeof(struct sockaddr)) < 0) {
				/*perror("failed to send private message");
				exit(-1);*/
				cout << "send private dail" << endl;
			}
			//发送完毕跳出
			break;
		case PRINT_ONLINE:
			memset(clientname, 0, sizeof(clientname));
			cout << msg.name << ":is request to print online" << endl;
			my_addr = nulladdr;
			cout << "the size of clientlist is :" << sizeof(client_list) << endl;
			//list<client>::iterator it;
			for (iq = client_list.begin();iq != client_list.end();++iq) {
				if (strcmp(iq->name, msg.name) == 0) {
					my_addr = iq->addr;
					cout << msg.name << endl;
				}
				strcat(clientname, iq->name);
				strcat(clientname, " : is online\n");
			}
			strcpy(msg.mtext, clientname);
			msg.type = PRINT_ONLINE;
			if (sendto(udpsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
				cout << "failed to print online client" << endl;
				break;
			}
			break;
		}
	}
}
//从电脑服务器端键盘输入发送数据
void udpserver::send_message(int scoket, sockaddr_in *serverAddr, pid_t a) {
	struct message msg;
	char buf[512];
	//获取父线程？？？
	signal(getppid(), father_func);
	while (1)
	{
		usleep(500);
		printf(">");
		//获取键盘缓冲区中的内容
		fgets(buf, sizeof(buf), stdin);
		buf[strlen(buf) - 1] = 0;
		//将内容拷贝至msg中相应的部分
		strcpy(msg.mtext, buf);
		strcpy(msg.name, "server");
		msg.type = SERVER_CHAT;
		//如果发送的内容为quit，则执行退出操作
		if (strncmp(buf, "quit", 4) == 0) {
			//设定类型
			msg.type == SERVER_QUIT;
			//发送程序
			if (sendto(udpsocket, &msg, sizeof(msg), 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0) {
				perror("failed to send server_quit message");
				exit(-1);
			}
			pause();
			//发送完成之后关闭发送线程
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			//线程关闭之后关闭socket
			close(udpsocket);
			//无错退出服务器应用程序
			exit(0);
		}
		//如果发送失败，则返回错误信息
		if (sendto(udpsocket, &msg, sizeof(msg), 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0) {
			perror("failed to send server_chat message");
			exit(-1);
		}
	}
}


void father_func(int sig_no)
{
	return;
}

void udpserver::brocast_msg(int socket, list<client> li, struct message *msg) {
	list<client>::iterator it;
	for (it = client_list.begin();it != client_list.end();++it) {
		//如果消息为登陆的话，不向自己发送消息
		if (msg->type == CLIENT_LOGIN) {
			if (strcmp(it->name,msg->name) == 0) {
				continue;
			}		
		}
		//否则向其他的客户端发送消息
		if (sendto(socket, msg, sizeof(struct message), 0, (struct sockaddr *)&(it->addr), sizeof(struct sockaddr)) < 0) {
			cout << "发送失败" << endl;
		};
	}
}
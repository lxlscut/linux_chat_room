#include <iostream>
#include "tcpclient.h"

using namespace std;

// 客户端类构造函数
Client::Client() {

	// 初始化要连接的服务器地址和端口
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// 初始化socket
	sock = 0;

	// 初始化进程号
	pid = 0;

	// 客户端状态
	isClientwork = true;

	// epool fd
	epfd = 0;
}

// 客户端类成员函数

// 连接服务器
void Client::Connect() {
	cout << "Connect Server: " << SERVER_IP << " : " << SERVER_PORT << endl;

	// 创建socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("sock error");
		exit(-1);
	}

	// 连接服务端
	if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("connect error");
		exit(-1);
	}

	// 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
	if (pipe(pipe_fd) < 0) {
		perror("pipe error");
		exit(-1);
	}

	// 创建epoll
	epfd = epoll_create(EPOLL_SIZE);

	if (epfd < 0) {
		perror("epfd error");
		exit(-1);
	}

	//将sock和管道读端描述符都添加到内核事件表中
	addfd(epfd, sock, true);
	addfd(epfd, pipe_fd[0], true);

}

// 断开连接，清理并关闭文件描述符
void Client::Close() {

	if (pid) {
		//关闭父进程的管道和sock
		close(pipe_fd[0]);
		close(sock);
	}
	else {
		//关闭子进程的管道
		close(pipe_fd[1]);
	}
}

// 启动客户端
void Client::Start() {

	// epoll 事件队列
	static struct epoll_event events[2];

	string str = "PRINT ONLINE";

	// 连接服务器
	Connect();

	//创建选择项变量
	int c;

	//
	string content;

	//私聊接收者客户端id
	int receiveid;

	// 创建子进程
	pid = fork();

	// 如果创建子进程失败则退出
	if (pid < 0) {
		perror("fork error");
		close(sock);
		exit(-1);
	}
	else if (pid == 0) {
		// 进入子进程执行流程
		//子进程负责写入管道，因此先关闭读端
		close(pipe_fd[0]);
		// 如果客户端运行正常则不断读取输入发送给服务端
		while (isClientwork) {
			// 输入exit可以退出聊天室
			cout << "Please input 'exit' to exit the chat room" << endl;
			cout << "Input 1 to send group message " << endl;
			cout << "Input 2 to send private message" << endl;
			cout << "Input 3 to get online members" << endl;
			cout << "Input q to exit." << endl;
			 c = getchar();
			while (getchar() != '\n');
			cout << "the option u chosed is :" << c-'0' << endl;
			switch (c)
			{

			case '1':
				//清空结构体
				memset(&msg, 0, sizeof(msg));
				//从键盘缓冲区中获取输入的信息
				cout << "please input the message\n" << endl;
				fgets(msg.content, BUF_SIZE, stdin);
				memset(send_buf, 0, BUF_SIZE);
				//结构体转换为字符串
				memcpy(send_buf, &msg, sizeof(msg));
				if (write(pipe_fd[1], send_buf, sizeof(send_buf)) < 0) {
					perror("group message send fail");
					exit(-1);
				}break;

			case '2':
				
				//清空结构体
				memset(&msg, 0, sizeof(msg));
				msg.type = 1;
				//获取接收者的ID
				cout << "please input the receiveid:\n" << endl;
				receiveid = getchar();
				while (getchar() != '\n');
				msg.toID = receiveid;
				cout << "please input the messtext:\n" << endl;
				fgets(msg.content, BUF_SIZE, stdin);				
				//content.append(to_string(receiveid));
				//content.append(msg.content);
				//memcpy(msg.content,&content,sizeof(content));
				//cout << msg.content << endl;
				//结构体转换为字符串
				cout << msg.type << endl;
				memcpy(send_buf, &msg, sizeof(msg));
				if (write(pipe_fd[1], send_buf, sizeof(send_buf)) < 0) {
					perror("private message send fail");
					exit(-1);
				}break;
			
			case '3':
				
				//清空结构体
				memset(&msg, 0, sizeof(msg));
				msg.type = 2;
				//从键盘缓冲区中获取输入的信息
				//fgets(msg.content, BUF_SIZE, stdin);
				memcpy(msg.content, &str, sizeof(str));
				memset(send_buf, 0, BUF_SIZE);
				memcpy(send_buf, &msg, sizeof(msg));
				if (write(pipe_fd[1], send_buf, sizeof(send_buf)) < 0) {
					perror("print online send fail");
					exit(-1);
				}break;
			case 'q':
				isClientwork = 0;
			default:
				break;
			}
		}
	}
	else {
		//pid > 0 父进程
		//父进程负责读管道数据，因此先关闭写端
		close(pipe_fd[1]);

		// 主循环(epoll_wait)
		while (isClientwork) {
			int epoll_events_count = epoll_wait(epfd, events, 2, -1);

			//处理就绪事件
			for (int i = 0; i < epoll_events_count; ++i)
			{
				memset(recv_buf, 0, sizeof(recv_buf));
				//服务端发来消息
				if (events[i].data.fd == sock)
				{
					//接受服务端广播消息
					int ret = recv(sock, recv_buf, BUF_SIZE, 0);
					//清空结构体
					memset(&msg, 0, sizeof(msg));
					//将发来的消息转换为结构体
					memcpy(&msg, recv_buf, sizeof(msg));
					// ret= 0 服务端关闭
					if (ret == 0) {
						cout << "Server closed connection: " << sock << endl;
						close(sock);
						isClientwork = 0;
					}
					else {
						cout << msg.content << endl;
					}
				}
				//子进程写入事件发生，父进程处理并发送服务端
				else {
					//父进程从管道中读取数据
					int ret = read(events[i].data.fd, recv_buf, BUF_SIZE);
					// ret = 0
					if (ret == 0)
						isClientwork = 0;
					else {
						// 将从管道中读取的字符串信息发送给服务端
						send(sock, recv_buf, sizeof(recv_buf), 0);
					}
				}
			}//for
		}//while
	}

	// 退出进程
	Close();
}


// 客户端主函数
// 创建客户端对象后启动客户端
//int main(int argc, char *argv[]) {
//	Client client;
//	client.Start();
//	return 0;
//}

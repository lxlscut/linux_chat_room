#include "udpclient.h"

udpclient::udpclient()
{
	//初始化连接地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);	
}

void udpclient::Init() {
	cout << "Init Server..." << endl;
	//建立socket
	if ((clientsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("creatsocket failed");
		exit(-1);
	}
	
}

void udpclient::start()
{
	char name[20];
	cout << "please input your chatname\n" << endl;
	fgets(name, sizeof(name), stdin);
	name[strlen(name) - 1] = 0;
	cout << "hahahh";
	cout <<"the username u input is:" << name << endl;
	//建立线程
	if ((pid = fork()) < 0) {
		perror("failed to fork pid");
		exit(-1);
	}else if (pid == 0) {
		recv_message(clientsocket);
	}else{
		send_message(clientsocket, &server_addr,name, pid);
	}
}

void udpclient::recv_message(int clientsocket) {
	struct message msg;
	while (1) {
		memset(&msg, 0, sizeof(msg));
		if (recvfrom(clientsocket, &msg, sizeof(msg), 0, NULL, NULL) < 0) {
			perror("FAILED TO RECV SERVER MESSAGE");
			exit(-1);
		}
		switch (msg.type)
		{
		case CLIENT_LOGIN:
			cout << "**************LOGIN IN *****************\n" << endl;
			cout  << msg.name << "is login in\n" << endl;
			cout << "**************LOGIN IN *****************\n" << endl;
			break;
		case CLIENT_CHAT:
			cout << "***************GROUP MSG ****************\n" << endl;
			cout << "name:" << msg.name << "\n" << endl;
			cout << "msg: " << msg.mtext << "\n" << endl;
			cout << "****************GROUP MSG ****************\n" << endl;
			break;
		case CLIENT_QUIT:
			cout << "****************QUIT MSG *****************\n" << endl;
			cout << "is QUIT \n" << msg.name << "is QUIT \n"  << endl;
			cout << "****************QUIT MSG *****************\n" << endl;
			break;
		case SERVER_CHAT:
			cout << "*****************SERVER MSG **************\n" << endl;
			cout << "msg:" << msg.mtext << "\n"<< endl;
			cout << "*****************SERVER MSG **************\n" << endl;
			break;
		case SERVER_QUIT:
			kill(getppid(), SIGKILL);
			exit(-1);
		case PRIVATE_CHAT:
			cout << "***************PRIVATE MSG *****************\n" << endl;
			cout << "name:" << msg.name<<"\n" << endl;
			cout << "msg:" << msg.mtext <<"\n"<< endl;
			cout << "***************PRIVATE MSG ******************\n" << endl;
			break;
		case PRINT_ONLINE:
			cout << "***************PRINT ONLINE *****************\n" << endl;
			cout << "msg:" << msg.mtext << "\n" << endl;
			cout << "***************PRINT ONLINE ******************\n" << endl;
			break;
		default:
			break;
		}

	}
}

void udpclient::send_message(int clientsocket, sockaddr_in *a, char *name, pid_t pid) {
	struct message msg;
	//文本消息缓存区
	char buf[512];
	//私聊接收人姓名缓存区
	char name1[20];
	//消息类型
	int c;
	//设置消息类型为登录
	msg.type = CLIENT_LOGIN;
	//设置发送名为name
	strcpy(msg.name, name);
	//发送登录消息，进行登录：
	if (sendto(clientsocket, &msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
		perror("failed to send login message");
		exit(-1);
	}
	//如果登陆成功，进入主循环
	//对于1的功能暂做保留处理
	while (1) {
		usleep(500);
		cout << "******************************************\n";
		cout << "u could do somethings as follows\n"<<endl;
		cout << "input 1 is to print online client\n" << endl;
		cout << "input 2 is to chat to all the clients\n" << endl;
		cout << "input 3 is to chat to only one client \n" << endl;
		cout << "input q ls to quit\n" << endl;
		cout << "********************************************\n" << endl;
		cout << ">>>";
		c = getchar();
		while (getchar() != '\n');

		switch (c)
		{
		case '1':
			cout << "execute print online" << endl;
			msg.type = PRINT_ONLINE;
			if (sendto(clientsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
				perror("failed to send online message");
				exit(-1);
			}
			break;
		case '2':	
			cout << "********************GROUP CHAT ROOM*******************\n" << endl;
			cout << "****************welcome to group char room ***********\n" << endl;
			cout << "if u wanna quit,piease input quit \n";
			cout << "*******************************************************\n" << endl;
			while (1) {
				memset(buf, 0, sizeof(buf));
				usleep(500);
				cout << ">>>"<<endl;
				//从键盘缓存区拿到输入信息
				fgets(buf, sizeof(buf), stdin);
				buf[strlen(buf) - 1] = 0;
				//将消息类型与内容放入
				msg.type = CLIENT_CHAT;
				strcpy(msg.mtext, buf);
				
				//如果输入的是quit，退出
				if (strncmp(buf, "quit", 4) == 0)
					break;
				if (sendto(clientsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
							perror("failed to send group message");
							exit(-1);
						}
				cout << msg.mtext << endl;
			}
			break;
		case '3':
			memset(name1, 0, sizeof(name1));
			cout << "please input the peer_name\n" << endl;
			cout << ">>>" << endl;
			fgets(name1, sizeof(name1), stdin);
			name1[strlen(name1) - 1] = 0;

			strcpy(msg.peer_name, name1);
			msg.type = PRIVATE_CHAT;

			cout << "connect to %s ok!\n" << msg.peer_name << endl;

			while (1)
			{
				memset(buf, 0, sizeof(buf));
				usleep(500);
				cout << ">>>" << endl;
				fgets(buf, sizeof(buf), stdin);
				buf[strlen(buf) - 1] = 0;
				strcpy(msg.mtext, buf);
				if (strncmp(buf, "quit", 4) == 0) {
					break;
				}
				if (sendto(clientsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
					perror("failed to send private message");
					exit(-1);
				}
			}
			break;
		case 'q':
			msg.type = CLIENT_QUIT;
			if (sendto(clientsocket, &msg, sizeof(struct message), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
				perror("failed to send quit message");
				exit(-1);
			}
			kill(pid, SIGKILL);
			waitpid(pid,NULL,0);
			exit(0);
		default:
			break;
		}
	}


}

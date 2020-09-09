#ifndef UDP_CLIENT
#define UDP_CLIENT
#endif // !UD_PCLIENT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <sys/wait.h>

#define CLIENT_LOGIN 100
#define CLIENT_CHAT 200
#define CLIENT_QUIT 300

#define SERVER_CHAT 400
#define SERVER_QUIT 500

#define PRINT_ONLINE 600
#define PRIVATE_CHAT 700

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

using  namespace std;

struct message
{
	long type;
	char name[20];
	char peer_name[20];
	char mtext[512];
};

class udpclient
{
public:
	udpclient();
	void Init();
	void start();
	pid_t pid;
	struct sockaddr_in server_addr;
	int clientsocket;
	void recv_message(int clientsocket);
	void send_message(int clientsocket, sockaddr_in *a, char *name,pid_t pid);
};




#include "udpserver.h"
#include "tcpserver.h"

int main(){
	int c;
	cout << "please chose the  procotol\n" << "1:tcp...   2:udp...\n"<<endl;
	c = getchar();
	while (getchar() != '\n'); 
	if(c == '1'){
		 Server server;
		 server.Start();
		}else if(c == '2'){
			udpserver server;
	server.init();
	server.start();
			}
	return 0;
	}

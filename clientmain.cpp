#include "udpclient.h"
#include "tcpclient.h"

int main(){
	cout << "input the protocol u chose\n" << "1:tcp...  2:udp...\n" <<endl;
	int c;
	c = getchar();
	while (getchar() != '\n'); 
	if(c == '1'){
		Client client;
        client.Start();
		}
	else if(c == '2'){
		udpclient udp;
	    udp.Init();
	    udp.start();
		}
	return 0;
	}

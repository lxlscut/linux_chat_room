CC = g++
CFLAGS = -std=c++11
 
all: clientmain.cpp servermain.cpp tcpserver.o tcpclient.o udpserver.o udpclient.o
	$(CC) $(CFLAGS) servermain.cpp  tcpserver.o udpserver.o -o chatroom_server
	$(CC) $(CFLAGS) clientmain.cpp  tcpclient.o udpclient.o -o chatroom_client
 
tcpserver.o: tcpserver.cpp tcpserver.h tcpcommon.h 
	$(CC) $(CFLAGS) -c tcpserver.cpp  
udpserver.o: udpserver.cpp udpserver.h 
	$(CC) $(CFLAGS) -c  udpserver.cpp

tcpclient.o: tcpclient.cpp tcpclient.h tcpcommon.h 
	$(CC) $(CFLAGS) -c tcpclient.cpp  

udpclient.o:  udpclient.cpp udpclient.h 
	$(CC) $(CFLAGS) -c  udpclient.cpp

 
clean:
	rm -f *.o chatroom_server chatroom_client

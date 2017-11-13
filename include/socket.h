#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream> 

class Socket {
	public:
		int sockfd, portno;
		struct sockaddr_in serv_addr;
		int read_to(void* output, size_t length);
		void write_to(void* output, size_t length);
		bool is_open();
};

class SocketServer : public Socket {
	private:
		int serversockfd;
		socklen_t clilen;
		struct sockaddr_in cli_addr;
	public:
		SocketServer (int port);
		~SocketServer();
};

class SocketClient : public Socket {
	private:
		struct hostent *server;
	public:
		SocketClient (int port, char* ip);
		~SocketClient();

};
#endif

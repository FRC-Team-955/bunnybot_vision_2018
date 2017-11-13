#include <socket.h>

SocketServer::SocketServer(int portno) {
	this->portno = portno;
	serversockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (serversockfd < 0) {
		std::cerr << "ERROR opening socket" << std::endl;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//http://stackoverflow.com/questions/5592747/bind-error-while-recreating-socket
	int yes=1;
	if (setsockopt(serversockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		std::cerr << "Socket bind option error" << std::endl;
	}

	if (bind(serversockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "ERROR on binding" << std::endl;
	}

	listen(serversockfd,5);
	clilen = sizeof(cli_addr);
	sockfd = accept(serversockfd, 
			(struct sockaddr *) &cli_addr, 
			&clilen);
	if (sockfd < 0) { 
		std::cerr << "ERROR on accept" << std::endl;
	} else {
		std::cerr << "Client connected" << std::endl;
	}
}

void Socket::write_to(void* output, size_t length) {
	if (write(sockfd, output, length) < 0) {
		std::cerr << "ERROR writing to socket" << std::endl;
	}
}

int Socket::read_to(void* output, size_t length) {
	bzero(output, length);
	int bytes_read = read(sockfd, output, length);
	if (bytes_read < 0) {
		std::cerr << "ERROR reading from socket" << std::endl;
	}
	//if (bytes_read != length) {
	//	std::cerr << "WARNING bytes read did not equal length" << std::endl;
	//}
	return bytes_read;
}

bool Socket::is_open() {
	char errbuf;
	return recv(sockfd, &errbuf, 1, MSG_PEEK); 
}

SocketServer::~SocketServer() {
	std::cerr << "Closing server socket..." << std::endl;
	close(serversockfd);
	close(sockfd);
}

//==================================================================

SocketClient::SocketClient(int portno, char* ip) {
	this->portno = portno;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		std::cerr << "ERROR opening socket" << std::endl;
	}

	server = gethostbyname(ip);
	if (!server) {
		std::cerr << "ERROR server not found" << std::endl;
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);

	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "ERROR connecting" << std::endl;
	}

}

SocketClient::~SocketClient() {
	std::cerr << "Closing client socket..." << std::endl;
	close(sockfd);
}

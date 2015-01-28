#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

int main() {
//	struct sockaddr_storage *their_addr;
//	socklen_t addr_size;
	struct addrinfo hints, *res, *client;
	int status, sockfd, newfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags	= AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "3921", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 1;
	}
	
	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 2;
	}
	
	if((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 3;
	}
	
	struct sockaddr_in their_addr;
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(3920);
	their_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(their_addr.sin_zero), '\0', 8);

	char msg[100];
	int len, bytes_sent, bytes_recv;
	int addr_size = sizeof(their_addr);
	
	if((bytes_recv = recvfrom(sockfd, msg, 100, 0, (struct sockaddr*)&their_addr, &addr_size)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 6;
	}

	len = strlen(msg);
	if((bytes_sent = sendto(sockfd, msg, len, 0, (struct sockaddr*)&their_addr, addr_size)) != len) {
		fprintf(stderr, "errno:%d\n", errno);
		return 6;
	}

	close(sockfd);
	return 0;
}

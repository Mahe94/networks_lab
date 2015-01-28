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
	int status, sockfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags	= AI_PASSIVE;
	
	res = NULL;
	if((status = getaddrinfo(NULL, "3920", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 1;
	}

	
	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "socket errno:%d\n", errno);
		return 2;
	}	
	
	if((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 3;
	}
	
	struct sockaddr_in their_addr;
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(3921);	
	their_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(their_addr.sin_zero), '\0', 8);
	
	char buff;
	
	char msg[100];
	int bytes_recv, len=100, bytes_sent;


	int addr_size = sizeof(their_addr);
	
	gets(msg);
	len = strlen(msg);
	if((bytes_sent = sendto(sockfd, msg, len, 0, (struct sockaddr *)&their_addr, addr_size)) != len) {
		fprintf(stderr, "Sending failed\n");
		return 6;
	}
		
	if((bytes_recv = recvfrom(sockfd, msg, len, 0, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
		fprintf(stderr, "Receiving error:%d\n", errno);
		return 4;
	}
	else {
		msg[bytes_recv]='\0';
		printf("%s\n", msg);
	}
		
	close(sockfd);
	return 0;
}

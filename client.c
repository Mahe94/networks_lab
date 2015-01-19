#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

int main() {
	struct addrinfo hints, *res=NULL;
	int status, sockfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
/**	if((status = getaddrinfo(NULL, "3491", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 1;
	}
**/	
	res = NULL;
	if((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 4;
	}
	
	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "1errno:%d\n", errno);
		return 2;
	}
	
/**	if((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "2errno:%d\n", errno);
		return 3;
	}
	
**/	
	if((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "3errno:%d\n", errno);
		return 5;	
	}
	
	char msg[25];
	int len = 25, bytes_recv;
	
	if((bytes_recv = recv(sockfd, msg, len, 0)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 6;
	}
	else
		printf("Message received:%s\n", msg);
		
	close(sockfd);
	return 0;
}

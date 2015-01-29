#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

int main() {
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int status, sockfd, newfd;
	pid_t PID;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
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
	
	printf("Listening ......\n");
	if((status = listen(sockfd, 10)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 3;
	}
	
	addr_size = sizeof(their_addr);
	
	for(;;) {	
		if((newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size)) == -1) {
			fprintf(stderr, "errno:%d\n", errno);
			return 4;
		}	
	
		if( (PID = fork()) == -1) {
			close(newfd);
			continue;
		} else if( PID>0) {
			close(newfd);
			continue;
		}
		
		
		char *msg = "Connected to local host\n";
		int len, bytes_sent, bytes_recv;
	
		len = strlen(msg);
		if((bytes_sent = send(newfd, msg, len, 0)) != len) {
			fprintf(stderr, "Packet lost\n");
			return 5;
		}
		else
			printf("Message sent\n");
		
		char new_msg[25];
		len = 25;
		if((bytes_recv = recv(newfd, new_msg, len, 0)) == -1) {
			fprintf(stderr, "errno:%d\n", errno);
			return 6;
		}
		else
			printf("Message received:%s\n", new_msg);
	
		close(sockfd);
		exit(0);
	
	}
	return 0;
}

#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
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
		return 4;
	}
	
	addr_size = sizeof(their_addr);
	
	if((newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 5;
	}	
	
	char *msg = "Enter filename:";
	int len, bytes_sent, bytes_recv;
	
	len = strlen(msg);
	if((bytes_sent = send(newfd, msg, len, 0)) != len) {
		fprintf(stderr, "Sending failed\n");
		return 6;
	}
	else
		printf("Message sent\n");
		
	FILE *fp;
	fp = fopen("new.txt", "w");
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 7;
	}
		
	char buff='a';
	while(buff!='\0') {
		if((bytes_recv = recv(newfd, &buff, 1, 0)) == -1) {
			fprintf(stderr, "errno:%d\n", errno);
			return 6;
		}
		else	{
			if(buff!='\0') {
//				printf("%c", buff);
				fputc(buff, fp);
			}	
		}
	}
	fclose(fp);

	printf("Sending reversed file\n");
		
	fp = fopen("new.txt", "r");
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 7;
	}
	fseek(fp,0,SEEK_END);
	fseek(fp,-1, SEEK_CUR);
	while(ftell(fp)>=0) {
		buff = fgetc((FILE *)fp);
		if((bytes_sent = send(newfd, &buff, 1, 0)) != 1) {
			fprintf(stderr, "Sending failed\n");
			return 8;
		}
//		printf("%c", buff);
		if(ftell(fp)==1) 
			break;
		fseek(fp,-2, SEEK_CUR);
	}
	buff='\0';
	if((bytes_sent = send(newfd, &buff, 1, 0)) != 1) {
		fprintf(stderr, "Sending failed\n");
		return 8;
	}
	fclose(fp);	
	
	close(sockfd);
	return 0;
}

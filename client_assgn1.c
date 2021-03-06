#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

int main() {
	struct addrinfo hints, *res;
	int status, sockfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
	res = NULL;
	if((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 1;
	}

	
	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 2;
	}	
	
	if((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 3;	
	}
	
	char msg[25];
	int bytes_recv, len=25, bytes_sent;

	FILE *fp;
	char filename[20];
	
	if((bytes_recv = recv(sockfd, msg, len, 0)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 4;
	}
	else
		printf("%s", msg);
	
	gets(filename);
	
	fp = fopen(filename, "r");
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 5;
	}
	
	char buff;
	while((buff = fgetc((FILE *)fp))!=EOF) {
//		printf("%c", buff);
		if((bytes_sent = send(sockfd, &buff, 1, 0)) != 1) {
			fprintf(stderr, "Sending failed\n");
			return 6;
		}
	}
	buff='\0';
	if((bytes_sent = send(sockfd, &buff, 1, 0)) != 1) {
		fprintf(stderr, "Sending failed\n");
		return 6;
	}
	
	fclose(fp);
	
	fp = fopen("new_client.txt", "w");
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 5;
	}
	
	buff='a';
	while(buff!='\0') {
		if((bytes_recv = recv(sockfd, &buff, 1, 0)) == -1) {
			fprintf(stderr, "errno:%d\n", errno);
			return 7;
		}
		else	{
			if(buff!='\0') {
//				printf("%c", buff);
				fputc(buff, fp);
			}	
		}
	}
	fclose(fp);
	
	printf("\n");
		
	close(sockfd);
	return 0;
}

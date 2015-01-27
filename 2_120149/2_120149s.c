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
	
//	printf("Listening ......\n");
//	if((status = listen(sockfd, 10)) == -1) {
//		fprintf(stderr, "errno:%d\n", errno);
//		return 4;
//	}

//	if((status = getaddrinfo(NULL, "3481", &hints, &client))) {
//		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
//		return 1;
//	}
//	their_addr = (struct sockaddr_storage *)client->ai_addr;
//	addr_size = sizeof(struct sockaddr_storage);
	
//	if((newfd = accept(sockfd, (struct sockaddr*)their_addr, &addr_size)) == -1) {
//		fprintf(stderr, "errno:%d\n", errno);
//		return 5;
//	}	
	
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
	msg[bytes_recv]='\0';
	int i=0, j=0;
	char msg_new[100];
	while(msg[i] != ' ') {
		++i;
	}
	++i;
	while(msg[i]!='\0') {
		msg_new[j] = msg[i];
		++i;
		++j;
	}
	msg_new[j]='\0';
	
	FILE *fp;
	fp = fopen("ip.txt", "r");
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 7;
	}
	
	char url[100], ip[100];
	char ch='a', buff;
	int login=0;
	i=0;
	while((ch = fgetc((FILE *)fp))!=EOF) {
		while(ch!='\n' && ch!=EOF && ch!=' ') {
			url[i] = ch;
			ch = fgetc((FILE *)fp);
		++i;
		}
		url[i]='\0';
		if(strcmp(url, msg_new)==0) {
			i=0;
			ch = fgetc((FILE *)fp);
			while(ch!='\n' && ch!=EOF && ch!=' ') {
				ip[i] = ch;
				ch = fgetc((FILE *)fp);
				++i;
			}
			ip[i]='\0';
			break;			
		}
		i=0;		
	}	
	fclose(fp);
//	printf("\n%s\n", out);
	
	len = strlen(ip);
	if((bytes_sent = sendto(sockfd, ip, len, 0, (struct sockaddr*)&their_addr, addr_size)) != len) {
		fprintf(stderr, "errno:%d\n", errno);
		return 6;
	}

	close(sockfd);
	return 0;
}

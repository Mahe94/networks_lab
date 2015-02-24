/*
	Assignment 4
	Client program that takes port number as input and presents a GUI for file sharing
*/

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include<time.h>
#include<signal.h>

#define PORT "3300"

int main(int argc, char* argv[] ) {
	if(argc < 2) {
		fprintf(stderr, "Enter port number\n");
		return 10;
	}
	
	struct addrinfo hints, *res;
	int status, sockfd;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
	res = NULL;
	if((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
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
	
	char msg2[105], msg3[105], msg4[113], msg5[105], msg6[111], msg[105], name[100];
	int bytes_recv, len, bytes_sent;
	
	fd_set master, read_fds;
	int fdmax = sockfd;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(sockfd, &master);
	
	printf("Enter your username:");
	gets(name);
	
	len = strlen(name);
	if(len < 10) 
		sprintf(msg2, "20%d%s", len, name);
	else
		sprintf(msg2, "2%d%s", len, name);
		
	len = strlen(msg2);
	if((bytes_sent = send(sockfd, msg2, len, 0)) != len) {
		fprintf(stderr, "Sending failed\n");
		return 6;
	}	
	
	if(fork()) {
		while(1) {
			printf("Enter filename to be searched:");
			gets(name);
		
			len = strlen(name);
			if(len < 10) 
				sprintf(msg3, "30%d%s", len, name);
			else
				sprintf(msg3, "3%d%s", len, name);
		
			len = strlen(msg3);
			if((bytes_sent = send(sockfd, msg3, len, 0)) != len) {
				fprintf(stderr, "Sending failed\n");
				return 6;
			}
	/**		while(1) {
				read_fds = master;
				if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
					perror("Select:");
					exit(4);
				}
			
				printf("USER_ID\tUSERNAME\n");
				if(FD_ISSET(sockfd, &read_fds)) {
					if((bytes_recv = recv(i, msg, 105, 0)) == -1) {
						perror("Recieve:");
						return 4;
					}
				
				}
				else
					break;
			}		
	**/		
		}
	}
	else {
		while(1) {
			read_fds = master;
			if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
				perror("Select:");
				exit(4);
			}
		
			if(FD_ISSET(sockfd, &read_fds)) {
				if((bytes_recv = recv(sockfd, msg, 105, 0)) == -1) {
					perror("Recieve:");
					return 4;
				}
				switch(msg[0]) {
					case '3' :
							printf("Do you have %s\n", msg+3);
				}
			}
		}
	}
	
	
	
	// --------------------for file sending----------------------- //
/*
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
*/		
	close(sockfd);
	return 0;
}

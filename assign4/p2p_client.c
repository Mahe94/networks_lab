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
	int status, sockfd, acceptfd, newfd;
	
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);
	
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
	
	freeaddrinfo(res);
	//-----------connected to server-------------//
	
	char msg[1024], msg3[1024], name[100], port[10];
	int bytes_recv, len, bytes_sent;
	
	fd_set master, read_fds;
	int fdmax = sockfd;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(sockfd, &master);
	
	//----------------getting info for packet 1------------------//
	
	msg[0] = '1';
	strcat(msg, argv[1]);
	system("ls>>files");
	FILE *fp;
	fp = fopen("files", "r");
	char buff;
	
	if(fp == NULL) {
		printf("Unable to open file\n");
		return 5;
	}
	else {
		char line[1024];
		while(fgets(line, sizeof(line), fp) != NULL) {
			strcat(msg, " ");
			line[strlen(line)-1] = '\0';
			strcat(msg, line);
		}
	}
	system("rm files");
	
//	printf("%s\n", msg);

	len = strlen(msg);
	if((bytes_sent = send(sockfd, msg, len, 0)) != len) {
		fprintf(stderr, "Sending failed\n");
		return 6;
	}	
	//-------------sent packet 1---------//
	
	if(fork()) {
		while(1) {
			printf("Enter filename to be searched:");
			gets(name);
			
			memset(msg, 0, sizeof(msg));
			msg[0] = '2';
			strcat(msg, name);
			
			len = strlen(msg);
			if((bytes_sent = send(sockfd, msg, len, 0)) != len) {
				fprintf(stderr, "Sending failed\n");
				return 6;
			}
			//---------sent packet 2 to server-----------//
			
			while(1) {
				read_fds = master;
				if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
					perror("Select:");
					exit(4);
				}
			
				printf("\n---------------Search Result---------------\n\n");
				if(FD_ISSET(sockfd, &read_fds)) {
					if((bytes_recv = recv(sockfd, msg3, 1024, 0)) == -1) {
						perror("Recieve:");
						return 4;
					}
					else {
						msg3[bytes_recv] = '\0';
						char *result = msg3 + 1;
						printf("%s\n---------------End Result---------------\n", result);
						break;
					}				
				}
				else
					break;
			}
			
			printf("Select the port:");
			gets(port);
			
			if((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
				fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
				return 1;
			}

	
			if((newfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
				fprintf(stderr, "errno:%d\n", errno);
				return 2;
			}	
	
			if((status = connect(newfd, res->ai_addr, res->ai_addrlen)) == -1) {
				fprintf(stderr, "errno:%d\n", errno);
				return 3;	
			}
			
			len = strlen(msg);
			if((bytes_sent = send(newfd, msg, len, 0)) != len) {
				fprintf(stderr, "Sending failed\n");
				return 6;
			}
			
			FILE *fp = fopen(name, "w");
			if(fp == NULL) {
				printf("Unable to open file\n");
				return 5;
			}
	
			buff='a';
			while(buff!='\0') {
				if((bytes_recv = recv(newfd, &buff, 1, 0)) == -1) {
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
			close(newfd);
			
		}
	}
	else {
		if((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
			return 1;
		}
		if((acceptfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
			fprintf(stderr, "socket:%d\n", errno);
			return 2;
		}
	
		if((status = bind(acceptfd, res->ai_addr, res->ai_addrlen)) == -1) {
			fprintf(stderr, "bind:%d\n", errno);
			return 3;
		}
	
//		printf("Listening ......\n");
		if((status = listen(acceptfd, 10)) == -1) {
			fprintf(stderr, "Listen:%d\n", errno);
			return 4;
		}
		
		FD_ZERO(&master);
		FD_ZERO(&read_fds);
		FD_SET(acceptfd, &master);
		fdmax = acceptfd;
		
		while(1) {
			read_fds = master;
			if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
				perror("Select:");
				exit(4);
			}
			
			if(FD_ISSET(acceptfd, &read_fds)) {
				if((newfd = accept(acceptfd, (struct sockaddr*)&their_addr, &addr_size)) == -1) 
					perror("Accept:");
				else {
					if(fork())
						close(newfd);
					else {
						if((bytes_recv = recv(newfd, msg, 1024, 0)) == -1) {
							perror("Recieve:");
							return 4;
						}
						else {
							msg[bytes_recv] = '\0';
							printf("Packet2 recieved\n");
							
							char *filename;
							filename = msg + 1;
							
							FILE *fp = fopen(filename, "r");
							if(fp == NULL) {
								printf("Unable to open file\n");
								return 5;
							}
	
							
							while((buff = fgetc((FILE *)fp))!=EOF) {
						//		printf("%c", buff);
								if((bytes_sent = send(newfd, &buff, 1, 0)) != 1) {
									fprintf(stderr, "Sending failed\n");
									return 6;
								}
							}
							buff = '\0';
							if((bytes_sent = send(newfd, &buff, 1, 0)) != 1) {
								fprintf(stderr, "Sending failed\n");
								return 6;
							}
							
							fclose(fp);
							printf("Finished Sending\n");
//							scanf("%c", &buff);
						}	
						close(newfd);	
						exit(0);	
					}
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

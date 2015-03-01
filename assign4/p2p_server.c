#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

#define PORT "3300"

int main() {
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int status, sockfd, newfd;
	int i, j, k, bytes_recv, bytes_sent, len;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
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
	
	//--------------server initialization-------------------//
	
	system("rm client_list");
	
	FILE *fw;
	FILE *fr;
	
	fd_set master, read_fds;
	int fdmax = sockfd;
	
	char msg[1024], msg3[1024], name[100], line[1024], filename[1024];
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(sockfd, &master);
//	printf("%d - SOCKFD\n", sockfd);
	
	while(1) {
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Select:");
			exit(4);
		}
		for(i = 0; i <= fdmax; ++i) {
			if(FD_ISSET(i, &read_fds)) {
				if(i == sockfd) {
					if((newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size)) == -1) 
						perror("Accept:");
					else {
						FD_SET(newfd, &master);
						if(newfd > fdmax)
							fdmax = newfd;
						printf("New client accepted ....\n");
					}
						
				}
				else {
					memset(msg, 0, sizeof(msg));
					if((bytes_recv = recv(i, msg, 1024, 0)) <=0) {
						if(bytes_recv <0) {
							fprintf(stderr, "errno:%d\n", errno);
							return 4;
						}
						close(i);
						FD_CLR(i, &master);
					}	
					else {
						msg[bytes_recv] = '\0';
						switch(msg[0]) {
							case '1':
								fw = fopen("client_list", "a");
								
								printf("PORT added .....\n");	
								fputs(msg, (FILE *)fw);
								fputc('\n', (FILE *)fw);
								
								fclose(fw);
								
								break;
								
							case '2':
								filename[0]=' ';
								filename[1]='\0';
								strcat(filename, msg + 1);
								strcat(filename, " ");
								//filename = msg + 1;
								msg3[0] = '3';
								j=1;
								
								fr = fopen("client_list", "r");
								while(fgets(line, sizeof(line), fr) != NULL) {
									if(strstr(line, filename) != NULL) {
										k = 1;
										while(line[k] != ' ') {
											msg3[j]=line[k];
											++j;
											++k;
										}
										msg3[j] = '\n';
										++j;
									}
								}
								fclose(fr);
								
								len = strlen(msg3);
								if((bytes_sent = send(i, msg3, len, 0)) != len) {
									fprintf(stderr, "Sending failed\n");
									return 6;
								}
								else
									printf("Packet3 send\n");
						}
					}			
				}
			}
		}
	}
	
	close(sockfd);
	return 0;
}

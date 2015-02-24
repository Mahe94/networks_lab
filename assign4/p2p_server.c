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

int user_fd[100];

int find(int fd) {
	int i;
	for(i=0; i<100; ++i)
		if(user_fd[i] == fd)
			return i;
	return -1;
}

int main() {
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int status, sockfd, newfd;
	
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
	
	fd_set master, read_fds;
	int fdmax = sockfd;
	
	char username[100][100];
	memset(user_fd, 0, sizeof(user_fd));
	memset(username, 0 , sizeof(username));
	int bytes_recv, bytes_sent, i, len;
	char msg[120];
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(sockfd, &master);
	printf("%d - SOCKFD\n", sockfd);
	
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
						printf("Newfd added:%d\n", newfd);
					}
						
				}
				else {
					if((bytes_recv = recv(i, msg, 105, 0)) <=0) {
						if(bytes_recv <0) {
							fprintf(stderr, "errno:%d\n", errno);
							return 4;
						}
						close(i);
						FD_CLR(i, &master);
						
						int fd = find(i);
						user_fd[fd] = 0;
						memset(username[fd], 0, sizeof(username[fd]));
					}	
					else {
						len = strlen(msg);
						int u;
						switch(msg[0]) {
							case '2':
								u = 0;
								while(user_fd[u] != 0) 
									++u;
								
								user_fd[u] = i;
								msg[bytes_recv]='\0';
								strcpy(username[u], msg+3);
							
								printf("%s has joined at fd:%d\n", username[u], user_fd[u]);
								break;
								
							case '3':
								if(fork())
									continue;
								for(u = 0; u<100; ++u) {
									if(user_fd[u] != 0 && user_fd[u] != i) {
										if((bytes_sent = send(user_fd[u], msg, len, 0)) != len) {
											perror("Send:");
											return 6;
										}	
									}	
								}
								exit(0);
								break;
						}
					}			
				}
			}
		}
	}
	
	close(sockfd);
	return 0;
}

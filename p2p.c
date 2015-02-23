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

struct sockaddr_in their_addr;
char msg1[110], name[110], recv_msg[110], send_msg[110];
int len, bytes_sent, bytes_recv, sockfd;
int addr_size = sizeof(their_addr);
fd_set master, read_fds;
int fdmax;
struct timeval tv;

void execute(char *recv_msg, struct sockaddr_in their_addr) {
	char ip[INET_ADDRSTRLEN];
	inet_ntop(their_addr.sin_family, &their_addr.sin_addr, ip, INET_ADDRSTRLEN);
	char size[3];
	int len;
	switch(recv_msg[0]) {
		case '1':			
		case '2':
		case '3':
			printf("%s %s\n", recv_msg, ip);
	}
}

void check_users() {
	len = strlen(msg1);
	if((bytes_sent = sendto(sockfd, msg1, len, 0, (struct sockaddr*)&their_addr, addr_size)) != len) {
		perror("Error");
		return ;
	}
	while(1) {
		read_fds = master;
		select(fdmax + 1, &read_fds, NULL, NULL, &tv);		
		if(FD_ISSET(sockfd, &read_fds)) {
			if((bytes_recv = recvfrom(sockfd, recv_msg, 100, 0, (struct sockaddr*)&their_addr, &addr_size)) == -1) {
				perror("Packet 1 Error:");
			}	
			else {
				if(recv_msg[0] == 2) {
					char ip[INET_ADDRSTRLEN];
					recv_msg[bytes_recv] = '\0';
					printf("%s from %s\n", recv_msg, inet_ntop(their_addr.sin_family, &their_addr.sin_addr, ip, INET_ADDRSTRLEN));
				}
				else
					execute(recv_msg, their_addr);	
			}
		}
		else
			break;
//		printf("in the loop\n");
	}
	printf("sending packet 1\n");
	signal(SIGALRM, check_users);
	alarm(5);
}

int main() {
//	struct sockaddr_storage *their_addr;
//	socklen_t addr_size;
	struct addrinfo hints, *res, *client;
	int status;
	int broadcast = 1;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags	= AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, "3300", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(status));
		return 1;
	}
	
	if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 2;
	}
	
	if((setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast)) == -1)
	{
		perror("setsockopt - SO_SOCKET ");
		exit(1);
	}
	
	if((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "errno:%d\n", errno);
		return 3;
	}
	
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(sockfd, &master);
	
	fdmax = sockfd;
	
	tv.tv_sec = 2;
	tv.tv_usec = 500000;
	
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(3300);
	inet_aton("255.255.255.255",&their_addr.sin_addr);
	memset(&(their_addr.sin_zero), '\0', 8);

	printf("Enter username:");
	gets(name);
	if(strlen(name)>=10)
		sprintf(msg1, "1%d%s", (int)strlen(name), name);
	else
		sprintf(msg1, "10%d%s", (int)strlen(name), name);
	
	check_users();
	
	while(1) {
/*		printf("Enter the filename to be searched:");
		gets(name);
		if(strlen(name)>=10)
			sprintf(send_msg, "3%d%s", (int)strlen(name), name);
		else
			sprintf(send_msg, "30%d%s", (int)strlen(name), name);
		len = strlen(send_msg);
		if((bytes_sent = sendto(sockfd, send_msg, len, 0, (struct sockaddr*)&their_addr, addr_size)) != len) {
			perror("Error");
			return ;
		}
		
		clock_t begin, end;
		begin = clock();
		end = clock();
		int search = 0;
		while((end-begin)/CLOCKS_PER_SEC < 2) {
			read_fds = master;
			if(select(fdmax, &read_fds, NULL, NULL, &tv) != -1) {
				if((bytes_recv = recvfrom(sockfd, recv_msg, 100, 0, (struct sockaddr*)&their_addr, &addr_size)) != -1) {
					perror("Error:");
				}	
				else {
					if(recv_msg[0] == 4) {
						printf("%d ", search);
						for(int i=3; i<bytes_recv; ++i) 
							printf("%c", recv_msg[i]);
						
						if(recv_msg[1]==0)
							printf("%c\n", recv_msg[2]);
						else
							printf("%c%c\n", recv_msg[1], recv_msg[2]);
					}
					else
						execute(recv_msg);	
				}
			}
			end = clock();
		}
		printf("\n");
		
		printf("Enter the number of the client to start download (Enter -1 for cancellation): ");
		scanf("%d", &search);
		if(search != -1) {
		}
*/	}
	
	close(sockfd);
	return 0;
}

#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

int main(int argc, char* argv[]) {
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int status, sockfd, newfd, pid, bytes_recv, bytes_sent;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags	= AI_PASSIVE;
	
	if((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
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
	
	
	addr_size = sizeof(client_addr);
	
	while(1) {
		if((newfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
			fprintf(stderr, "Here is the errno:%d\n", errno);
			return 4;
		}	
	
		if((pid = fork()) == 0) {
			int flag, port=0, i, sockfd2, n;
			struct sockaddr_in host_addr;
			struct hostent *host;
			char buffer[510], t1[300], t2[300], t3[300];
			char *temp = NULL;
			memset(buffer, 0, sizeof(buffer));
			
			if((bytes_recv = recv(newfd, buffer, 500, 0)) == -1) {
				fprintf(stderr, "errno:%d\n", errno);
				return 6;
			}
			
			sscanf(buffer, "%s %s %s", t1, t2, t3);
			
			if((strncmp(t1, "GET", 3)==0) && (strncmp(t3, "HTTP/1.0", 8)==0)) {
				strcpy(t1, t2);
				flag = 0;
				
				for(i=7; i<strlen(t2); ++i) {
					if(t2[i]==':') {
						flag = 1;
						break;
					}
				}
				
				temp = strtok(t2, "//");
				if(flag == 0) {
					port = 80;
					temp = strtok(NULL, "/");
				}
				else 
					temp = strtok(NULL, ":");
					
				sprintf(t2, "%s", temp);
				printf("host = %s\n", t2);
				host = gethostbyname(t2);
				
				if(flag == 1) {
					temp = strtok(NULL, "/");
					port = atoi(temp);
				}
				
				
				strcat(t1, "^]");
				temp = strtok(t1, "//");
				temp = strtok(NULL, "/");
				if(temp != NULL) 
					temp = strtok(NULL, "^]");
				printf("Path = %s\nPort = %d\n", temp, port);
				
				memset(&host_addr, 0, sizeof(host_addr));
				host_addr.sin_port=htons(port);
				host_addr.sin_family=AF_INET;	
				memcpy((char*)&host_addr.sin_addr.s_addr, (char*)host->h_addr, host->h_length);
				
				if((sockfd2 = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
					fprintf(stderr, "errno:%d\n", errno);
					return 2;
				}
				
				if((status = connect(sockfd2, (struct sockaddr*)&host_addr, sizeof(struct sockaddr))) == -1) {
					fprintf(stderr, "3errno:%d\n", errno);
					return 5;	
				}
	
				sprintf(buffer, "\nConnected to %s IP - %s\n", t2, inet_ntoa(host_addr.sin_addr));
				printf("\n%s\n", buffer);
				
				memset(buffer, 0, sizeof(buffer));
				
				if(temp!=NULL)
					sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
				else
					sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);
					
				printf("\n%s\n",buffer);
 				if((n = send(sockfd2, buffer, strlen(buffer), 0)) != strlen(buffer)) {
					fprintf(stderr, "Packet lost\n");
					return 5;
				}
				else {
					printf("Getting response and sending to client\n");
					do {
						memset(buffer, 0, sizeof(buffer));
						n = recv(sockfd2, buffer, 500, 0);
						if(!(n<=0))
							send(newfd, buffer, n, 0);
					}while(n>0);
				}			
			}
			else {
				if((bytes_sent = send(newfd, "400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0)) != 18) {
					fprintf(stderr, "Packet lost\n");
					return 5;
				}
			}
			
			printf("----------------------Completed---------------------\n");
//			close(sockfd2);
//			close(newfd);
//			close(sockfd);
			
//			exit (0);
			
		} else {
			close(newfd);
		}
	}
	/** ----------------------------------------
	char *msg = "Connected to local host\n";
	int len, bytes_sent;
	
	len = strlen(msg);
	if((bytes_sent = send(newfd, msg, len, 0)) != len) {
		fprintf(stderr, "Packet lost\n");
		return 5;
	}
	else
		printf("Message sent\n");
	
	close(sockfd);
	-------------------------- **/
	
	return 0;
}

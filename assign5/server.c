#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include<time.h>

#define PORT "3300"
#define WINDOW 4
#define PAYLOAD_SIZE 256
#define RTT 1

struct timing {
	int seq;
	int tim;
}t[WINDOW];

struct segment {
	int s;
	int a;
	char payload[PAYLOAD_SIZE];
}segm_sent, segm_recv;

void print_segm(struct segment *seg, int t) {
	if(t)
		printf("SENDING SEGMENT:");
	else
		printf("RECEIVING SEGMENT:");
	printf("%d %d %s", seg->s, seg->a, seg->payload);
}

void print_Window(int base, int arr[]) {
	int i;
//	printf("\n\nSequence num:\t");
//	for(i=0; i<WINDOW; ++i)
//		printf("%d\t", base+i);
//	printf("\nAcknowleged:\t");
	for(i=0; i<WINDOW; ++i)
		printf("%d\t", arr[i]);
	printf("\n\n");
}

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
	
	FILE *fp;
	
	fd_set master, read_fds, write_fds;
	int fdmax = sockfd;
	
	clock_t start = clock(), now;
	
	int sent_base=0, recv_ack[WINDOW], next_squ_num=0, next_ack_num=0, f1=0, f2=0, last=-1, u;
	char msg[PAYLOAD_SIZE], ack[10], filename[20], file[1000000], recv_msg[1000000];
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_SET(sockfd, &master);
	
	memset(recv_ack, -1, sizeof(recv_ack));
	memset(&segm_sent, 0, sizeof(segm_sent));
	memset(file, 0, sizeof(file));
	memset(t, -1, sizeof(t));
	
	while(1) {
		read_fds = master;
		write_fds = master;
		if(select(fdmax+1, &read_fds, &write_fds, NULL, NULL) == -1) {
			perror("Select:");
			exit(4);
		}
		if(f1 && f2)
			break;
		for(i = 0; i <= fdmax; ++i) {
			if(FD_ISSET(i, &read_fds)) {
				if(i == sockfd) {
					if((newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size)) == -1) 
						perror("Accept:");
					else {
						FD_SET(newfd, &master);
						if(newfd > fdmax)
							fdmax = newfd;
						printf("New client accepted ....\nEnter filename:");
						gets((char *)filename);						//open file to be send
						fp = fopen((char *)filename, "r");
						if(fp == NULL) {
							printf("Unable to open file\n");
							return 5;
						}
					}
						
				}
				else {
//					printf("READING:");
//					scanf("%d", &u);
					if((bytes_recv = recv(i, &segm_recv, sizeof(segm_recv), 0)) == -1) {		//receiving segment
						fprintf(stderr, "Receive error, errno:%d\n", errno);
						return 4;
					}
					print_segm(&segm_recv, 0);
					int ACK = segm_recv.a;
					printf("ACK recevied:%d\n\n", ACK);
//					if(ACK >= sent_base ) {
						for(j=0; j<WINDOW; ++j)
							if(recv_ack[j] == ACK)							//removing entry from rcv_ack
								recv_ack[j] = -2;
						
						for(j=0; j<WINDOW; ++j)
							if(recv_ack[j] >= 0 || recv_ack[j]==-1)					//-1 means free slot and >= ack not received
								break;	
			
						sent_base = recv_ack[j];									//increment base of the window
						
						for(k=0; j<WINDOW; ++j, ++k)
							recv_ack[k] = recv_ack[j];						//shifting 
			
						while(k<WINDOW) {
							recv_ack[k] = -1;							//make the last entries 0
							++k;
						}
						
						for(k=0; k<WINDOW; ++k) {							//removing entry from time struct
							if(ACK == t[k].seq) {
								t[k].seq = -1;
								t[k].tim = -1;
								break;
							}
						}
						
						print_Window(sent_base, recv_ack);
						if(ACK == last)  {								//if last packet
							f1 = 1;	
							printf("f1 is set\n");
						}
	//					scanf("%d",&k);
//					}
					
					if(!f2) {
						if(segm_recv.payload[0] == '\0') {
							f2=1;
							printf("f2 is set\n");
						}
						for(j=0; j<strlen(segm_recv.payload); ++j)						//storing in main buffer
							recv_msg[segm_recv.s + j] = segm_recv.payload[j];
						
						j=0;
						while(recv_msg[j] != '\0') ++j;
						next_ack_num = j;
					}
				}
			}
			
			if(f1 && f2)
				break;
			
			if(FD_ISSET(i, &write_fds) && i== newfd) {
//				printf("WRITING:");
//				scanf("%d", &u);
				if(f1) {
					segm_sent.s = last;
					segm_sent.a = next_ack_num;
					segm_sent.payload[0] = '\0';
					if((bytes_sent = send(newfd, &segm_sent, sizeof(segm_sent), 0)) == 0) {				//sending segment
						fprintf(stderr, "Sending segment error, errno:%d\n", errno);
						return 4;
					}
				}
				else {
					for(i=0; i<WINDOW; ++i) {
						if(recv_ack[i] == -1)
							break;
					}
						
					if(i == WINDOW)
						continue;							//checking if window is full
					
					memset(&segm_sent, 0, sizeof(segm_sent));
				
		
					segm_sent.s = next_squ_num;
					segm_sent.a = next_ack_num;
					fgets(segm_sent.payload, PAYLOAD_SIZE, fp);
				
					for(j=segm_sent.s, k=0; k<strlen(segm_sent.payload); ++j, ++k) 
						file[j] = segm_sent.payload[k];		//copying to buffer
				
					next_squ_num += strlen(segm_sent.payload);
				
					if(segm_sent.payload[0] == '\0') {
						printf("Finished sending file\n\n");
						last = next_squ_num;
					}

				
	//				printf("%d", segm_sent.s);
					j=0;
					while(t[j].seq!=-1) ++j;
					
					print_segm(&segm_sent, 1);
					printf("Do you want to drop this packet:\n");
					scanf("%d", &u);
					if(!u) {				
						if((bytes_sent = send(newfd, &segm_sent, sizeof(segm_sent), 0)) == 0) {				//sending segment
							fprintf(stderr, "Sending segment error, errno:%d\n", errno);
							return 4;
						}
					}	
				
					t[j].seq = next_squ_num;									//receiving ack will be equal to next required byte
					t[j].tim = clock();
				
					recv_ack[i] = next_squ_num;
					print_Window(sent_base, recv_ack);				
				}
			}			
		}
		
		for(i=0; i<WINDOW; ++i) {
			now = clock();
			
			if((t[i].seq != -1) && ((now - t[i].tim)*1000/CLOCKS_PER_SEC > RTT*1000)) {
				printf("ACK not received for %d\n", t[i].seq);
				int last_line;
				for(j=0; j<t[i].seq; ++j)
					if(file[j] == '\n')
						last_line = j;
				
						
				segm_sent.s = last_line+1;
				
				for(j=0; ; ++j)	{
					segm_sent.payload[j] = file[last_line+1 + j];
					if(file[j]=='\n') 
						break;
				}
						
				segm_sent.a = next_ack_num;
				
				t[i].tim = clock();
				
				print_segm(&segm_sent, 1);
				if((bytes_sent = send(newfd, &segm_sent, sizeof(segm_sent), 0)) == 0) {				//sending segment
					fprintf(stderr, "Sending segment error, errno:%d\n", errno);
					return 4;
				}
				print_Window(sent_base, recv_ack);
			}
			
						
		}								
	}
	
	printf("%s\n", recv_msg);
	close(sockfd);
	return 0;
}

/*
Student ID : 
Name : 
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100

void error_handling(char *message);
void display();
int findEmptySlot(int *clienttcpSocket); 

#define MAXCLIENT (8)
int clienttcpSocket[MAXCLIENT];	// max 8 clients


int main(int argc, char **argv) {
	
  int tcpServ_sock;
  
  struct sockaddr_in tcpServer_addr, remote;
  struct sockaddr_in tcpClient_addr;
  struct sockaddr_in newTcp_addr;

  int addrlen;
  int bytesread;
  // initialize the client socket descriptors
  for(int i = 0; i < MAXCLIENT; i++) {
  	clienttcpSocket[i] = -1;;	// max 8 clients
    
  }

  int slot;
  socklen_t clnt_len;
    
  fd_set reads, temps;
  int fd_max;

  
  char str[BUFSIZE];
  int option = 2;

  char *tcpport = NULL;

  if(argc != 2) {
    printf("Usage : %s <tcpport> \n", argv[0]);
    exit(1);
  }

  tcpport = argv[1];

  display();
	
  tcpServ_sock = socket(PF_INET, SOCK_STREAM, 0);
  if(tcpServ_sock == -1)
	  error_handling("socket() error");
  
  memset(&tcpServer_addr, 0 , sizeof(tcpServer_addr));
  tcpServer_addr.sin_family = AF_INET;
  tcpServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcpServer_addr.sin_port = htons(atoi(tcpport));

  setsockopt(tcpServ_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&option, sizeof(int));
  if( bind(tcpServ_sock, (struct sockaddr *) &tcpServer_addr, sizeof(tcpServer_addr)) == -1 )
	  error_handling("bind() error");

			
  if(listen(tcpServ_sock, 5)==-1)
    error_handling("listen() error");
  
  FD_ZERO(&reads);
  FD_SET(tcpServ_sock, &reads);
  fd_max = tcpServ_sock;

  while(1) {
    int nfound = 0;

    temps = reads;
    
    nfound = select(fd_max+1, &temps, 0, 0, NULL);
	
	if(FD_ISSET(tcpServ_sock, &temps)) {
		// Input from the client for new connection
		// This request will arrive after UDP sent
		int new = findEmptySlot(clienttcpSocket);  
		if(new == -1)
			printf("error");
		addrlen = sizeof(remote);
		clienttcpSocket[new] = accept(tcpServ_sock, 
					(struct sockaddr *)&remote, &addrlen);
		if(clienttcpSocket[new] < 0){
			perror("accept");
			exit(1);
		}
		printf("connection from host %s, port %d, socket %d\n",
			inet_ntoa(remote.sin_addr), ntohs(remote.sin_port),
			clienttcpSocket[new]);
		FD_SET(clienttcpSocket[new], &reads);
		if (clienttcpSocket[new] > fd_max)
			fd_max = clienttcpSocket[new];
  		FD_CLR(clienttcpSocket[new], &temps);



	// NEED TO IMPLEMENT HERE



	} else {
		for(int i = 0; i < MAXCLIENT; i++) {
		  if (FD_ISSET(clienttcpSocket[i], &temps)) {
			FD_CLR(clienttcpSocket[i], &temps);

			/* process the data */
           // forward the message to all the other sockets


			// NEED TO IMPLEMENT HERE
			char buf[1024];
			bytesread = read(clienttcpSocket[i], buf, sizeof buf);
			if( bytesread == 0){
				printf("Connection Closed %d\n", clienttcpSocket[i]);
				FD_CLR(clienttcpSocket[i], &reads);
				clienttcpSocket[i] = -1;
				continue;
			}
			buf[bytesread] = '\0';
			for(int j=0; j<MAXCLIENT; j++){
				if( (clienttcpSocket[j] != -1) && (i != j) ){
				    if(write(clienttcpSocket[j], buf, bytesread) 
								!= bytesread)
						perror("echo");
				}
			}

		  }
		}//for End
	}//else End

  }//while End
}//main End

void display() {
	printf("Student ID : 20162800 \n");
	printf("Name : InGyu Lee  \n");
}

void error_handling(char *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  perror("hw4 error");
  exit(1);
}

int findEmptySlot(int *clienttcpSocket){
  for(int i=0; i<MAXCLIENT; i++){
      if(clienttcpSocket[i] == -1)
	return i;
  }
  return -1;
}



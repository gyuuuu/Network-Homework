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

#define TALKCMD "@talk"
#define QUITCMD "@quit"

void display();


int peertcpSocket = -1;	// peer socket

int main(int argc, char **argv) {
	
	int tcpServ_sock;
	//int tcpCli_sock;
	
	struct sockaddr_in tcpServer_addr, remote;
	struct sockaddr_in tcpClient_addr;
	struct sockaddr_in newTcp_addr;
	struct hostent *he;

	int clnt_len, addrlen;
		
	fd_set reads, temps;
	int fd_max;

	char command[1024];
	


	char *tcpport = argv[1];
	char po[20];
	strcpy(po, tcpport);

	char *userid = argv[2];
	//char message[1024];
	//strcpy(message, userid);

	char separator[5] = "@";
	//strcat(message, separator);
	//printf("%s\n", message);

	int portnum = atoi(po);

	int bytesread;

	//printf("eho : %s %s\n", tcpport, userid);

// NEED TO ADD SOME VARIABLES 

	if(argc != 3){
		printf("Usage : %s <tcpport> <userid>\n", argv[0]);
		exit(1);
	}


	display();
	
	// NEED TO CREATE A SOCKET FOR TCP SERVER

	tcpServ_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	memset((void *) &tcpServer_addr, 0, sizeof (tcpServer_addr));
	tcpServer_addr.sin_family = AF_INET;
	tcpServer_addr.sin_addr.s_addr = INADDR_ANY;
	tcpServer_addr.sin_port = htons((u_short)portnum);

	// NEED TO bind
	if (bind(tcpServ_sock, (struct sockaddr *)&tcpServer_addr, sizeof (tcpServer_addr)) < 0) {
		perror("bind");
		exit(1);
		//이미 다른 프로세스(소켓)이 내가 쓰려는 포트넘버 사용 중일때
	}		

	// NEED TO listen
	if (listen(tcpServ_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(1);
	}

	// initialize the select mask variables and set the
	// mask with stdin and the tcp server socket

	FD_ZERO(&reads);
	FD_SET(tcpServ_sock, &reads);
	FD_SET(fileno(stdin), &reads);
	fd_max = tcpServ_sock;

	printf("%s> \n", userid);

	while(1) {
		int nfound;

		temps = reads;
		
		nfound = select(fd_max+1, &temps, 0, 0, NULL);
	
		if(FD_ISSET(fileno(stdin), &temps)) {
			// Input from the keyboard
			fgets(command, sizeof (command), stdin);
				FD_CLR(fileno(stdin), &temps);
		

		// NEED TO IMPLEMENT for input from keybord
	
				printf("%s> \n", userid);
				char *ptr = strchr(command, '@');
				if(ptr != NULL){
					char *cmd = strtok(command, " \t\n\r");
					if((cmd == NULL) || (strcmp(cmd, "") == 0)) 
						continue;
					else if(strcasecmp(cmd, QUITCMD) == 0){
						exit(0);
						//quit일 때 할 일 만들기
					}
					if(!strcasecmp(cmd, TALKCMD) == 0) {
						printf("Wrong command %s\n", cmd);
						continue;
					}

					char *hostname = strtok(NULL, " \t\n\r");
					char *pnum = strtok(NULL, " ");
					//printf("%s:%s\n", hostname, pnum);
					//cliconnect(,hostname, pnum);
					char domain[100];
					char po[50];
					strcpy(domain, hostname);
					strcpy(po, pnum);
					
					he = gethostbyname(domain);
					if ((peertcpSocket = socket(AF_INET, SOCK_STREAM, 0))== -1){
       					perror("Socket");
       					exit(1);
    					}

					memset((void *) &tcpServer_addr, 0, sizeof (tcpServer_addr));
					tcpServer_addr.sin_family = AF_INET;
					memcpy((void *) &tcpServer_addr.sin_addr, he->h_addr, he->h_length);
					tcpServer_addr.sin_port = htons((u_short)atoi(po));

					if (connect(peertcpSocket, (struct sockaddr *)&tcpServer_addr, sizeof (tcpServer_addr)) < 0) {
						(void) close(peertcpSocket);
						fprintf(stderr, "connect");
						exit(1);
					}
					
					FD_SET(peertcpSocket, &reads);
					if (peertcpSocket > fd_max)
						fd_max = peertcpSocket;
					FD_CLR(peertcpSocket, &temps);

				}
				else{
					//printf("it's a chatting\n");
					//printf("%s\n", command);
					char message[1024];
					strcpy(message, userid);
					strcat(message, separator);

					strcat(message, command);
					if(write(peertcpSocket, message, strlen(message)) < 0){
						perror("write");
						exit(1);
					}
				}
		}
		else if(FD_ISSET(tcpServ_sock, &temps))
		{
			//connect request from a peer
			addrlen = sizeof(remote);
			peertcpSocket = accept(tcpServ_sock,
				(struct sockaddr *)&remote, &addrlen);
			if (peertcpSocket < 0) {
				perror("accept");
				exit(1);
			}
			printf("connection from host %s, port %d, socket %d\n",
				inet_ntoa(remote.sin_addr), ntohs(remote.sin_port),
				peertcpSocket);
			FD_SET(peertcpSocket, &reads);
			if (peertcpSocket > fd_max)
				fd_max = peertcpSocket;
			FD_CLR(peertcpSocket, &temps);

		}
		else if(FD_ISSET(peertcpSocket, &temps))
		{

			// message from a peer
			char buf[1024];
			bytesread = read(peertcpSocket, buf, sizeof buf);
			if( bytesread == 0){
				printf("Connection Closed %d\n", peertcpSocket);
				FD_CLR(peertcpSocket, &reads);
				continue;
			}
			buf[bytesread] = '\0';
			char *yourid = strtok(buf, "@");
			char *msg = strtok(NULL, "@");
			printf("%s : %s\n", yourid, msg);
		}

	}//while End
}//main End



void display() {
	printf("Student ID : 20162800 \n");
	printf("Name : InGyu Lee \n");
}




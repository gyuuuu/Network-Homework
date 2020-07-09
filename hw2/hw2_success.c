// Student ID :
// Name :

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


char real_file[20];

int ReadHttpfile(int sock){
    char c;
    char buff[1024]="",*ptr=buff+1;
    int bytes_received;
    char *pt;
    char filename[20];
    int index = 1;
    
    //printf("Begin Response ..\n");
    while(bytes_received = recv(sock, ptr, 1, 0)){
        if(bytes_received==-1){
            perror("ReadHttpStatus");
            exit(1);
        }

        if((ptr[-1]=='\r')  && (*ptr=='\n' )) break;
        ptr++;
    }
    *ptr=0;
    ptr=buff+1;

    sscanf(ptr,"%*s %s %*s", filename);

    printf("%s\n",ptr);
    //printf("filename=%s\n",filename);

    while(filename[index] != '\0'){
	real_file[index-1] = filename[index];
	index++;
    }
    real_file[index-1] = '\0';
    //printf("%s\n", real_file);

    if (bytes_received > 0)
	return 1;
    else
	return 0;

}

int ParseHeader(int sock){
    char c;
    char buff[1024]="",*ptr=buff+4;
    int bytes_received, status;
    while(bytes_received = recv(sock, ptr, 1, 0)){
        if(bytes_received==-1){
            perror("Parse Header");
            exit(1);
        }

        if(
            (ptr[-3]=='\r')  && (ptr[-2]=='\n' ) &&
            (ptr[-1]=='\r')  && (*ptr=='\n' )
        ) break;
        ptr++;
    }

    *ptr=0;
    ptr=buff+4;
    printf("%s\n",ptr);

    return  bytes_received ;

}

int main(int argc, char *argv[]) {
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	int bytesread;
	socklen_t addrlen;
	char buf[BUFSIZ];

	if (argc != 2) {
		(void) fprintf(stderr,"usage: %s portnum \n",argv[0]);
		exit(1);
	}

	int portnum = atoi(argv[1]);

	if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}
	
	printf("Student ID : 20162800\n");
	printf("Name : InGyu Lee\n");

	// Create a Server Socket

	memset((void *) &server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons((u_short)atoi(argv[1]));

	if (bind(request_sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
		perror("bind");
		exit(1);
		//이미 다른 프로세스(소켓)이 내가 쓰려는 포트넘버 사용 중일때
	}
	if (listen(request_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(1);
	}

	while(1) {
		/* a new connection is available on the connetion socket */
		addrlen = sizeof(remote);
		new_sock = accept(request_sock,
			(struct sockaddr *)&remote, &addrlen);
		if (new_sock < 0) {
			perror("accept");
			exit(1);
		}
		printf("connection from host %s, port %d, socket %d\n",
			inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), new_sock);


		int j = 0;
		char *arr[3];
		char status[20];
		char request[1024];
		int size = 0;
		char send_data[1024];
		
		//j = ReadHttpStatus(new_sock);
		//printf("%d\n", j);
		//ParseHeader(new_sock);
		if(ReadHttpfile(new_sock) && (ParseHeader(new_sock))){
			FILE *fp = fopen(real_file, "r");
			if(fp == NULL){
				printf("Server Error : No such file /%s\n", real_file);
				char str[] = "404 NOT FOUND";
				strcpy(status, str);
				snprintf(send_data, sizeof(send_data), "HTTP/1.0 %s\r\nConnection: close\r\nID: 20162800\r\nName: 이인규\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", status, size);

				//printf("%s\n", send_data);
				send(new_sock, send_data, strlen(send_data)+1, 0);
			}

			else{
				char str[] = "200 OK";
				strcpy(status, str);
				
				char *buffer;
				int  count;

				fseek(fp, 0, SEEK_END);
				size = ftell(fp);
				buffer = (char*)malloc(size);
				memset(buffer, 0, size);
				fseek(fp, 0, SEEK_SET);
				count = fread(buffer, size, 1, fp);
				//int ss = strlen(buffer);
				//printf("%d\n", ss);
				
				snprintf(send_data, sizeof(send_data), "HTTP/1.0 %s\r\nConnection: close\r\nID: 20162800\r\nName: 이인규\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", status, size);

				//printf("%s\n", send_data);
				send(new_sock, send_data, strlen(send_data), 0);
				int a = send(new_sock, buffer, size, 0);
				printf("finish %d %d\n", size, a);
				fclose(fp);
				free(buffer);
			}
		}


		else{
			//printf("fail\n");
		}










		//for (;;) {

			//ReadHttpStatus(new_sock);

			//bytesread = recv(new_sock, request, 1024, 0);
			//if (bytesread<=0) {
				//printf("server: end of file on %d\n", new_sock);
				//if (close(new_sock)) 
					//perror("close");
				//break;
			//}
			//buf[bytesread] = '\0';
			//printf("%s", request);
			
			//for(i = 0; i < bytesread; i++)
				//buf[i] = toupper(buf[i]);
			
			/* echo it back */

			//if (write(new_sock, buf, bytesread) != bytesread)
				//perror("echo");
		//}



	}//end while
} /* main - hw2.c */



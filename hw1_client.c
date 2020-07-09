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

#define PROMPT() {printf("\n> ");fflush(stdout);}
#define GETCMD "dl"
#define QUITCMD "quit"

int ReadHttpStatus(int sock){
    char c;
    char buff[1024]="",*ptr=buff+1;
    int bytes_received, status;
    char *pt;
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

    sscanf(ptr,"%*s %d ", &status);

    if(strstr(ptr, "200") == NULL){
	char *tmp = strtok(ptr, " ");
	while(tmp != NULL){
	    tmp = strtok(NULL, " ");
	    if(tmp == NULL){return 0;}
	    printf("%s", tmp);
	}
    }
    
    //printf("%s\n",ptr);
    return (bytes_received>0)?status:0;

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
    //printf("%s",ptr);

    if(bytes_received){
        ptr=strstr(ptr,"Content-Length:");
        if(ptr){
            sscanf(ptr,"%*s %d",&bytes_received);

        }else
            bytes_received=-1; //unknown size

       printf("Total Size %d bytes\n",bytes_received);
    }
    return  bytes_received ;

}

int main(int argc, char *argv[]) {
	int socktoserver = -1;
	char buf[BUFSIZ];

	char fname[BUFSIZ];	

	printf("Student ID : 20162800\n");
	printf("Name : 이인규\n");

A:	PROMPT(); 

	for (;;) {
		if (!fgets(buf, BUFSIZ - 1, stdin)) {
			if (ferror(stdin)) {
				perror("stdin");
				exit(1);
			}
			exit(0);
		}

		char *cmd = strtok(buf, " \t\n\r");

		if((cmd == NULL) || (strcmp(cmd, "") == 0)) {
			PROMPT(); 
			continue;
		} else if(strcasecmp(cmd, QUITCMD) == 0) {
				exit(0);
		}

		if(!strcasecmp(cmd, GETCMD) == 0) {
			printf("Wrong command %s\n", cmd);
			PROMPT(); 
			continue;
		}

		// connect to a server
		char *hostname = strtok(NULL, " \t\n\r");
		char *pnum = strtok(NULL, " ");
		char *filename = strtok(NULL, " \t\n\r");

		// just for Debugging, print the URL
		//printf("%s:%s%s\n", hostname, pnum, filename);


		// NEED TO IMPLEMENT HERE

		char domain[100];
		char po[50];
		char path[100];
		strcpy(domain, hostname);
		strcpy(po, pnum);
		strcpy(path, filename);

		char filen[100];
		char real_file[50];
		strcpy(filen, path);
		char *ptr = strtok(filen, "/");
		while(ptr != NULL){
			strcpy(real_file,ptr);
			ptr = strtok(NULL, "/");
		}
		//printf("%s\n", real_file);


		int sock, bytes_received;  
    		char send_data[1024],recv_data[1024], *p;
    		struct sockaddr_in server_addr;
    		struct hostent *he;


    		he = gethostbyname(domain);
    		if (he == NULL){
		printf("%s: unknown host\n", hostname);
		printf("cannot connect to server %s %s", hostname, pnum);
       		//herror("gethostbyname");
       		//exit(1);
		goto A;
    		}

    		if ((sock = socket(AF_INET, SOCK_STREAM, 0))== -1){
       		perror("Socket");
       		exit(1);
    		}
    		server_addr.sin_family = AF_INET;     
    		server_addr.sin_port = htons(80);
    		server_addr.sin_addr = *((struct in_addr *)he->h_addr);
    		bzero(&(server_addr.sin_zero),8); 

    		if (connect(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
       			perror("Connect");
       			exit(1); 
    		}


		snprintf(send_data, sizeof(send_data), "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-agent: HW1/1.0\r\nID: 20162800\r\nName: 이인규\r\nConnection: close\r\n\r\n", path, domain);

    		if(send(sock, send_data, strlen(send_data), 0)==-1){
        		perror("send");
        		exit(2); 
    		}

		//fp=fopen("received_file","wb");

    		int contentlengh;

		if(ReadHttpStatus(sock) && (contentlengh=ParseHeader(sock))){

        		float bytes=0;
			float percent=0;
			int num = 0;
        		FILE* fd=fopen(real_file,"wb");

        		while(bytes_received = recv(sock, recv_data, 1024, 0)){
            			if(bytes_received==-1){
                		perror("recieve");
                		exit(3);
            			}


            			fwrite(recv_data,1,bytes_received,fd);
            			bytes+=bytes_received;
				percent = bytes/contentlengh*100;
				if(percent-num >= 10){
					printf("Current Downloading %.0f/%d (bytes) %.0f%%\n",bytes, contentlengh, percent);
					num +=10;
				}
			
            			if(bytes==contentlengh)
            			break;
        		}
        		fclose(fd);
    		}
		else{
			close(sock);
			goto A;
		}

		close(sock);
    		printf("Download Complete: %s, %d/%d\n",real_file, contentlengh, contentlengh );
		goto A;
	}
} 





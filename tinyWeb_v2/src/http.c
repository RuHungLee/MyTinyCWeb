#include "http.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

int listenfd;

char verification(reqInfo *req){
	
	//not general
	unsigned long h;
	char *e , *name , *pwd , *body = req->payload;
	char sn[100] , sp[100];
	FILE *fp;

	*(body + req->len) = 0;
	e = strstr(body , "&");
	*e = 0;
	if(name = strstr(body , "name=")){
		name += 5;
		if(pwd = strstr(e+1 , "pwd=")){
			pwd += 4;
			h = hash(pwd);
			fp = fopen(SHADOW , "r");
			if(fp == NULL){printf("failed");}
			printf("%ld\n" , h);
			while(fscanf(fp , "%s %s" , sn , sp)!=EOF){
				if(strcmp(sn , name) == 0){
					if(hash(sp) == h){
						printf("success\n");
						return 1;
					}
				}
			}
		}
	}
		
	return 0;
}

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void startSvr(const char *port){

    int connfd , cpid , option;
    struct sockaddr_in seraddr , cliaddr;
    socklen_t clilen;

	signal(SIGCHLD,SIG_IGN);
	
    printf(
            "Server started %shttp://127.0.0.1:%s%s\n",
            "\033[92m",port,"\033[0m"
            );

	//create socket.
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) > 0){
        printf("The socket was created! , the listenfd is %d\n" , listenfd);
    }else{perror("Error occured when creating socket......\n");}

    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = INADDR_ANY;
    seraddr.sin_port = htons(atoi(port));
	
	//bind socket on specified port.
    if(bind(listenfd , (SA *)&seraddr , sizeof(seraddr))!=0 || listen(listenfd , PADDINGSIZE) < 0){
        perror("Binding or Listening error......\n");
        exit(1);
	}

	//wait for syn packet and try to connect to client.
    for(;;){    
		if((connfd = accept(listenfd , (SA *)&cliaddr , &clilen))<0){
            if(errno == EINTR){
                continue;
            }else{
                perror("accept error!");
            }
        }
        if ((cpid = fork()) == 0) {
            close(listenfd);
            respond(connfd);
            exit(0);
        }
        close(connfd);          
	}
}


void respond(const int client){
	
	int rcvd , flags ;
	char *buf , *payload , *lenptr;
	reqInfo cliInfo;
	memset((void *)&cliInfo , 0 , sizeof(client));
	
	buf = malloc(65535);
	rcvd=read(client , buf , 65535);
	printf("rcvd : %d\n" , rcvd );	

	//rcvd = read(client , buf , 65535);
    if (rcvd<0){    // receive error
        fprintf(stderr,("recv() error\n"));
    }else if (rcvd==0){    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    }else{    // message received
        payload = strstr( buf, "\r\n\r\n" );
		lenptr	= strstr( buf , "Content-Length:");
        
		if(payload){
			payload += 4;
			cliInfo.payload = payload;
		}
		
		if(lenptr){
			lenptr += 16;
			cliInfo.len = atoi(lenptr);
		}
		
		cliInfo.fd = client;
        cliInfo.method = strtok(buf ,  " ");
        cliInfo.uri    = strtok(NULL, " ");
        cliInfo.prot   = strtok(NULL, " \r\n");
    }
	
	route(&cliInfo);
	shutdown(client, SHUT_RDWR);
	close(client);
	free(buf);
}


void saveFile(reqInfo *req , char *file){
	
    int fd;	
    fd = open(SAVEPIC , O_WRONLY | O_CREAT , 0755);
    write(fd , file , 65535);
    close(fd);

}

void parseJPG(reqInfo *req){
	char *bptr = strstr(req->payload , "\r\n\r\n")+4;
	if(bptr[0] == '\xff' && bptr[1] == '\xd8' && bptr[2] == '\xff' && bptr[3] == '\xe0'){
		saveFile(req , bptr);
	}else{
		printf("Can't identify the type of file...!");
	}
}

void page(reqInfo *req , const char *file){
  
    char *page;
    int fd , flag , len;
    char buffer[65536];
    
    if((fd = open(file , S_IRUSR)) == -1){
        
        dprintf(fd , "HTTP/1.1 301 Moved Permanently\r\n\r\n");
        dprintf(fd , "There is no fileQQ\n");
    
    }else{
        
        dprintf(req->fd , "HTTP/1.1 200 OK\r\n\r\n");
        while((len = read(fd , buffer , 65536))!=0){
			write(req->fd , buffer , len);
		}
    }

}


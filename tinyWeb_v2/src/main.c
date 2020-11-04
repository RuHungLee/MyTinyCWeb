#include "http.h"


int main(int argc , char **argv){

	startSvr(argv[1]);
	
	return 0;

}

void route(reqInfo *req){
	
	if(strcmp(req->method , "GET")==0){
		
		if(strcmp(req->uri , "/")==0){
			
			page(req , MAIN);
			printf("main...\n");
		
		}else if(strcmp(req->uri , "/upload")==0){
			
			page(req , UPLOAD);
			printf("upload...\n");
		
		}else if(strcmp(req->uri , "/robots")==0){
			
			page(req , FLAG);
			printf("flag...\n");

		}else{
		
			dprintf(req->fd , "HTTP/1.1 500\r\n\r\n");
			dprintf(req->fd , "There is no page~!\n");
		
		}
	
	}else if(strcmp(req->method , "POST")==0){
		/*
		if(strcmp(req->uri , "/")){
			//authentication
			if(verification(req)){
				;
			}else{
				page(req , MAIN);
			}

		}else 
		*/
		if(strcmp(req->uri , "/upload")==0){
			parseJPG(req);	
			page(req , UPLOAD);
		}
	}

}




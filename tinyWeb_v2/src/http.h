#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define SA struct sockaddr
#define BUFFERSIZE 1024
#define MAXLEN 8096
#define PADDINGSIZE 5
#define MAIN "/home/ruhung/networkProgramming/tinyWeb_v2/page/ruhung.html"
#define UPLOAD "/home/ruhung/networkProgramming/tinyWeb_v2/page/upload.html"
#define FLAG "/home/ruhung/networkProgramming/tinyWeb_v2/page/flag"
#define SHADOW "/home/ruhung/networkProgramming/tinyWeb_v2/shadow/shadow"
#define SAVEPIC "/home/ruhung/networkProgramming/tinyWeb_v2/database/success.jpg"
typedef struct reqInfo{
	int fd , len;
	char *prot , *method , *uri , *payload;
}reqInfo;
unsigned long hash(unsigned char *);
char verification(reqInfo *);
void parseJPG(reqInfo *);
void saveFile(reqInfo * , char *);
void startSvr(const char *);
void page(reqInfo * , const char *);
void respond(const int);
void route(reqInfo *);

#include "mylib.h"
#include <string.h>

int setup_client(char *hostname, in_port_t port){
    struct hostent *server_ent;
    struct sockaddr_in server;
    int soc;    

    if((server_ent = gethostbyname(hostname)) == NULL){
        perror("gethostbyname");
        return -1;
    }

    memset((char *)&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy((char *)&server.sin_addr,server_ent->h_addr,server_ent->h_length);

    if((soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        return -1;
    }

    if(connect(soc,(struct sockaddr *)&server,sizeof(server)) == -1){
        perror("connect");
        return -1;
    }
    fprintf(stderr, "connected.\n");

    return soc;
}


// setup_mserver.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

int mserver_socket(in_port_t port, int backlog) {
    struct sockaddr_in me;
    int soc_waiting;

    memset((char *)&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port = htons(port);

    if((soc_waiting = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("socket");
        return -1;
    }

    if(bind(soc_waiting,(struct sockaddr *)&me,sizeof(me)) == -1){
        perror("bind");
        return -1;
    }

    if(listen(soc_waiting,backlog) == -1){
        perror("listen");
        return -1;
    }
    
    fprintf(stderr,"successfully setup, now waiting.\n");

    return soc_waiting;
}

int mserver_maccept(int soc_waiting, int limit, void (*func)()) {
    int i;
    int fd;

    for(i=0; i<limit; i++) {
        if((fd = accept(soc_waiting, NULL ,NULL)) == -1) {
            fprintf(stderr, "accept error\n");
            return -1;
        }
        (*func)(i, fd);
    }
    close(soc_waiting);

    return fd;
}


#include "mylib.h"
#include <string.h>
#include<stdlib.h>

int setup_server(in_port_t port)
{
    struct sockaddr_in me;
    int soc_waiting;
    int soc;

    memset((char *)&me ,0,sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port = htons(port);

    // ソケット作成
    if((soc_waiting = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        exit(1);
    }

    // サーバーのアドレスをソケットに設定
    if(bind(soc_waiting,(struct sockaddr *)&me,sizeof(me))==-1){
        perror("bind");
        exit(1);
    }

    // ソケットで待ち受けることを設定
    listen(soc_waiting,1);
    fprintf(stderr,"successfully bound, now waiting.\n");

    // 接続要求が来るまで待つ
    soc = accept(soc_waiting,NULL,NULL);

    close(soc_waiting);

    return soc;
}
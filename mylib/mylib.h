#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

extern int setup_server(in_port_t port);
extern int setup_client(char *hostname, in_port_t port);
extern int mserver_socket(in_port_t port,int num);
extern int mserver_maccept(int soc, int limit, void (*func)());
extern int *chp_newline(char *buf, int len);
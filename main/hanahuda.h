#include <sys/types.h>
#include<GL/glut.h>
#include <GL/glpng.h>
#include "mylib.h"

#define PORT (in_port_t)50000
#define HOSTNAME_LENGTH 64

#define WINDOW_W 400
#define WINDOW_H 300
#define CARD_NUM 49 // 12*4 +1

GLuint cardimg[CARD_NUM];
pngInfo cardinfo[CARD_NUM];

void Display(void);
void Reshape(int,int);
void Timer(int);
void PutSprite(int,int,int,pngInfo *,double);
void readImg(void);
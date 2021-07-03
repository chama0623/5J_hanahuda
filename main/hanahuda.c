#include"hanahuda.h"
#include<GL/glut.h>
#include <GL/glpng.h>
#include<stdio.h>
#include <stdlib.h>

void Display(void){
    // 描画クリア
    //glClear(GL_COLOR_BUFFER_BIT);
    PutSprite(cardimg[0],10,10,&cardinfo[0],1);
    // 描画の反映
    glFlush();
}

// windowサイズが変更されたときの処理
void Reshape(int w,int h){
    //printf("ウィンドウの幅と高さ=%d x %d\n",w,h);
    glViewport(0,0,w,h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);
    glScaled(1,-1,1);
    glTranslated(0,-h,0);

    //windowサイズ固定 
    glutReshapeWindow(WINDOW_W,WINDOW_H);
}

// タイマーの処理
void Timer(int value){
    glutPostRedisplay();
    glutTimerFunc(500,Timer,0);
}

// (x,y)に大きさscaleの画像を表示
void PutSprite(int num, int x, int y, pngInfo *info,double scale)
{
    int w, h;  //  テクスチャの幅と高さ

    w = info->Width*scale;   //  テクスチャの幅と高さを取得する
    h = info->Height*scale;

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, num);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);  //  幅w, 高さhの四角形

    glTexCoord2i(0, 0); 
    glVertex2i(x, y);

    glTexCoord2i(0, 1);
    glVertex2i(x, y + h);

    glTexCoord2i(1, 1);
    glVertex2i(x + w, y + h);

    glTexCoord2i(1, 0);
    glVertex2i(x + w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void readImg(void){
    char fname[100];
    sprintf(fname,".\\card\\jan-%d.png",1);
    cardimg[0] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
       &cardinfo[0], GL_CLAMP, GL_NEAREST, GL_NEAREST);

}
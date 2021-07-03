#include "hanahuda.h"
#include<GL/glut.h>
#include <GL/glpng.h>

int main(int argc,char **argv){
    int soc;
    // 初期化処理
    // 引数処理
    glutInit(&argc,argv);
    // 初期Windowサイズ設定
    glutInitWindowSize(WINDOW_W,WINDOW_H);
    // 新規Window作成
    glutCreateWindow("test");
    // 関数登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    //glutMouseFunc(Mouse);
    glutTimerFunc(500,Timer,0);
    // display初期化
    glutInitDisplayMode(GLUT_RGBA);
    glClearColor(0.96,0.96,1.0,1.0);
    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    readImg();

    if( (soc=setup_server(PORT))==-1){
        exit(1);
    }
    // glutのメインループ
    glutMainLoop();
    return 0;
}
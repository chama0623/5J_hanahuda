#include<stdio.h>
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
    glutCreateWindow("hanahuda_host");
    // 関数登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutPassiveMotionFunc(PassiveMotion);
    glutTimerFunc(500,Timer,0);
    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // display初期化
    glutInitDisplayMode(GLUT_RGBA);
    glClearColor(0.28,0.48,0.32,1.0);
    if( (soc=setup_server(PORT))==-1){
        exit(1);
    }

    // ゲーム初期化
    game_init(soc,0);

    // glutのメインループ
    glutMainLoop();
    return 0;
}
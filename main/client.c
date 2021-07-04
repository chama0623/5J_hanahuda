#include<stdio.h>
#include "hanahuda.h"
#include<GL/glut.h>
#include <GL/glpng.h>

int main(int argc,char **argv){
    int soc;
    char hostname[64];
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
    glutMouseFunc(Mouse);
    glutPassiveMotionFunc(PassiveMotion);
    glutTimerFunc(500,Timer,0);
    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // display初期化
    glutInitDisplayMode(GLUT_RGBA);
    glClearColor(0.0,1.0,0.0,1.0);
    //サーバーのホスト名の入力
    printf("Input server's hostname: ");
    fgets(hostname,HOSTNAME_LENGTH,stdin);
    chop_newline(hostname,HOSTNAME_LENGTH);

    //接続完了まで
    if((soc = setup_client(hostname,PORT)) == -1){
        exit(1);
    }

    // ゲーム初期化
    game_init(soc,1);
    
    // glutのメインループ
    glutMainLoop();

    return 0;
}
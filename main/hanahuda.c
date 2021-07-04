#include"hanahuda.h"
#include<GL/glut.h>
#include <GL/glpng.h>
#include<stdio.h>
#include <stdlib.h>
#include <time.h>

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

// 画像読み込み
void readImg(void){
    char fname[100];
    int i,j;
    int cnt=0;
    // 全カード読み込み
    for(i=0;i<MONTH;i++){
        for(j=0;j<MONTH_CARD;j++){
            sprintf(fname,".\\card\\%s-%d.png",month[i],j+1);
            //printf("%s\n",fname);
            cardimg[cnt] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
                &cardinfo[cnt], GL_CLAMP, GL_NEAREST, GL_NEAREST);
            cnt++;
        }
    }
    // 裏返しのカード(back)を読み込み
    sprintf(fname,".\\card\\back.png");
    cardimg[cnt] = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &cardinfo[cnt], GL_CLAMP, GL_NEAREST, GL_NEAREST);    

}

// カード構造体の状態を表示
void card_status(void){
    int i;
    printf("month | num | rank | have\n");
    for(i=0;i<CARD_NUM;i++){
        printf("%d  | %d | %d | %d\n",cards[i].month,cards[i].num,cards[i].rank,cards[i].have);
    }
}

// カードの情報をカード構造体に格納
void card_init(void){
    int i,j;
    int cnt=0;
    // すべてのカードについて
    for(i=1;i<=MONTH;i++){
        for(j=0;j<MONTH_CARD;j++){
            cards[cnt].month=i;
            cards[cnt].num=j+1;
            cards[cnt].rank=cardrank[i-1][j];
            cards[cnt].have=0;
            cnt++;
        }
    }
    //card_status();
}

// カードをシャッフル
void shuffle(void){
    srand((unsigned) time(NULL));
    int i,rnd1,rnd2,tmp;
    // 全てのカードを山札に格納
    for(i=0;i<CARD_NUM;i++){
        deck[i]=i;
    }

    for(i=0;i<SHUFFLE_TIME;i++){
        rnd1 = rand()%CARD_NUM;
        rnd2 = rand()%CARD_NUM;
        tmp = deck[rnd1];
        deck[rnd1] = deck[rnd2];
        deck[rnd2]=tmp;
    }
    /*
    for(i=0;i<CARD_NUM;i++){
        printf("%d,%d\n",i,deck[i]);
    }*/
}

// 山札からカードを出す処理
// カードのインデックスを返す
int popDeck(void){
    deck_num++;
    return deck[deck_num-1];
}

// 場にカードを入れる処理
int pushPlace(int c){
    place[place_num++]=c;
}

// 場と手札にカードを配る処理
void arrangeCard(void){
    int i;
    // 場,親,子に8枚カードを出す
    for(i=0;i<INIT_PLACE;i++){
        pushPlace(popDeck());
        mycard[i] = popDeck();
        peercard[i] = popDeck();
    }


    printf("place_num = %d\n",place_num);
    printf("mycard_num = %d\n",mycard_num);
    printf("peercard_num = %d\n",peercard_num);
    printf("deck_num = %d\n",deck_num);
    printf("index | place | my | peer\n");
    for(i=0;i<INIT_PLACE;i++){
        printf("%d | %d | %d | %d\n",i,place[i],mycard[i],peercard[i]);
    }
} 


// ゲーム初期化
// role 0:親,1:子
void game_init(int soc,int argrole){
    hanahuda_soc = soc;
    role=argrole;
    readImg(); // 画像読み込み
    card_init(); // カード情報をカード構造体に格納
    shuffle(); // 山札シャッフル
    arrangeCard(); // 場, 手札にカードを配る
}

// カードがとれるか判定する処理
int istake(int card1,int card2){
    if(cards[card1].month==cards[card2].month){
        return 1;
    }
    return 0;
}

void Mouse(int b,int s,int x,int y){
    if(b==GLUT_LEFT_BUTTON){
        if(s==GLUT_UP){
            beforeclickedCard = clickedCard;
            clickedCard = calWhichMycard(x,y);
            if(clickedCard==-1){
                clickedPlaceCard = calWhichPlacecard(x,y);
                clickedCard = beforeclickedCard;
            }
            // カードがとれないとき
            if((clickedCard!=-1)&&(clickedPlaceCard!=-1)){
                if(cards[mycard[clickedCard]].month != cards[place[clickedPlaceCard]].month){
                    clickedPlaceCard=-1;
                }else{
                    turn=2;
                }
            }
        }
    }
    //printf("clickedCard = %d\n",clickedCard);
}

// 場札のどのカードの上にマウスがあるか計算する処理
int calWhichPlacecard(int x,int y){
    int index=-1;
    // 上段の判定
    if((220<y)&&(y<300)){
        if((200<=x)&&(x<=400)){
            index=x-200;
            index/=50;
            if(place_num<4){
                if(index>=place_num){
                    index=-1;
                }
            }
        }
        return index;
    }
    // 下段の判定
    if(place_num>4){
        if((300<y)&&(y<380)){
            if((200<=x)&&(x<=400)){
                index=x-200;
                index/=50;
                index+=4;
                if(index>=place_num){
                    index=-1;
                }        
            }
        }
    }
    return index;
}


// 手札のどのカードの上にマウスがあるか計算する処理
// -1:手札の上にマウスがない
// 0～7:手札の番号
int calWhichMycard(int x,int y){
    int index;
    if(y<410||y>490){
        index=-1;
    }else if((100<=x)&&(x<=500)){
        index=x-100;
        index/=50;
        if(index>=mycard_num){
            index=-1;
        }
    }else{
        index=-1;
    }
    return index;
}

// 指定された座標をカードの大きさで囲む
void surroundCard(int x,int y,int R,int G,int B){
    glLineWidth(2.0);
    glColor3ub(R,G,B);
    glBegin(GL_LINE_STRIP);
    glVertex2i(x-1,y-1);
    glVertex2i(x-1,y+80);
    glVertex2i(x+48,y+80);
    glVertex2i(x+48,y-1);
    glVertex2i(x-1,y-1);
    glEnd();
    glColor3ub(255,255,255);
    glLineWidth(1.0);
}

void PassiveMotion(int x,int y){
    //printf("passive(%d,%d)\n",x,y);
    selectedCard = calWhichMycard(x,y);
    selectedPlaceCard = calWhichPlacecard(x,y);
}

void Display(void){
    int i;
    int x,y;
    // 描画クリア
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 場のカードを描画
    x=200;
    y=220;

    glBegin(GL_LINES);
    glVertex2i(300,0);
    glVertex2i(300,600);
    glVertex2i(0,300);
    glVertex2i(600,300);
    glEnd();

    for(i=0;i<place_num;i++){
        PutSprite(cardimg[place[i]],x,y,&cardinfo[place[i]],0.5);
        x+=50;
        if(i!=0 && i%4==3){
            y+=80;
            x=200;
        }
    }

    // 山札を描画
    PutSprite(cardimg[ALL_CARD-1],500,260,&cardinfo[ALL_CARD-1],0.5);

    // 自分の手札を描画
    x=100;
    y=410;
    for(i=0;i<mycard_num;i++){
        if(role==0){
            PutSprite(cardimg[mycard[i]],x,y,&cardinfo[mycard[i]],0.5);
        }else{
            PutSprite(cardimg[peercard[i]],x,y,&cardinfo[peercard[i]],0.5);
        }
        x+=50;
    }

    // 相手の手札を裏面にして描画
    x=100;
    y=110;
    for(i=0;i<peercard_num;i++){
        PutSprite(cardimg[ALL_CARD-1],x,y,&cardinfo[ALL_CARD-1],0.5);
        x+=50;
    }

    //カード選択
    if((turn==0)&&(role==0)){
        if(selectedCard!=-1){
            surroundCard(100+selectedCard*50,410,255,0,0);
        }
        if(clickedCard!=-1){
            surroundCard(100+clickedCard*50,410,0,0,255);
        }
        if(selectedPlaceCard!=-1){
            //下段
            if(selectedPlaceCard>=4){
                surroundCard(200+(selectedPlaceCard-4)*50,300,255,0,0);
            }else{ // 上段
                surroundCard(200+selectedPlaceCard*50,220,255,0,0);
            }
        }
        if(clickedPlaceCard!=-1){
            //下段
            if(clickedPlaceCard>=4){
                surroundCard(200+(clickedPlaceCard-4)*50,300,0,0,255);
            }else{ // 上段
                surroundCard(200+clickedPlaceCard*50,220,0,0,255);
            }            
        }
    }else if((turn==1)&&(role==1)){
        if(selectedCard!=-1){
            surroundCard(100+selectedCard*50,410,255,0,0);
        }
        if(clickedCard!=-1){
            surroundCard(100+clickedCard*50,410,0,0,255);
        }
        if(selectedPlaceCard!=-1){
            //下段
            if(selectedPlaceCard>=4){
                surroundCard(200+(selectedPlaceCard-4)*50,300,255,0,0);
            }else{ // 上段
                surroundCard(200+selectedPlaceCard*50,220,255,0,0);
            }
        }
        if(clickedPlaceCard!=-1){
            //下段
            if(clickedPlaceCard>=4){
                surroundCard(200+(clickedPlaceCard-4)*50,300,0,0,255);
            }else{ // 上段
                surroundCard(200+clickedPlaceCard*50,220,0,0,255);
            }            
        }
    }

    if(turn==2){
        printf("aaa\n");
    }
    // 描画の反映
    glFlush();
}
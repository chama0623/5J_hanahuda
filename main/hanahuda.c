#include"hanahuda.h"
#include<GL/glut.h>
#include <GL/glpng.h>
#include<stdio.h>
#include <stdlib.h>
#include <time.h>

void Reshape(int w,int h){
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
    glutTimerFunc(100,Timer,0);
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

    sprintf(fname,".\\koikoi.png");
    koikoiimg = pngBind(fname, PNG_NOMIPMAP, PNG_ALPHA, 
        &koikoiinfo, GL_CLAMP, GL_NEAREST, GL_NEAREST);        
}

// 配列の初期化
void array_init(void){
    int i;
    // 配列の初期化
    for(i=0;i<INIT_PLACE;i++){
        mycard[i]=-1;
        peercard[i]=-1;
    }
    for(i=0;i<CARD_NUM/2;i++){
        mygetcard[i]=-1;
        peergetcard[i]=-1;
    }
    for(i=0;i<PLACE_MAX;i++){
        place[i]=-1;
    }
    for(i=0;i<YAKU_NUM;i++){
        myyaku[i]=0;
        peeryaku[i]=0;
    }    
}

// ゲーム初期化
// role 0:親,1:子
void game_init(int soc,int argrole){
    int tmp=1;
    hanahuda_soc = soc;
    role=argrole;
    
    array_init();
    readImg(); // 画像読み込み
    card_init(); // カード情報をカード構造体に格納
    while(tmp==1){
        shuffle(); // 山札シャッフル
        tmp=arrangeCard(); // 場, 手札にカードを配る
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
            cnt++;
        }
    }
}

// カードをシャッフル
void shuffle(void){

    int i,rnd1,rnd2,tmp;
    srand((unsigned) time(NULL));
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
}

// 場と手札にカードを配る処理
// 場札に同じ月のカードが3枚以上あるとき1,それ以外のとき0を返す.
int  arrangeCard(void){
    int i,j,tmp;
    for(i=0;i<PLACE_MAX;i++){
        place[i]=-1;
    }
    // 場,親,子に8枚カードを出す
    for(i=0;i<INIT_PLACE;i++){
        pushPlace(popDeck());
        mycard[i] = popDeck();
        peercard[i] = popDeck();
    }
        // カードチェック
    for(i=1;i<=MONTH;i++){
        tmp=0;
        for(j=0;j<place_num;j++){
            if(cards[place[j]].month==i){
                tmp++;
            }
        }
        if(tmp>=3){
            tmp=1;
            return tmp;
        }
    }
    tmp=0;
    return tmp;
}

// 山札からカードを出す処理
// カードのインデックスを返す
int popDeck(void){
    deck_num++;
    return deck[deck_num-1];
}

// 場にカードを入れる処理
void pushPlace(int c){
    place[place_num++]=c;
}

// 場からカードを出す処理
int popPlace(int index){
    int i;
    int tmp=place[index];
    for(i=index+1;i<place_num;i++){
        place[i-1]=place[i];
    }
    place[i-1]=-1;
    place_num--;
    return tmp;
}

// 手札にカードを入れる処理
void pushHandCard(int c){
    if(role==0){
        mycard[mycard_num] = c;
        mycard_num++;
    }else{
        peercard[peercard_num] = c;
        peercard_num++;
    }
}

// 手札からカードを出す処理
int popHandCard(int index){
    int i;
    int tmp;
    if(role==0){
        tmp = mycard[index];
        for(i=index+1;i<mycard_num;i++){
            mycard[i-1]=mycard[i];
        }
        mycard[mycard_num-1]=-1;
        mycard_num--;
    }else{
        tmp = peercard[index];
        for(i=index+1;i<peercard_num;i++){
            peercard[i-1]=peercard[i];
        }
        peercard[peercard_num-1]=-1;
        peercard_num--;        
    }
    return tmp;    
}

// 持ち札にカードを入れる処理
void pushgetCard(int c){
    if(role==0){
        mygetcard[mygetcard_num]=c;
        mygetcard_num++;
    }else{
        peergetcard[peergetcard_num]=c;
        peergetcard_num++;
    }
}

// 場札のどのカードの上にマウスがあるか計算する処理
int calWhichPlacecard(int x,int y){
    int index=-1;
    // 上段の判定
    if((220<y)&&(y<300)){
        if((200<=x)&&(x<=500)){
            index=x-200;
            index/=50;
            if(place_num<6){
                if(index>=place_num){
                    index=-1;
                }
            }
        }
        return index;
    }
    // 下段の判定
    if(place_num>6){
        if((300<y)&&(y<380)){
            if((200<=x)&&(x<=500)){
                index=x-200;
                index/=50;
                index+=6;
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
        if(role==0){
            if(index>=mycard_num){
                index=-1;
            }
        }else{
            if(index>=peercard_num){
                index=-1;
            }            
        }
    }else{
        index=-1;
    }
    return index;
}

// 選択されている手札のカードで取れる場のカードがあるか
// ある:取れる枚数
// ない:0
// index:自分のカードのmycardにおけるインデックス
int isGetCard(int index){
    int i;
    int flag=0;
    for(i=0;i<place_num;i++){
        if(role==0){
            if(cards[place[i]].month==cards[mycard[index]].month){
                flag++;
            }
        }else{
            if(cards[place[i]].month==cards[peercard[index]].month){
                flag++;
            }
        }
    }
    return flag;
}

int isGetPlace(int c){
    int i;
    int flg=0;
    for(i=0;i<place_num;i++){
        if(cards[c].month==cards[place[i]].month){
            flg++;
        }
    }
    return flg;
}

void Keyboard(unsigned char key,int x,int y){
    if(status==4){
        if(key=='y'){ // こいこいするとき
            if(role==0){
                mykoikoi=1;
            }else{
                peerkoikoi=1;
            }
        }else if(key=='n'){ // こいこいしないとき
            if(role==0){
                mykoikoi=0;
            }else{
                peerkoikoi=0;
            }
        }

    }
}

void Mouse(int b,int s,int x,int y){
    int tmpclick=-1;
    int tmpclickplace=-1;
    if((turn==0)&&(role==0)){
        if(b==GLUT_LEFT_BUTTON){
            if(s==GLUT_UP){
                tmpclick = calWhichMycard(x,y);
                tmpclickplace = calWhichPlacecard(x,y);
                if(tmpclick!=-1){
                    clickedCard = tmpclick;
                }
                if(tmpclickplace!=-1){
                    clickedPlaceCard=tmpclickplace;
                }            
                // カードを場札に捨てる状態のとき
                if((clickedCard!=-1)&&(clickedPlaceCard==-1)){
                    if(isGetCard(clickedCard)==0){
                        status=2;
                    }
                }else if((clickedCard!=-1)&&(clickedPlaceCard!=-1)){// カードを取れる状態のとき
                    if(cards[mycard[clickedCard]].month != cards[place[clickedPlaceCard]].month){
                        clickedPlaceCard=-1;
                    }else{
                        status=1;
                    }
                }
            }
        }
    }else if((turn==1)&&(role==1)){
        if(b==GLUT_LEFT_BUTTON){
            if(s==GLUT_UP){
                tmpclick = calWhichMycard(x,y);
                tmpclickplace = calWhichPlacecard(x,y);
                if(tmpclick!=-1){
                    clickedCard = tmpclick;
                }
                if(tmpclickplace!=-1){
                    clickedPlaceCard=tmpclickplace;
                }            
                // カードを場札に捨てる状態のとき
                if((clickedCard!=-1)&&(clickedPlaceCard==-1)){
                    if(isGetCard(clickedCard)==0){
                        status=2;
                    }
                }else if((clickedCard!=-1)&&(clickedPlaceCard!=-1)){// カードを取れる状態のとき
                    if(cards[peercard[clickedCard]].month != cards[place[clickedPlaceCard]].month){
                        clickedPlaceCard=-1;
                    }else{
                        status=1;
                    }
                }
            }
        }
    }
}

// マウスモーション
void PassiveMotion(int x,int y){
    selectedCard = calWhichMycard(x,y);
    selectedPlaceCard = calWhichPlacecard(x,y);
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

// カードを描画
void PaintCards(void){
    int i,j,num;
    int x,y;
    // 場のカードを描画
    x=200;
    y=220;

    // 場を描画
    for(i=0;i<place_num;i++){
        PutSprite(cardimg[place[i]],x,y,&cardinfo[place[i]],0.5);
        x+=50;
        if(i!=0 && i%6==5){
            y+=80;
            x=200;
        }
    }

    // 山札を描画
    PutSprite(cardimg[ALL_CARD-1],100,260,&cardinfo[ALL_CARD-1],0.5);

    // 自分の手札を描画
    x=100;
    y=410;
    if(role==0){
        num=mycard_num;
    }else{
        num=peercard_num;
    }
    for(i=0;i<num;i++){
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
    if(role==0){
        num=peercard_num;
    }else{
        num=mycard_num;
    }
    for(i=0;i<num;i++){
        PutSprite(cardimg[ALL_CARD-1],x,y,&cardinfo[ALL_CARD-1],0.5);
        x+=50;
    }

    y=510;
    x=10;
    // 自分の持ち札を描画
    if(role==0){
        num=mygetcard_num;
    }else{
        num=peergetcard_num;
    }    
    for(i=4;i>=1;i--){ // 札の種類別に
        for(j=0;j<num;j++){
            if(role==0){
                if(cards[mygetcard[j]].rank==i){
                    PutSprite(cardimg[mygetcard[j]],x,y,&cardinfo[mygetcard[j]],0.5);
                    x+=20;
                }
            }else{
                if(cards[peergetcard[j]].rank==i){
                    PutSprite(cardimg[peergetcard[j]],x,y,&cardinfo[peergetcard[j]],0.5);
                    x+=20;
                }
            }
        }
        x=10+150*(5-i);
    }

    y=10;
    x=10;
    // 相手の持ち札を描画
    if(role==0){
        num=peergetcard_num;
    }else{
        num=mygetcard_num;
    }    
    for(i=4;i>=1;i--){ // 札の種類別に
        for(j=0;j<num;j++){
            if(role==1){
                if(cards[mygetcard[j]].rank==i){
                    PutSprite(cardimg[mygetcard[j]],x,y,&cardinfo[mygetcard[j]],0.5);
                    x+=20;
                }
            }else{
                if(cards[peergetcard[j]].rank==i){
                    PutSprite(cardimg[peergetcard[j]],x,y,&cardinfo[peergetcard[j]],0.5);
                    x+=20;
                }
            }
        }
        x=10+150*(5-i);
    }

    if(status==0){
        if(turn==0){
            if(role==0){
                if(selectedCard!=-1){
                    surroundCard(100+selectedCard*50,410,255,0,0);
                }
                if(clickedCard!=-1){
                    surroundCard(100+clickedCard*50,410,0,0,255);
                }
                if(selectedPlaceCard!=-1){
                    //下段
                    if(selectedPlaceCard>=6){
                        surroundCard(200+(selectedPlaceCard-6)*50,300,255,0,0);
                    }else{ // 上段
                        surroundCard(200+selectedPlaceCard*50,220,255,0,0);
                    }
                }
                if(clickedPlaceCard!=-1){
                    //下段
                    if(clickedPlaceCard>6){
                        surroundCard(200+(clickedPlaceCard-6)*50,300,0,0,255);
                    }else{ // 上段
                        surroundCard(200+clickedPlaceCard*50,220,0,0,255);
                    }            
                }
            }
        }else{
            if(role==1){
                if(selectedCard!=-1){
                    surroundCard(100+selectedCard*50,410,255,0,0);
                }
                if(clickedCard!=-1){
                    surroundCard(100+clickedCard*50,410,0,0,255);
                }
                if(selectedPlaceCard!=-1){
                    //下段
                    if(selectedPlaceCard>=6){
                        surroundCard(200+(selectedPlaceCard-6)*50,300,255,0,0);
                    }else{ // 上段
                        surroundCard(200+selectedPlaceCard*50,220,255,0,0);
                    }
                }
                if(clickedPlaceCard!=-1){
                    //下段
                    if(clickedPlaceCard>=6){
                        surroundCard(200+(clickedPlaceCard-6)*50,300,0,0,255);
                    }else{ // 上段
                        surroundCard(200+clickedPlaceCard*50,220,0,0,255);
                    }            
                }
            }
        }
    }
}

void checkYaku(void){
    int i,num;
    int checkkou=0;
    int flgrain=0; // 雨の光札用フラグ
    int hanami=0;
    int tukimi=0;
    int inosikacho=0;
    int akatan=0;
    int aotan=0;
    int tane=0;
    int tan=0;
    int kasu=0;
    if(role==0){
        for(i=0;i<mygetcard_num;i++){
        // 光札の数をカウント
        if(cards[mygetcard[i]].rank==4){
            // 雨かどうか
            if(cards[mygetcard[i]].month==11){
                flgrain=1;
            }
            checkkou++;
        }
        // 盃をチェック
        if((cards[mygetcard[i]].month==9)&&(cards[mygetcard[i]].rank==3)){
            hanami++;
            tukimi++;
            kasu++;
        }
        // 花見チェック
        if((cards[mygetcard[i]].month==3)&&(cards[mygetcard[i]].rank==4)){
            hanami++;
        }
        // 月見チェック
        if((cards[mygetcard[i]].month==8)&&(cards[mygetcard[i]].rank==4)){
            tukimi++;
        }
        // 猪鹿蝶チェック
        // 猪
        if((cards[mygetcard[i]].month==7)&&(cards[mygetcard[i]].rank==3)){
            inosikacho++;
        }
        // 鹿
        if((cards[mygetcard[i]].month==10)&&(cards[mygetcard[i]].rank==3)){
            inosikacho++;
        }
        // 蝶
        if((cards[mygetcard[i]].month==6)&&(cards[mygetcard[i]].rank==3)){
            inosikacho++;
        }

        // 赤短チェック
        if((cards[mygetcard[i]].month==1)&&(cards[mygetcard[i]].rank==2)){
            akatan++;
        }
        if((cards[mygetcard[i]].month==2)&&(cards[mygetcard[i]].rank==2)){
            akatan++;
        }
        if((cards[mygetcard[i]].month==3)&&(cards[mygetcard[i]].rank==2)){
            akatan++;
        }

        // 青短チェック
        if((cards[mygetcard[i]].month==6)&&(cards[mygetcard[i]].rank==2)){
            aotan++;
        }
        if((cards[mygetcard[i]].month==9)&&(cards[mygetcard[i]].rank==2)){
            aotan++;
        }
        if((cards[mygetcard[i]].month==10)&&(cards[mygetcard[i]].rank==2)){
            aotan++;
        }
        // 取得した札の枚数をカウント
        if(cards[mygetcard[i]].rank==3){
            tane++;
        }
        if(cards[mygetcard[i]].rank==2){
            tan++;
        }
        if(cards[mygetcard[i]].rank==1){
            kasu++;
        }
        }
       // 配列に役を格納
        if(checkkou==5){  // 五光 
            myyaku[0]=1; 
            myyaku[1]=0; // 四光, 雨四光, 三光を破棄
            myyaku[2]=0;
            myyaku[3]=0;
        }
        if((checkkou==4)&&(flgrain==0)){ // 四光
            myyaku[1]=1;
            myyaku[2]=0;
            myyaku[3]=0;  // 三光を破棄
        }
        if((checkkou==4)&&(flgrain==1)){ // 雨四光 
            myyaku[1]=0;
            myyaku[2]=1;
            myyaku[3]=0;  // 三光を破棄
        }
        if((checkkou==3)&&(flgrain==0)){ myyaku[3]=1; } // 三光
        if(hanami==2){ myyaku[4]=1;} // 花見で一杯
        if(tukimi==2){ myyaku[5]=1;} // 月見で一杯
        if(inosikacho==3){ myyaku[6]=1;} // 猪鹿蝶
        if(akatan==3){ myyaku[7]=1;} // 赤短
        if(aotan==3){ myyaku[8]=1;} // 青短
        if(tane>=5){// タネ 
            myyaku[9]=tane-4;
        }else{
            myyaku[9]=0;
        }
        if(tan>=5){ // タン 
            myyaku[10]=tan-4;
        }else{
            myyaku[10]=0;
        }
        if(kasu>=10){ // カス
            myyaku[11]=kasu-9;
        }else{
            myyaku[11]=0;
        }
    }else{
        for(i=0;i<peergetcard_num;i++){
        // 光札の数をカウント
        if(cards[peergetcard[i]].rank==4){
            // 雨かどうか
            if(cards[peergetcard[i]].month==11){
                flgrain=1;
            }else{
                checkkou++;
            }
        }
        // 盃をチェック
        if((cards[peergetcard[i]].month==9)&&(cards[peergetcard[i]].rank==3)){
            hanami++;
            tukimi++;
        }
        // 花見チェック
        if((cards[peergetcard[i]].month==3)&&(cards[peergetcard[i]].rank==4)){
            hanami++;
        }
        // 月見チェック
        if((cards[peergetcard[i]].month==8)&&(cards[peergetcard[i]].rank==4)){
            tukimi++;
        }
        // 猪鹿蝶チェック
        // 猪
        if((cards[peergetcard[i]].month==7)&&(cards[peergetcard[i]].rank==3)){
            inosikacho++;
        }
        // 鹿
        if((cards[peergetcard[i]].month==10)&&(cards[peergetcard[i]].rank==3)){
            inosikacho++;
        }
        // 蝶
        if((cards[peergetcard[i]].month==6)&&(cards[peergetcard[i]].rank==3)){
            inosikacho++;
        }

        // 赤短チェック
        if((cards[peergetcard[i]].month==1)&&(cards[peergetcard[i]].rank==2)){
            akatan++;
        }
        if((cards[peergetcard[i]].month==2)&&(cards[peergetcard[i]].rank==2)){
            akatan++;
        }
        if((cards[peergetcard[i]].month==3)&&(cards[peergetcard[i]].rank==2)){
            akatan++;
        }

        // 青短チェック
        if((cards[peergetcard[i]].month==6)&&(cards[peergetcard[i]].rank==2)){
            aotan++;
        }
        if((cards[peergetcard[i]].month==9)&&(cards[peergetcard[i]].rank==2)){
            aotan++;
        }
        if((cards[peergetcard[i]].month==10)&&(cards[peergetcard[i]].rank==2)){
            aotan++;
        }
        // 取得した札の枚数をカウント
        if(cards[peergetcard[i]].rank==3){
            tane++;
        }
        if(cards[peergetcard[i]].rank==2){
            tan++;
        }
        if(cards[peergetcard[i]].rank==1){
            kasu++;
        }
        }
        // 配列に役を格納
        if(checkkou==5){  // 五光 
            peeryaku[0]=1; 
            peeryaku[1]=0; // 四光, 雨四光, 三光を破棄
            peeryaku[2]=0;
            peeryaku[3]=0;
        }
        if((checkkou==4)&&(flgrain==0)){ // 四光
            peeryaku[1]=1;
            peeryaku[2]=0;
            peeryaku[3]=0;  // 三光を破棄
        }
        if((checkkou==4)&&(flgrain==1)){ // 雨四光 
            peeryaku[1]=0;
            peeryaku[2]=1;
            peeryaku[3]=0;  // 三光を破棄
        }
        if((checkkou==3)&&(flgrain==0)){ peeryaku[3]=1; } // 三光
        if(hanami==2){ peeryaku[4]=1;} // 花見で一杯
        if(tukimi==2){ peeryaku[5]=1;} // 月見で一杯
        if(inosikacho==3){ peeryaku[6]=1;} // 猪鹿蝶
        if(akatan==3){ peeryaku[7]=1;} // 赤短
        if(aotan==3){ peeryaku[8]=1;} // 青短
        if(tane>=5){// タネ 
            peeryaku[9]=tane-4;
        }else{
            peeryaku[9]=0;
        }
        if(tan>=5){ // タン 
            peeryaku[10]=tan-4;
        }else{
            peeryaku[10]=0;
        }
        if(kasu>=10){ // カス
            peeryaku[11]=kasu-9;
        }else{
            peeryaku[11]=0;
        }
    }
}

int calcPoint(void){
    int point=0;
    int i;
    for(i=0;i<YAKU_NUM;i++){
        if(role==0){
            point+=myyaku[i]*pointlist[i];
        }else{
            point+=peeryaku[i]*pointlist[i];
        }
    }
    return point;
}

void printYaku(void){
    int i,point;
    printf("---------------\n");
    for(i=0;i<YAKU_NUM;i++){
        if(role==0){
            if(myyaku[i]!=0){
                printf("%s : %d点\n", yakuname[i],pointlist[i]*myyaku[i]);
            }
        }else{
            if(peeryaku[i]!=0){
                printf("%s : %d点\n", yakuname[i],pointlist[i]*peeryaku[i]);
            }
        }
    }
    point=calcPoint();

    if(role==0){
        if(peerkoikoi==1){
            point*=2;
            printf("相手こいこい得点2倍 ×2\n");
        }
    }else{
        if(mykoikoi==1){
            point*=2;
            printf("相手こいこい得点2倍 ×2\n");
        }
    }

    if(point>=7){
        point*=2;
        printf("7点以上得点2倍 ×2\n");
    }

    printf("合計 : %d点\n",point);
}

void setPacket(int *data){
    int i,j;
    for(i=0;i<17;i++){
        if(i==0){ // turn
            data[packet_sep[i]]=turn;
        }else if(i==1){
            for(j=0;j<CARD_NUM;j++){
                data[packet_sep[i]+j] = deck[j];
            }               
        }else if(i==2){ // place
            for(j=0;j<PLACE_MAX;j++){
                data[packet_sep[i]+j] = place[j];
            }           
        }else if(i==3){ // mycard 
            for(j=0;j<INIT_PLACE;j++){
                data[packet_sep[i]+j] = mycard[j];
            }
        }else if(i==4){ // peercard
            for(j=0;j<INIT_PLACE;j++){
                data[packet_sep[i]+j] = peercard[j];
            }                
        }else if(i==5){ // mygetcard
            for(j=0;j<CARD_NUM/2;j++){
                data[packet_sep[i]+j] = mygetcard[j];
            }    
        }else if(i==6){
            for(j=0;j<CARD_NUM/2;j++){ // peergetcard
                data[packet_sep[i]+j] = peergetcard[j];
            }            
        }else if(i==7){
            data[packet_sep[i]] = deck_num;
        }else if(i==8){
            data[packet_sep[i]] = place_num;
        }else if(i==9){
            data[packet_sep[i]] = mycard_num;
        }else if(i==10){
            data[packet_sep[i]] = peercard_num;
        }else if(i==11){
            data[packet_sep[i]] = mygetcard_num;
        }else if(i==12){
            data[packet_sep[i]] = peergetcard_num;
        }else if(i==13){
            for(j=0;j<YAKU_NUM;j++){ // myyaku
                data[packet_sep[i]+j] = myyaku[j];
            }              
        }else if(i==14){
            for(j=0;j<YAKU_NUM;j++){ // peeryaku
                data[packet_sep[i]+j] = peeryaku[j];
            }                
        }else if(i==15){
            data[packet_sep[i]] = mykoikoi;
        }else if(i==16){
            data[packet_sep[i]] = peerkoikoi;
        }
    }
}

void getPacket(int *data){
    int i,j;
    for(i=0;i<17;i++){
        if(i==0){ // turn
            turn = data[packet_sep[i]];
        }else if(i==1){
            for(j=0;j<CARD_NUM;j++){
                deck[j]=data[packet_sep[i]+j];
            }               
        }else if(i==2){ // place
            for(j=0;j<PLACE_MAX;j++){
                place[j] = data[packet_sep[i]+j];
            }           
        }else if(i==3){ // mycard 
            for(j=0;j<INIT_PLACE;j++){
                mycard[j] = data[packet_sep[i]+j];
            }
        }else if(i==4){ // peercard
            for(j=0;j<INIT_PLACE;j++){
                peercard[j] = data[packet_sep[i]+j];
            }                
        }else if(i==5){ // mygetcard
            for(j=0;j<CARD_NUM/2;j++){
                mygetcard[j] = data[packet_sep[i]+j];
            }    
        }else if(i==6){
            for(j=0;j<CARD_NUM/2;j++){ // peergetcard
                peergetcard[j] = data[packet_sep[i]+j];
            }            
        }else if(i==7){
            deck_num = data[packet_sep[i]];
        }else if(i==8){
            place_num = data[packet_sep[i]];
        }else if(i==9){
            mycard_num = data[packet_sep[i]];
        }else if(i==10){
            peercard_num = data[packet_sep[i]];
        }else if(i==11){
            mygetcard_num = data[packet_sep[i]];
        }else if(i==12){
            peergetcard_num = data[packet_sep[i]];
        }else if(i==13){
            for(j=0;j<YAKU_NUM;j++){ // myyaku
                myyaku[j]=data[packet_sep[i]+j];
            }              
        }else if(i==14){
            for(j=0;j<YAKU_NUM;j++){ // peeryaku
                peeryaku[j]=data[packet_sep[i]+j];
            }                
        }else if(i==15){
            mykoikoi=data[packet_sep[i]];
        }else if(i==15){
            peerkoikoi=data[packet_sep[i]];
        }
    }    
}


void Display(void){
    int data[DATA_LENGTH];
    char dispstr[100];
    int r,i,j;
    int tmp;
    int tmpcard;
    int tmpyaku[YAKU_NUM];

    // 描画クリア
    glClear(GL_COLOR_BUFFER_BIT);
    // カード描画
    PaintCards();
    // カード選択
    if(status==0){ // 同期処理と札選択
        if(turn==0){
            if(role==0){
                setPacket(data);
                write(hanahuda_soc,&data[0],4*DATA_LENGTH);
                glColor3ub(0,0,0);
                glRasterPos2i(90,250);
                sprintf(dispstr,"your turn");
                for(i=0;i<strlen(dispstr);i++){
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,dispstr[i]);
                }
            }else{
                read(hanahuda_soc,&data[0],4*DATA_LENGTH);
                getPacket(data);
                glColor3ub(0,0,0);
                glRasterPos2i(110,250);
                sprintf(dispstr,"wait");
                for(i=0;i<strlen(dispstr);i++){
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,dispstr[i]);
                }
            }
         }else{
            if(turn==1){
                if(role==1){
                    setPacket(data);
                    write(hanahuda_soc,&data[0],4*DATA_LENGTH);
                    glColor3ub(0,0,0);
                    glRasterPos2i(90,250);
                    sprintf(dispstr,"your turn");
                    for(i=0;i<strlen(dispstr);i++){
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,dispstr[i]);
                    }
                }
                if(role==0){
                    read(hanahuda_soc,&data[0],4*DATA_LENGTH);
                    getPacket(data);
                    glColor3ub(0,0,0);
                    glRasterPos2i(110,250);
                    sprintf(dispstr,"wait");
                    for(i=0;i<strlen(dispstr);i++){
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,dispstr[i]);
                    }
                }    
            }
        }
        if((mykoikoi==0)||(peerkoikoi==0)){
            status=6;
        }
        if((mycard_num==0)&&(peercard_num==0)){
            status=6;
        }
    }else if(status==1){ // 手札から出した札で札が取れるとき
        // 場札の獲得
        r = popHandCard(clickedCard);
        pushgetCard(r);
        r = popPlace(clickedPlaceCard);
        pushgetCard(r);

        // 山札から出した札の処理
        r = popDeck();
        if(isGetPlace(r)==0){ // 取れる札がないとき
            pushPlace(r);
        }else if(isGetPlace(r)==1){ // 取れる札が1枚のとき
            pushgetCard(r);
            for(i=0;i<place_num;i++){
                if(cards[r].month==cards[place[i]].month){
                    r=popPlace(i);
                    break;
                }
            }
            pushgetCard(r);
        }else if(isGetPlace(r)>=2){ // 取れる札が2枚のとき
            tmp=0;
            for(i=0;i<place_num;i++){ // 得点の高い札を探索
                if(cards[place[i]].month==cards[r].month){ // rと月が一緒なら
                    if(tmp<cards[place[i]].rank){
                        tmpcard=i;
                        tmp=cards[place[i]].rank;
                    }
                }
            }
            pushgetCard(r);
            r = popPlace(tmpcard);
            pushgetCard(r);
        }
        status=3; // 役の処理へ
    }else if(status==2){ // 手札から出した札で札が取れないとき
        // 手札から1枚だす
        r = popHandCard(clickedCard);
        pushPlace(r); 

        // 山札から出した札の処理
        r = popDeck();
        if(isGetPlace(r)==0){ // 取れる札がないとき
            pushPlace(r);
        }else if(isGetPlace(r)==1){ // 取れる札が1枚のとき
            pushgetCard(r);
            for(i=0;i<place_num;i++){
                if(cards[r].month==cards[place[i]].month){
                    r=popPlace(i);
                    break;
                }
            }
            pushgetCard(r);
        }else if(isGetPlace(r)>=2){ // 取れる札が2枚のとき
            tmp=0;
            for(i=0;i<place_num;i++){ // 得点の高い札を探索
                if(cards[place[i]].month==cards[r].month){ // rと月が一緒なら
                    if(tmp<cards[place[i]].rank){
                        tmpcard=i;
                        tmp=cards[place[i]].rank;
                    }
                }
            }
            pushgetCard(r);
            r = popPlace(tmpcard);
            pushgetCard(r);
        }
        status=3; // 役の処理へ      
    }else if(status==3){ // 役の処理
    // 1つ前の役の和を計算
    tmp=0;
    if(role==0){
        for(i=0;i<YAKU_NUM;i++){
            tmpyaku[i]=myyaku[i];
        }
    }else{
        for(i=0;i<YAKU_NUM;i++){
            tmpyaku[i]=peeryaku[i];
        }
    }
    checkYaku();
    if(role==0){
        for(i=0;i<YAKU_NUM;i++){
            if(tmpyaku[i]!=myyaku[i]){
                tmp=1;
                break;
            }
        }
    }else{
        for(i=0;i<YAKU_NUM;i++){
            if(tmpyaku[i]!=peeryaku[i]){
                tmp=1;
                break;
            }
        }
    }    

    if(role==0){
        if(tmp==1){
            printYaku();
            mykoikoi=-1;
            status=4;
        }else{status=5;}   
    }else{
        if(tmp==1){
            printYaku();
            peerkoikoi=-1;
            status=4;
        }else{status=5;}
    }
    }else if(status==4){ // こいこいの処理
        if((turn==0)&&(role==0)){
            PutSprite(koikoiimg,200,200,&koikoiinfo,1);
        }else if((turn==1)&&(role==1)){
            PutSprite(koikoiimg,200,200,&koikoiinfo,1);
        }

        if(role==0){
            if(mykoikoi!=-1){
                status=5;
            }
        }else{
            if(peerkoikoi!=-1){ 
                status=5;
            }
        }
    }else if(status==5){ // ターン終了処理
        selectedCard=-1;
        clickedCard=-1;
        selectedPlaceCard=-1;
        clickedPlaceCard=-1;
        turn=1-turn;
        // パケット送信準備
        setPacket(data);
        write(hanahuda_soc,data,4*DATA_LENGTH);
        if(role==0){
            if(mykoikoi==0){
                status=6;
            }else{
                status=0;
            }
        }else{
            if(peerkoikoi==0){
                status=6;
            }else{
                status=0;
            }
        }
    }else if(status==6){ // ゲーム終了処理
        printYaku();
        if((mykoikoi!=0)&&(peerkoikoi!=0)){
            printf("引き分け\n");
        }
        if(mykoikoi==0){
            printf("親の勝ちです\n");
        }
        if(peerkoikoi==0){
            printf("子の勝ちです\n");
        }
        printf("ゲームを終了してください\n");
        status=7;
    }
    // 描画の反映
    glFlush(); 
}
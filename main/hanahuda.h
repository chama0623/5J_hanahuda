#include <sys/types.h>
#include<GL/glut.h>
#include <GL/glpng.h>
#include "mylib.h"

#define PORT (in_port_t)50000 // ポート番号
#define HOSTNAME_LENGTH 64 // ホストネーム長
#define DATA_LENGTH 158 // 同期通信でやりとりするパケット長

#define WINDOW_W 600 // ウィンドウの横の長さ
#define WINDOW_H 600 // ウィンドウの縦の長さ
#define CARD_WIDTH 128 // 札の縦の長さ(読み込み時)
#define CARD_HEIGHT 256 // 札の横の長さ(読み込み時)
#define CARD_NUM 48 // 12*4
#define ALL_CARD 49 // 全札+裏面の札
#define MONTH 12 // 月の数
#define MONTH_CARD 4 // 1つの月の札の数
#define PLACE_MAX 13 // 場における最大札数
#define INIT_PLACE 8 // 最初の場の数
#define SHUFFLE_TIME 1000 // 山札をシャッフルする回数
#define YAKU_NUM 12 // 役の数

static int pointlist[YAKU_NUM] = {10,8,7,5,5,5,5,5,5,1,1,1}; // 役を取った時の得点
static int packet_sep[17] ={0,1,49,62,70,78,102,126,127,128,129,130,131,132,144,156,157}; // パケットの区切り目  
static GLuint cardimg[ALL_CARD]; // 札描画のための構造体
static pngInfo cardinfo[ALL_CARD]; // 札描画のための構造体
static GLuint koikoiimg; // こいこいポップアップの画像
static pngInfo koikoiinfo; // こいこいポップアップの画像

static char month[MONTH][4]={"jan","feb","mar","apr","may","jun",
                            "jul","aug","sep","oct","nov","dec"}; // 月の英語表記(略記)
static char yakuname[YAKU_NUM][20] = {"五光","四光","雨四光","三光","花見で一杯","月見で一杯",
                                    "猪鹿蝶","赤短","青短","タネ","タン","カス"}; // 役名

// 札の点数
static int cardrank[MONTH][4] ={{4,2,1,1}, // 松に鶴
                                {3,2,1,1}, // 梅にうぐいす
                                {4,2,1,1}, // 桜に幕
                                {3,2,1,1}, // 藤にほととぎす
                                {3,2,1,1}, // 菖蒲に八ツ橋
                                {3,2,1,1}, // 牡丹に蝶
                                {3,2,1,1}, // 萩に猪
                                {4,3,1,1}, // ススキに月・雁
                                {3,2,1,1}, // 菊に盃
                                {3,2,1,1}, // 紅葉に鹿
                                {4,3,2,1}, // 小野道風にカエル・柳にツバメ
                                {4,1,1,1}}; // 桐に鳳凰

// 札用の構造体
struct cardstruct{
    int month; // 1-12
    int num; // imgの番号
    int rank; //1-4, 4:光札,3:タネ札,2:短冊札,1:カス札
};
typedef struct cardstruct card;
// 札用の構造体の配列を定義
static card cards[CARD_NUM];

static int deck_num=0; // 次に出る山札配列のindex
static int deck[CARD_NUM]; // 山札
static int place_num=0; // 場に出ている札の数
static int place[PLACE_MAX]; // 場の配列
static int mycard_num=INIT_PLACE; //親の札の数
static int mycard[INIT_PLACE]; //親の札
static int peercard_num=INIT_PLACE; //子の札の数
static int peercard[INIT_PLACE]; //子の札 
static int role; // 親(0)か子(1)か
static int turn=0; // 誰のターンか
static int selectedCard=-1; // 選択中の手札の札の番号を保持
static int clickedCard=-1; // クリックした手札の札を保持 
static int selectedPlaceCard=-1; // 選択中の場札の札の番号を保持
static int clickedPlaceCard=-1; // クリックした場札の札の番号を保持
static int mygetcard_num=0; // 取得した札の枚数
static int mygetcard[CARD_NUM/2]; // 取得した札の番号の配列
static int peergetcard_num=0; // 子が取得した札の枚数
static int peergetcard[CARD_NUM/2]; // 子が取得した札の番号の配列
static int myyaku[YAKU_NUM]; // 取得した札
static int peeryaku[YAKU_NUM]; // 子が取得した札
static int mykoikoi=-1; // こいこいの状態
static int peerkoikoi=-1; // 子のこいこいの状態
static int hanahuda_soc; // ソケット
static int status=0; // ターンの進行状態

void Reshape(int,int);
void Timer(int);
void PutSprite(int,int,int,pngInfo *,double);
void readImg(void);
void array_init(void);
void card_init(void);
void shuffle(void);
void game_init(int,int);
int popDeck(void);
void pushPlace(int c);
int popPlace(int);
void pushHandCard(int);
int popHandCard(int);
void pushgetCard(int);
int arrangeCard(void);
void surroundCard(int,int,int,int,int);
void PaintCards(void);
int calWhichPlacecard(int,int);
int calWhichMycard(int,int);
int isGetCard(int);
int isGetPlace(int);
void Keyboard(unsigned char,int,int);
void Mouse(int,int,int,int);
void PassiveMotion(int,int);
void checkYaku(void);
void printYaku(void);
int calcPoint(void);
void setPacket(int *data);
void getPacket(int *data);
void Display(void);
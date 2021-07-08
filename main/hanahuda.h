#include <sys/types.h>
#include<GL/glut.h>
#include <GL/glpng.h>
#include "mylib.h"

#define PORT (in_port_t)50000
#define HOSTNAME_LENGTH 64
#define DATA_LENGTH 132

#define WINDOW_W 600
#define WINDOW_H 600
#define CARD_WIDTH 128
#define CARD_HEIGHT 256
#define CARD_NUM 48 // 12*4
#define ALL_CARD 49
#define MONTH 12 // 月の数
#define MONTH_CARD 4 // 1つの月のカードの数
#define PLACE_MAX 13 // 鳩ノ巣原理より場の配列は13で足りる
#define INIT_PLACE 8 // 最初の場の数
#define SHUFFLE_TIME 1000
#define YAKU_NUM 12

static int packet_sep[13] ={0,1,49,62,70,78,102,126,127,128,129,130,131};  
static GLuint cardimg[ALL_CARD];
static pngInfo cardinfo[ALL_CARD];
static char month[MONTH][4]={"jan","feb","mar","apr","may","jun",
                            "jul","aug","sep","oct","nov","dec"};
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
struct cardstruct{
    int month; // 1-12
    int num; // imgの番号
    int rank; //1-4, 4:光札,3:タネ札,2:短冊札,1:カス札
};
typedef struct cardstruct card;
static card cards[CARD_NUM];

static int deck_num=0; // 次に出るdeck配列のindex
static int deck[CARD_NUM]; // 山札
static int place_num=0; // 場に出ている札の数
static int place[PLACE_MAX]; // 場の配列
static int mycard_num=INIT_PLACE; //親のカードの数
static int mycard[INIT_PLACE]; //親のカード
static int peercard_num=INIT_PLACE; //子のカードの数
static int peercard[INIT_PLACE]; //子のカード 
static int role; // 親(0)か子(1)か
static int turn=0; // 誰のターンか
static int selectedCard=-1; // 選択中の手札のカードの番号を保持
static int clickedCard=-1; // クリックした手札のカードを保持 
static int selectedPlaceCard=-1; // 選択中の場札のカードの番号を保持
static int clickedPlaceCard=-1; // クリックした場札のカードの番号を保持
static int mygetcard_num=0; // 取得したカードの枚数
static int mygetcard[CARD_NUM/2]; // 取得したカードの番号の配列
static int peergetcard_num=0; // ペアが取得したカードの枚数
static int peergetcard[CARD_NUM/2]; // ペアが取得したカードの番号の配列
static int myyaku[YAKU_NUM];
static int peeryaku[YAKU_NUM];
static int hanahuda_soc;
static int status=0;
static int tmp_card;

void Reshape(int,int);
void Timer(int);
void PutSprite(int,int,int,pngInfo *,double);
void readImg(void);
void card_init(void);
void shuffle(void);
void game_init(int,int);
int popDeck(void);
void pushPlace(int c);
int popPlace(int);
void pushHandCard(int);
int popHandCard(int);
void pushgetCard(int);
void arrangeCard(void);
void surroundCard(int,int,int,int,int);
void PaintCards(void);
int calWhichPlacecard(int,int);
int calWhichMycard(int,int);
int isGetCard(int);
int isGetPlace(int);
void Mouse(int,int,int,int);
void PassiveMotion(int,int);
void checkYaku(void);
void setPacket(int *data);
void getPacket(int *data);

void Display(void);
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>

typedef enum {
    M1, //一萬
    M9, //九萬
    P1, //一筒
    P9, //九筒
    S1, //一索
    S9, //九索
    EA, //東
    SO, //南
    WE, //西
    NO, //北
    WH, //白
    GR, //發
    RD, //中
    TILE_KINDS_NUM //牌の種類の数
} TILE;

typedef enum {
    HEAVENLY_WIN, //天和
    EARTHLY_WIN, //地和
    HUMANLY_WIN, //人和
    ALL_HONORS, //字一色
    BIG_DRAGONS, //大三元
    BIG_FOUR_WINDS, //大四喜
    SMALL_FOUR_WINDS, //小四喜
    ALL_TERMINALS, //清老頭
    FOUR_KANTSU, //四槓子
    THIRTEEN_ORPHANS, //国士無双
    ALL_TRIPLES, //対々和
    SEVEN_PAIRS, //七対子
    VALUE_TILES, //役牌
    FLUSH, //混一色
    LITTLE_DRAGONS, //小三元
    THREE_COLOR_TRIPLES, //三色同刻
    DRAGON_SEVEN_PAIRS, //三元七対子 (七対子の上位役)
    FOUR_WIND_SEVEN_PAIRS, //四喜七対子 (七対子の上位役)
    THREE_COLOR_PAIRS, //三色七対子 (七対子の上位役)
    KAN_DRAW_WIN, //嶺上開花
    ANKAN, //暗槓
    THREE_KANTSU, //三槓子
    YAKU_KINDS_NUM //役の数
} YAKU;

#define TILES_NUM TILE_KINDS_NUM * 4
#define SUTEHAI_NUM (TILES_NUM - 13 * 2) / 2

#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
#define IBLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE);
#define GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
#define YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

#define UP 0x48
#define RIGHT 0x4D
#define DOWN 0x50
#define LEFT 0x4B
#define ENTER 0x0D

//main関数内に書く関数
short title (void);
void play (void);
void rule (void);
void yakuList (void);

//牌山に関する関数
void initializeYama (void);
void shuffle (short *, short);

//手牌に関する関数
void getHaipai (short *, bool *, short);
void tehaiPrint (short *, bool *, short *, bool);
void ankanPrint(short);
short getShanten (short *, short *, short *, short *);
short hora (short *, bool *, short, short, bool, bool, bool, bool);

//ゲーム進行に関する関数
void initializeStatus (void);
void initializeSutehai (void);
void fieldPrint (short, short *, short *, short *);
void sutehaiPrint (short);
void playOnce (void);
bool isMePlay(short *, bool *, bool*, bool *);
bool isCOMPlay(short *, bool *, bool *, short *, bool *);
void goNextGame(void);
void resultPrint(void);

short choiceHai (bool *, short *);
void discard (short *, short *, short, short);
bool isHora (short *, short *, short *, short);
bool isFuriten (short *, short *);
bool isKan (short *, bool *, short *);
bool COMThink (short *, bool *, short *, short *, short *, short *, short *, short *, bool *);

enum {
    KOKUSHI,
    TOITSU,
    FIRST
};

//関数内でよく使用する関数
void haiPrint (int);
void swap (short *, short *);
void arrayCopy (short *, short *, int);
short getArrayMinsIndex(short *, short *, int);

short yama[TILES_NUM]; //牌山
short roundTile = EA; //場風
short kyoku = 0; //局数
short honba = 0; //本場

enum {
    ME,
    COM
};

typedef struct {
    short tehai[TILE_KINDS_NUM];
    bool ankan[TILE_KINDS_NUM];
    short sutehai[SUTEHAI_NUM];
    int point;
    bool dealer;
} STATUS;

STATUS status[2];

enum {
    COM_TSUMO,
    COM_RON,
    COM_THINKING,
    COM_TSUMOGIRI,
    COM_TEDASHI,
    COM_KAN,
    RYUKYOKU,
    PLAYER_THINKING,
    NOW_STATUS_NUM
};

enum {
    TSUMO,
    RON
};

enum {
    PLAY,
    RULE,
    YAKU_LIST,
    EXIT,
    CHOICE_NUM
};

int main (void) {
    while (1) {
        switch ( title() ) {
        case PLAY:
            play();
            return 0;
        case RULE:
            rule();
            break;
        case YAKU_LIST:
            yakuList();
            break;
        case EXIT:
            return 0;
        }
    }
    return 0;
}

short title (void) {
    short choice = 0;
    while (1) {
        system("cls");
        int i;
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            haiPrint(i);
        }
        WHITE
        printf("\n\n       ヤオチュー麻雀\n\n");
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            haiPrint(i);
        }
        WHITE
        printf("\n\n");
        for (i = 0; i < CHOICE_NUM; i++) {
            printf("        ");
            if (choice == i) {
                printf("→");
            } else {
                printf("  ");
            }
            switch (i) {
            case PLAY:
                printf("プレイ\n\n");
                break;
            case RULE:
                printf("ルール\n\n");
                break;
            case YAKU_LIST:
                printf("役一覧\n\n");
                break;
            case EXIT:
                printf("終了\n\n");
                break;
            }
        }
        char key = 0;
        key = getch();
        if (key == 0xFFFFFFE0 || key == 0) {
            key = getch();
            switch (key) {
            case UP:
                if (choice != 0) {
                    choice--;
                }
                break;
            case DOWN:
                if (choice != CHOICE_NUM - 1) {
                    choice++;
                }
                break;
            }
        }
        if (key == ENTER) {
            break;
        }
    }
    return choice;
}

void rule (void) {
    system("cls");
    printf("<ルール>\n"
           "・コンピュータとの2人麻雀です。\n"
           "・一九字牌のみを使用します。\n"
           "・リーチや鳴きはありません。\n"
           "・暗槓の回数に制限はありません。\n"
           "・フリテンの状態ではロンあがりできません。\n"
           "・国士無双の暗槓ロンはできません。\n"
           "・東1局～北2局までの一荘戦です。\n"
           "・ドラや王牌はありません。\n"
           "・ノーテン罰符はありません。流局すると親は流れます。\n"
           "・役の点数の合計があがり点になります。\n"
           "・親と子の点数の差はありません。\n"
           "・親があがると連荘し、積み棒が増えます。あがり点に積み棒分が加算されます。\n"
           "・子があがると積み棒はリセットされます。\n"
           "\n<操作方法>\n"
           "・捨てる牌を矢印キーの←→で選択し、↑を押すとその牌が捨てられます。\n"
           "・ツモやロン、カンができる状態の場合、↑を押すと実行され、↓を押すとキャンセルされます。\n\n");
    system("pause");
    return;
}

void yakuList (void) {
    system("cls");
    printf("<1点役>\n"
           "・役牌\n"
           "  自風牌または場風牌または三元牌を3枚揃える。\n"
           "・暗槓\n"
           "  同じ牌を4枚揃えてカンをする。\n"
           "・嶺上開花\n"
           "  カンをしたときの補充牌であがる。\n"
           "\n<2点役>\n"
           "・七対子\n"
           "  同じ牌を2枚揃えたものを7組揃える。\n"
           "・小三元\n"
           "  三元牌のうち2種類を3枚ずつ、1種類を2枚揃えてあがる。\n"
           "  役牌が必ず2つ複合する。\n"
           "\n<3点役>\n"
           "・混一色\n"
           "  萬子・筒子・索子のどれか一種類と字牌のみであがる。\n"
           "・三色同刻\n"
           "  萬子・筒子・索子の1か9のどちらかを3枚ずつ揃える。\n"
           "\n<4点役>\n"
           "・対々和\n"
           "  同じ牌を3枚揃えたものを4組と同じ牌を2枚揃えたものを1組揃える。\n"
           "・三元七対子\n"
           "  三元牌を全て2枚ずつ揃えて七対子であがる。七対子とは複合しない。\n"
           "・三色七対子\n"
           "  萬子・筒子・索子の1か9のどちらかを2枚ずつ揃えて七対子であがる。\n"
           "  1と9の両方を2枚ずつ揃えた場合は8点になる。七対子とは複合しない。\n"
           "\n<5点役>\n"
           "・四喜七対子\n"
           "  四喜牌を全て2枚ずつ揃えて七対子であがる。七対子とは複合しない。\n"
           "・三槓子\n"
           "  暗槓を3回する。暗槓とは複合しない。\n"
           "\n<7点役>\n"
           "・国士無双"
           "  全ての牌を1枚ずつ揃え、なおかつ1種類は2枚になる。\n\n");
    system("pause");
    system("cls");
    printf("<役満(15点)> ※一般役とは複合しない\n"
           "・大三元\n"
           "  三元牌を全て3枚ずつ揃えてあがる。"
           "・字一色\n"
           "  字牌のみであがる。\n"
           "・小四喜\n"
           "  四喜牌のうち3種類を3枚ずつ、1種類を2枚揃えてあがる。\n"
           "\n<大役満(20点)> ※一般役とは複合しない\n"
           "・大四喜"
           "  四喜牌を全て3枚ずつ揃えてあがる。\n"
           "・清老頭\n"
           "  1と9のみであがる。\n"
           "・四槓子\n"
           "  暗槓を4回してあがる。\n"
           "・天和\n"
           "  親の配牌時で既にあがっている。\n"
           "・地和\n"
           "  子の1回目のツモであがる。\n"
           "・人和\n"
           "  子が親の最初の捨牌であがる。\n\n");
    system("pause");
}

void play (void) {
    srand((unsigned int)time(NULL));
    initializeStatus();
    while (kyoku <= 7 && status[ME].point >= 0 && status[COM].point >= 0) {
        playOnce();
    }
    resultPrint();
    return;
}

void initializeYama (void) {
    short i;
    for (i = 0; i < TILES_NUM; i++) {
        yama[i] = i / 4;
    }
    return;
}

void shuffle (short ary[], short size) {
    short i, place;
    for (i = 0; i < size; i++) {
        place = rand() % size;
        swap(&ary[i], &ary[place]);
    }
    return;
}

void getHaipai (short tehai[], bool ankan[], short startPlace) {
    short i;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        tehai[i] = 0;
        ankan[i] = false;
    }
    for (i = 0; i < 13; i++) {
        tehai[ yama[startPlace + i] ]++;
    }
    return;
}

void tehaiPrint (short tehai[], bool ankan[], short *tsumohai, bool isOpen) {
    short i, j;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            continue;
        }
        for (j = 0; j < tehai[i]; j++) {
            if (isOpen) {
                haiPrint(i);
            } else {
                YELLOW
                printf("■");
            }
        }
    }
    if (tsumohai != NULL) {
        printf(" ");
        if (isOpen) {
            haiPrint(*tsumohai);
        } else {
            YELLOW
            printf("■");
        }
    } else {
        printf("   ");
    }
    printf(" ");
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            ankanPrint(i);
        }
    }
    WHITE
    printf("\n");
    return;
}

void ankanPrint (short hai) {
    int i;
    YELLOW
    printf("■");
    for (i = 0; i < 2; i++) {
        haiPrint(hai);
    }
    YELLOW
    printf("■");
}

short getShanten (short tehai[], short *kokushiShanten, short *chitoiShanten, short *toitoiShanten) {
    short kokushi, chitoi, toitoi;
    short i;

    //国士無双
    short nohave = 0;
    short head = 0;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (tehai[i] == 0) {
            nohave++;
        }
        if (tehai[i] == 2) {
            head = 1;
        }
    }
    kokushi = nohave - head;
    //printf("国士無双 %d シャンテン\n", kokushi);

    //七対子
    short toitsu = 0;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (tehai[i] >= 2) {
            toitsu++;
        }
    }
    chitoi = 6 - toitsu;
    if (13 - nohave < 7) {
        chitoi += nohave - 6;
    }
    //printf("七対子   %d シャンテン\n", chitoi);

    //対々和
    toitoi = 8;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        switch (tehai[i]) {
        case 2:
            toitoi--;
            break;
        case 3:
            toitoi -= 2;
            break;
        case 4:
            toitoi -= 2;
            break;
        }
    }
    if (13 - nohave < 5) {
        toitoi += nohave - 9;
    }
    if (toitsu >= 6) {
        toitoi += toitsu - 5;
    }

    //printf("対々和   %d シャンテン\n", toitoi);

    short shanten = chitoi;
    if (chitoi > kokushi) {
        shanten = kokushi;
    } else if (chitoi > toitoi) {
        shanten = toitoi;
    }

    if (kokushiShanten != NULL) {
        *kokushiShanten = kokushi;
    }
    if (chitoiShanten != NULL) {
        *chitoiShanten = chitoi;
    }
    if (toitoiShanten != NULL) {
        *toitoiShanten = toitoi;
    }

    return shanten;
}

short hora (short tehai[], bool ankan[], short roundWind, short seatWind, bool isHeaven, bool isEarth, bool isHuman, bool isRinshan) {
    //あがり形になっているtehai[]を渡されることを想定している

    short yaku_num = 0;
    int i;

    //役満
    //天和 天地人和はフラグを渡すことで判定
    if (isHeaven) {
        yaku_num += 20;
        printf("天和   大役満 20点\n");
    }

    //地和
    if (isEarth) {
        yaku_num += 20;
        printf("地和   大役満 20点\n");
    }

    //人和
    if (isHuman) {
        yaku_num += 20;
        printf("人和   大役満 20点\n");
    }

    //字一色 19牌がないことで判定
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (i < EA) {
            if (tehai[i] != 0) {
                break;
            }
        } else {
            yaku_num += 15;
            printf("字一色   役満 15点\n");
            break;
        }
    }

    //大三元 白發中が3枚ずつあることで判定
    if (tehai[WH] == 3 && tehai[GR] == 3 && tehai[RD] == 3) {
        yaku_num += 15;
        printf("大三元   役満 15点\n");
    }

    //大四喜 東南西北が3枚ずつあることで判定
    if (tehai[EA] == 3 && tehai[SO] == 3 && tehai[WE] == 3 && tehai[NO] == 3) {
        yaku_num += 20;
        printf("大四喜 大役満 20点\n");
    }

    //小四喜 東南西北が11枚あることで判定
    if ((tehai[EA] + tehai[SO] + tehai[WE] + tehai[NO] == 11)) {
        yaku_num += 15;
        printf("小四喜   役満 15点\n");
    }

    //清老頭 字牌がないことで判定
    for (i = EA; i < TILE_KINDS_NUM; i++) {
        if (tehai[i] != 0) {
            break;
        }
    }
    if (i == TILE_KINDS_NUM) {
        yaku_num += 20;
        printf("清老頭 大役満 20点\n");
    }

    //四槓子 暗槓4つで判定
    short ankan_num = 0;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            ankan_num++;
        }
    }

    if (ankan_num == 4) {
        yaku_num += 20;
        printf("四槓子 大役満 20点");
    }

    //役満は役満としか複合しないためここで計算打ち切り
    if (yaku_num > 0) {
        return yaku_num;
    }

    //一般役

    //嶺上開花 嶺上開花フラグ
    if (isRinshan) {
        yaku_num += 1;
        printf("嶺上開花      1点\n");
    }

    //国士無双 牌を全種類持っていることで判定
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (tehai[i] == 0) {
            break;
        }
    }
    if (i == TILE_KINDS_NUM) {
        yaku_num += 7;
        printf("国士無双      7点\n");
    }

    //対子と刻子を数える
    short pair = 0; //対子の数
    short triple = 0; //刻子の数
    short dragonPair = 0; //三元対子の数
    short windPair = 0; //四喜対子の数
    short onePair = 0; //1の対子の数
    short ninePair = 0; //9の対子の数
    short dragonTriple = 0; //三元刻子の数
    short windValueTriple = 0; //役牌になる四喜刻子の数+
    short oneTriple = 0; //1の刻子の数
    short nineTriple = 0; //9の刻子の数
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        switch (tehai[i]) {
        case 2:
            pair++;
            if (i == M1 || i == P1 || i == S1) {
                onePair++;
            }
            if (i == M9 || i == P9 || i == S9) {
                ninePair++;
            }
            if (i == WH || i == GR || i == RD) {
                dragonPair++;
            }
            if (i == EA || i == SO || i == WE || i == NO) {
                windPair++;
            }
            break;
        case 3:
            triple++;
            if (i == M1 || i == P1 || i == S1) {
                oneTriple++;
            }
            if (i == M9 || i == P9 || i == S9) {
                nineTriple++;
            }
            if (i == WH || i == GR || i == RD) {
                dragonTriple++;
            }
            if (i == roundWind) {
                windValueTriple++;
            }
            if (i == seatWind) {
                windValueTriple++;
            }
            break;
        }
    }

    //対々和 1対子4刻子で判定
    if (pair == 1 && triple == 4) {
        yaku_num += 4;
        printf("対々和        4点\n");
    }

    //七対子 7対子で判定
    bool isSpecialSevenPair = false; //上位七対子と七対子が複合しないためのフラグ
    if (pair == 7) {
        //三元七対子 三元対子が3つで判定
        if (dragonPair == 3) {
            yaku_num += 4;
            printf("三元七対子    4点\n");
            isSpecialSevenPair = true;
        }
        //四喜七対子 四喜対子が4つで判定 東東南南西西北北白白發發中中は字一色
        if (windPair == 4) {
            yaku_num += 5;
            printf("四喜七対子    5点\n");
            isSpecialSevenPair = true;
        }
        //三色七対子 1の対子が3つまたは9の対子が3つで判定 一一①①１１九九⑨⑨９９白白のような形は三色七対子×2
        if (onePair == 3 || ninePair == 3) {
            if (onePair + ninePair == 6) {
                yaku_num += 8;
                printf("三色七対子×2 8点\n");
            } else {
                yaku_num += 4;
                printf("三色七対子    4点\n");
            }
            isSpecialSevenPair = true;
        }
        if (!isSpecialSevenPair) {
            yaku_num += 2;
            printf("七対子        2点\n");
        }
    }

    //小三元 三元刻子が2つと三元対子が1つで判定
    if (dragonTriple == 2 && dragonPair == 1) {
        yaku_num += 2;
        printf("小三元        2点\n");
    }

    //役牌 役牌になる四喜刻子と三元刻子の和
    short yakuhai = windValueTriple + dragonTriple;
    if (yakuhai > 0) {
        yaku_num += yakuhai;
        printf("役牌×%d       %d点\n", yakuhai, yakuhai);
    }

    //三色同刻
    if (oneTriple == 3 || nineTriple == 3) {
        yaku_num += 3;
        printf("三色同刻      3点\n");
    }

    //混一色 2色持っていないことで判定
    short noHaveColor = 0;
    if (tehai[M1] == 0 && tehai[M9] == 0) {
        noHaveColor++;
    }
    if (tehai[P1] == 0 && tehai[P9] == 0) {
        noHaveColor++;
    }
    if (tehai[S1] == 0 && tehai[S9] == 0) {
        noHaveColor++;
    }
    if (noHaveColor == 2) {
        yaku_num += 3;
        printf("混一色        3点\n");
    }

    //三槓子 暗槓の数が3つで判定
    if (ankan_num == 3) {
        yaku_num += 5;
        printf("三槓子        5点\n");
    } else if (ankan_num > 0) {
        yaku_num += ankan_num;
        printf("暗槓×%d       %d点\n", ankan_num, ankan_num);
    }

    return yaku_num;
}

void initializeStatus (void) {
    status[ME].point = 50;
    status[COM].point = 50;
    if (rand() % 2 == 0) {
        status[ME].dealer = true;
        status[COM].dealer = false;
    } else {
        status[ME].dealer = false;
        status[COM].dealer = true;
    }
    return;
}

void initializeSutehai (void) {
    int i;
    for (i = 0; i < SUTEHAI_NUM; i++) {
        status[ME].sutehai[i] = -1;
        status[COM].sutehai[i] = -1;
    }
    return;
}

void fieldPrint (short nowStatus, short *actionhai, short *COMActionhai, short *place) {
    system("cls");
    short tedashiPlace = rand() % 13;
    short ankanNum = 0;
    short ankanHai[4] = {-1, -1, -1, -1};
    int i;
    switch (nowStatus) {
    case COM_TSUMO:
        tehaiPrint(status[COM].tehai, status[COM].ankan, COMActionhai, true);
        printf("\t   ツモ！");
        break;
    case COM_RON:
        tehaiPrint(status[COM].tehai, status[COM].ankan, COMActionhai, true);
        printf("\t   ロン！");
        break;
    case COM_THINKING:
        YELLOW
        tehaiPrint(status[COM].tehai, status[COM].ankan, COMActionhai, false);
        break;
    case COM_TSUMOGIRI:
        YELLOW
        tehaiPrint(status[COM].tehai, status[COM].ankan, NULL, false);
        break;
    case COM_TEDASHI:
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            if (status[COM].ankan[i] == true) {
                ankanHai[ankanNum] = i;
                ankanNum++;
            }
        }
        YELLOW
        for (i = 0; i < 13 - ankanNum * 3; i++) {
            if (i == tedashiPlace) {
                printf("  ");
            } else {
                printf("■");
            }
        }
        printf(" ■ ");
        for (i = 0; ankanHai[i] != -1; i++) {
            ankanPrint(ankanHai[i]);
        }
        printf("\n");
        break;
    case COM_KAN:
        YELLOW
        tehaiPrint(status[COM].tehai, status[COM].ankan, COMActionhai, false);
        printf("\t   カン！");
        break;
    case RYUKYOKU:
        tehaiPrint(status[COM].tehai, status[COM].ankan, COMActionhai, true);
        break;
    case PLAYER_THINKING:
        YELLOW
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            if (status[COM].ankan[i] == true) {
                ankanHai[ankanNum] = i;
                ankanNum++;
            }
        }
        YELLOW
        for (i = 0; i < 13 - ankanNum * 3; i++) {
            printf("■");
        }
        printf("    ");
        for (i = 0; ankanHai[i] != -1; i++) {
            ankanPrint(ankanHai[i]);
        }
        printf("\n");
        break;
    }
    WHITE
    printf ("\n%s     ＣＯＭ %d点\n", status[COM].dealer ? "東" : "南", status[COM].point);
    printf("\n");
    sutehaiPrint(COM);
    WHITE
    printf("---------------------------\n ");
    switch (roundTile) {
    case EA:
        printf("東");
        break;
    case SO:
        printf("南");
        break;
    case WE:
        printf("西");
        break;
    case NO:
        printf("北");
        break;
    }
    short nokoriTsumo = TILES_NUM - *place - 1;
    if (nowStatus == RYUKYOKU) { //-1表示になるのを防ぐ
        nokoriTsumo = 0;
    }
    printf("%d局 %d本場 残りツモ %2d回\n", kyoku % 2 + 1, honba, nokoriTsumo);
    printf("---------------------------\n");
    sutehaiPrint(ME);
    printf("\n");
    WHITE
    printf("%s プレイヤー %d点\n\n", status[ME].dealer ? "東" : "南", status[ME].point);
    tehaiPrint(status[ME].tehai, status[ME].ankan, actionhai, true);

    return;
}

void sutehaiPrint (short whose) {
    int i;
    switch (whose) {
    case COM:
        for (i = SUTEHAI_NUM - 1; i >= 0; i--) {
            haiPrint(status[COM].sutehai[i]);
        }
        break;
    case ME:
        for (i = 0; i < SUTEHAI_NUM; i++) {
            haiPrint(status[ME].sutehai[i]);
        }
        break;
    }
    printf("\n");
    return;
}

void playOnce (void) {
    short place = 26;
    bool isHeaven = true;
    bool isEarth = true;
    bool isHuman = true;
    bool isRinshan = false;
    bool isMEAgari = false;
    bool isCOMAgari = false;
    short comDiscard;
    short meDiscard;
    short comMode = FIRST;
    initializeYama();
    shuffle(yama, TILES_NUM);
    getHaipai(status[ME].tehai, status[ME].ankan, 0);
    getHaipai(status[COM].tehai, status[COM].ankan, 13);
    initializeSutehai();
    while (1) {
        if (status[ME].dealer) {
            if (!isMePlay(&place, &isMEAgari, &isCOMAgari, &isRinshan)) {
                break;
            }
            isHeaven = false;
            isHuman = false;
            if (!isCOMPlay(&place, &isMEAgari, &isCOMAgari, &comMode, &isRinshan)) {
                break;
            }
            isEarth = false;
        } else {
            if (!isCOMPlay(&place, &isMEAgari, &isCOMAgari, &comMode, &isRinshan)) {
                break;
            }
            isHeaven = false;
            isHuman = false;
            if (!isMePlay(&place, &isMEAgari, &isCOMAgari, &isRinshan)) {
                break;
            }
            isEarth = false;
        }
    }
    printf("\n\n");
    short horaten;
    if (isMEAgari) {
        if (status[ME].dealer) {
            horaten = hora (status[ME].tehai, status[ME].ankan , roundTile, status[ME].dealer ? EA : SO, isHeaven, false, false, isRinshan);
        } else {
            horaten = hora (status[ME].tehai, status[ME].ankan, roundTile, status[ME].dealer ? EA : SO, false, isEarth, isHuman, isRinshan);
        }
        if (honba != 0) {
            printf("積み棒       %2d点\n", honba);
        }
        horaten += honba;
        printf("        計 : %2d点\n\n", horaten);
        status[COM].point -= horaten;
        status[ME].point += horaten;
        if (!status[ME].dealer) {
            goNextGame();
            honba = 0;
        } else {
            honba++;
        }
    } else if (isCOMAgari) {
        if (status[ME].dealer) {
            horaten = hora (status[COM].tehai, status[COM].ankan, roundTile, status[COM].dealer ? EA : SO, isHeaven, false, false, isRinshan);
        } else {
            horaten = hora (status[COM].tehai, status[COM].ankan, roundTile, status[COM].dealer ? EA : SO, false, isEarth, isHuman, isRinshan);
        }
        if (honba != 0) {
            printf("積み棒       %2d点\n", honba);
        }
        horaten += honba;
        printf("        計 : %2d点\n\n", horaten);
        status[ME].point -= horaten;
        status[COM].point += horaten;
        if (!status[COM].dealer) {
            goNextGame();
            honba = 0;
        } else {
            honba++;
        }
    } else {
        fieldPrint(RYUKYOKU, NULL, NULL, &place);
        printf("\n流局\n\n");
        goNextGame();
        honba++;
    }
    system("pause");
    return;
}

bool isMePlay (short *place, bool *isMEAgari, bool *isCOMAgari, bool *isRinshan) {
    *isRinshan = false;
    short meDiscard;

    meKan: //カンしたときにここに戻ってくる

    if (*place >= TILES_NUM) {
        return false;
    }
    fieldPrint(PLAYER_THINKING, &yama[*place], NULL, place);

    //プレイヤーツモ和了
    if ( isHora(status[ME].tehai, status[ME].sutehai, &yama[*place], TSUMO) ) {
        *isMEAgari = true;
        return false;
    }

    //カン
    if ( isKan (status[ME].tehai, status[ME].ankan, &yama[*place]) && *place != TILES_NUM - 1) {
        (*place)++;
        *isRinshan = true;
        goto meKan;
    }
    //プレイヤー摸打
    meDiscard = choiceHai(status[ME].ankan, &yama[*place]);
    discard(status[ME].tehai, status[ME].sutehai, meDiscard, yama[*place]);

    //COMロン和了
    status[COM].tehai[meDiscard]++;
    if(getShanten(status[COM].tehai, NULL, NULL, NULL) == -1 &&
        isFuriten(status[COM].tehai, status[COM].sutehai)) {
        status[COM].tehai[meDiscard]--;
        //Sleep(500);
        fieldPrint(COM_RON, NULL, NULL, place);
        status[COM].tehai[meDiscard]++;
        *isCOMAgari = true;
        return false;
    }
    status[COM].tehai[meDiscard]--;
    (*place)++;
    return true;
}

bool isCOMPlay (short *place, bool *isMEAgari, bool *isCOMAgari, short *comMode, bool *isRinshan) {
    if (*place >= TILES_NUM) {
        return false;
    }
    short comDiscard;

    fieldPrint(COM_THINKING, NULL, &yama[*place], place);
    //COMツモ和了
    if ( COMThink(status[COM].tehai, status[COM].ankan, status[COM].sutehai, status[ME].sutehai, &yama[*place], &comDiscard, comMode, place, isRinshan) ) {
        status[COM].tehai[ yama[*place] ]--;
        Sleep(500);
        fieldPrint(COM_TSUMO, NULL, &yama[*place], place);
        status[COM].tehai[ yama[*place] ]++;
        *isCOMAgari = true;
        return false;
    }
    Sleep(500);
    if (yama[*place] == comDiscard) {
        fieldPrint(COM_TSUMOGIRI, NULL, &yama[*place], place);
    } else {
        fieldPrint(COM_TEDASHI, NULL, &yama[*place], place);
    }
    //プレイヤーロン和了
    if ( isHora(status[ME].tehai, status[ME].sutehai, &comDiscard, RON) ) {
        *isMEAgari = true;
        return false;
    }
    Sleep(500);
    (*place)++;
    return true;
}

bool isHora (short tehai[], short sutehai[], short *agarihai, short mode) {
    short tmpTehai[TILE_KINDS_NUM];
    arrayCopy(tmpTehai, tehai, TILE_KINDS_NUM);
    int i, j;
    //フリテン判定
    if (mode == RON && isFuriten(tmpTehai, sutehai)) {
        return false;
    }

    tmpTehai[*agarihai]++;
    char key = 0;
    if (getShanten(tmpTehai, NULL, NULL, NULL) == -1) {
        switch (mode) {
        case TSUMO:
            printf("ツモ(↑) あがらない(↓)");
            break;
        case RON:
            printf("ロン(↑) あがらない(↓)");
            break;
        }
        while (1) {
            key = getch();
            if (key == 0xFFFFFFE0 || key == 0) {
                key = getch();
                switch (key) {
                case UP:
                    tehai[*agarihai]++;
                    return true;
                case DOWN:
                    return false;
                default:
                    continue;
                }
            }
        }
    } else {
        return false;
    }
}

bool isFuriten (short tehai[], short sutehai[]) {
    int i, j;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        tehai[i]++;
        if (getShanten(tehai, NULL, NULL, NULL) == -1) {
            for (j = 0; j < SUTEHAI_NUM; j++) {
                if (i == sutehai[j]) {
                    tehai[i]--;
                    return true;
                }
            }
        }
        tehai[i]--;
    }
    return false;
}

bool isKan (short tehai[], bool ankan[], short *tsumohai) {
    int i;
    char key = 0;
    tehai[*tsumohai]++;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (tehai[i] == 4) {
            haiPrint(i);
            WHITE
            printf("カン(↑)　カンしない(↓)");
            while (1) {
                key = getch();
                if (key == 0xFFFFFFE0 || key == 0) {
                    key = getch();
                    switch (key) {
                    case UP:
                        tehai[i] = 3;
                        ankan[i] = true;
                        return true;
                    case DOWN:
                        tehai[*tsumohai]--;
                        return false;
                    default:
                        continue;
                    }
                }
            }
        }
    }
    tehai[*tsumohai]--;
    return false;
}

void goNextGame (void) {
    if (kyoku % 2 != 0) {
        roundTile++;
    }
    status[ME].dealer = !status[ME].dealer;
    status[COM].dealer = !status[COM].dealer;
    kyoku++;
    return;
}

void resultPrint (void) {
    system("cls");
    printf("---------------------------\n\n");
    printf("    あなた : %3d点\n\n", status[ME].point);
    printf("    ＣＯＭ : %3d点\n\n", status[COM].point);
    if (status[ME].point > status[COM].point) {
        printf("    あなたの勝ちです！\n\n");
    } else if (status[ME].point < status[COM].point) {
        printf("    あなたの負けです…\n\n");
    } else {
        printf("    引き分けです\n\n");
    }
    printf("---------------------------\n");
}

short choiceHai (bool ankan[], short *tsumohai) {
    short kanNum = 0;
    char key = 0;
    int i;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            kanNum++;
        }
    }
    short max = 13 - kanNum * 3;
    short place = max;
    while (key != UP) {
        printf("\r");
        for (i = 0; i < max + 1; i++) {
            if (i == place) {
                if (i == max) {
                    printf(" ");
                }
                printf("↑");
            } else {
                printf("  ");
            }
        }
        key = getch();
        if (key == 0xFFFFFFE0 || key == 0) {
            key = getch();
            switch (key) {
            case RIGHT:
                if (place == max) {
                    place = 0;
                } else {
                    place++;
                }
                break;
            case LEFT:
                if (place == 0) {
                    place = max;
                } else {
                    place--;
                }
                break;
            case UP:
                break;
            }
        }
    }

    if (place == max) {
        return *tsumohai;
    }

    short j;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            continue;
        }
        for (j = 0; j < status[ME].tehai[i]; j++) {
            place--;
            if (place == -1) {
                goto end;
            }
        }
    }
    end:
    return i;
}

void discard (short tehai[], short sutehai[], short hai, short tsumo) {
    int i;
    tehai[tsumo]++;
    tehai[hai]--;
    for (i = 0; i < SUTEHAI_NUM; i++) {
        if (sutehai[i] == -1) {
            sutehai[i] = hai;
            break;
        }
    }
    return;
}

bool COMThink (short tehai[], bool ankan[], short COMSutehai[], short PlayerSutehai[], short *tsumohai, short *comDiscard, short *mode, short *place, bool *isRinshan) {
    int i, j;

    COMKan: //カンしたときにここに戻ってくる

    tehai[*tsumohai]++;
    if (getShanten(tehai, NULL, NULL, NULL) == -1) {
        return true;
    }

    short toitsu = 0, kotsu = 0, kantsu = 0;
    //対子・刻子・槓子の数を数える
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        switch(tehai[i]) {
        case 2:
            toitsu++;
            break;
        case 3:
            kotsu++;
            break;
        case 4:
            kantsu++;
            break;
        }
    }
    if (kotsu + kantsu >= 3 || toitsu + kotsu + kantsu >= 4) {
        *mode = TOITSU;
    } else {
        *mode = KOKUSHI;
    }
    short kokushiShanten[TILE_KINDS_NUM];
    short chitoiShanten[TILE_KINDS_NUM];
    short toitoiShanten[TILE_KINDS_NUM];
    short toitsuShanten[TILE_KINDS_NUM];
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        kokushiShanten[i] = 10;
        chitoiShanten[i] = 10;
        toitoiShanten[i] = 10;
        toitsuShanten[i] = 10;
        if (tehai[i] > 0) {
            tehai[i]--;
            getShanten(tehai, &kokushiShanten[i], &chitoiShanten[i], &toitoiShanten[i]);
            toitsuShanten[i] = toitoiShanten[i] < chitoiShanten[i] ? toitoiShanten[i] : chitoiShanten[i];
            tehai[i]++;
        }
    }
    short discardHais[TILE_KINDS_NUM];
    short discardHaisNum;
    //printf("%s\n", *mode == KOKUSHI ? "国士" : "対子");

    //対子手の時は4枚持っている牌は暗槓
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if(tehai[i] == 4 && *place != TILES_NUM - 1) {
            if (*mode == TOITSU) {
                tehai[i] = 3;
                ankan[i] = true;
                (*place)++;
                *tsumohai = yama[*place];
                *isRinshan = true;
                fieldPrint(COM_KAN, NULL, tsumohai, place);
                goto COMKan;
            }
        }
    }

    *isRinshan = false;

    bool isKanned = false;
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (ankan[i] == true) {
            isKanned = true;
            break;
        }
    }

    if (isKanned) {
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            toitsuShanten[i] = toitoiShanten[i];
        }
    }

    //対子手の時は暗刻を優先する
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if (chitoiShanten[i] > 0 && tehai[i] == 3) {
            toitsuShanten[i] = 10;
        }
    }
    //対子手の時は河に2枚切れている牌は切る
    short kawaPaiNum[TILE_KINDS_NUM];
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        kawaPaiNum[i] = 0;
    }
    for (i = 0; COMSutehai[i] != -1; i++) {
        kawaPaiNum[ COMSutehai[i] ]++;
    }
    for (i = 0; PlayerSutehai[i] != -1; i++) {
        kawaPaiNum[ PlayerSutehai[i] ]++;
    }

    if (*mode == TOITSU) {
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            if (kawaPaiNum[i] >= 2 && tehai[i] == 1) {
                toitsuShanten[i] = -1;
            }
        }
    }
    //対子手の時は自分で捨てた牌は切る
    if (*mode == TOITSU) {
        for (i = 0; COMSutehai[i] != -1; i++) {
            for (j = 0; j < TILE_KINDS_NUM; j++) {
                if (COMSutehai[i] == j && tehai[j] == 1 && getShanten(tehai, NULL, NULL, NULL) > 0) {
                    toitsuShanten[j] = -1;
                }
            }
        }
    }

    //暗槓した牌は切れない
    for (i = 0; i < TILE_KINDS_NUM; i++) {
        if(ankan[i] == true) {
            toitsuShanten[i] = 10;
        }
    }
    switch (*mode) {
        case KOKUSHI:
            discardHaisNum = getArrayMinsIndex(kokushiShanten, discardHais, TILE_KINDS_NUM);
            break;
        case TOITSU:
            discardHaisNum = getArrayMinsIndex(toitsuShanten, discardHais, TILE_KINDS_NUM);
            break;
    }
    /*
    for (i = 0; i < discardHaisNum; i++) {
        haiPrint(discardHais[i]);
    }
    WHITE
    printf("\n");
    Sleep(1000);
    */
    *comDiscard = discardHais[ rand() % discardHaisNum ];
    if (tehai[*comDiscard] >= 2 && *mode == FIRST) {
        for (i = 0; i < TILE_KINDS_NUM; i++) {
            if (tehai[i] == 1) {
                *mode = KOKUSHI;
                break;
            }
        }
    }
    if (*mode != KOKUSHI) {
        *mode = TOITSU;
    }
    tehai[*tsumohai]--;
    discard(tehai, COMSutehai, *comDiscard, *tsumohai);
    return false;
}

void haiPrint(int hai){
    switch(hai){
    case M1:
        WHITE
        printf("一");
        break;
    case M9:
        WHITE
        printf("九");
        break;
    case P1:
        IBLUE
        printf("①");
        break;
    case P9:
        IBLUE
        printf("⑨");
        break;
    case S1:
        GREEN
        printf("１");
        break;
    case S9:
        GREEN
        printf("９");
        break;
    case EA:
        WHITE
        printf("東");
        break;
    case SO:
        WHITE
        printf("南");
        break;
    case WE:
        WHITE
        printf("西");
        break;
    case NO:
        WHITE
        printf("北");
        break;
    case WH:
        WHITE
        printf("白");
        break;
    case GR:
        GREEN
        printf("發");
        break;
    case RD:
        RED
        printf("中");
        break;
    default:
        printf("  ");
        break;
    }
    return;
}

void swap (short *a, short *b) {
    short tmp;
    if (*a != *b) {
        tmp = *a;
        *a = *b;
        *b = tmp;
    }
    return;
}

void arrayCopy (short a[], short b[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        a[i] = b[i];
    }
    return;
}

short getArrayMinsIndex (short ary[], short mins[], int size) {
    int i, count = 0;
    short min = 9999;
    for (i = 0; i < size; i++) {
        if (ary[i] < min) {
            min = ary[i];
            count = 0;
            mins[count] = i;
            //printf("! ");
        } else if (ary[i] == min) {
            count++;
            mins[count] = i;
            //printf("? ");
        } else {
            //printf("/ ");
        }
    }
    //printf("\n");
    return ++count;
}

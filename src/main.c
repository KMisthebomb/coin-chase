#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <math.h>
#include "gfx/gfx.h"

typedef struct {
    // x position
    uint16_t xpos;
    // y position
    uint8_t ypos;
    // fireball direction
    uint8_t dir;
    // score
    uint8_t coinCount;
} player_t;
player_t player;

uint24_t time;

typedef struct {
    // x position
    uint16_t xpos;
    // y position
    uint8_t ypos;
} coin_t;
coin_t COIN;

void setup() {
    srand(rtc_Time());

    timer_Disable(1);
    timer_Set(1, 32768 * 20);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    player.xpos = LCD_WIDTH / 2;
    player.ypos = LCD_HEIGHT / 2;
    player.coinCount = 0;
    player.dir = 1;

    COIN.xpos = randInt(0, 304);
    COIN.ypos = randInt(10, 224);

    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_SetPalette(mypalette, sizeof_mypalette, 0);
    gfx_SetTransparentColor(0);
    gfx_FillScreen(8);
    gfx_SetTextFGColor(2);
    gfx_SetTextBGColor(8);
    gfx_SetColor(1);
}

void scanKeys() {
    kb_key_t key;
    kb_Scan();
    key = kb_Data[7];

    if (key & kb_Up) {
        player.ypos -= 2;
        player.dir = 3;
    }
    if (key & kb_Down) {
        player.ypos +=  2;
        player.dir = 4;
    }
    if (key & kb_Left) {
        player.xpos -= 2;
        player.dir = 2;
    }
    if (key & kb_Right) {
        player.xpos += 2;
        player.dir = 1;
    }
}

void calculateDistance() {
    uint16_t dist;
    int16_t distX, distY;

    distX = player.xpos - COIN.xpos;
    distY = player.ypos - COIN.ypos;
    dist = sqrt((distX * distX) + (distY * distY));

    if (dist <= 16) { // 16 = player radius + coin radius
        player.coinCount++;
        COIN.xpos = randInt(0, 304);
        COIN.ypos = randInt(10, 224);
    }
}

void drawSprites() {
    // fill screen w/ white
    gfx_FillScreen(8);

    /* code to determine which way the player is facing to display correct sprite

    dir is the direction the fireball is facing
    1 = right
    2 = left
    3 = up
    4 = down
    */

    if (player.dir == 1) {
        gfx_TransparentSprite(playerR, player.xpos, player.ypos);
    }
    if (player.dir == 2) {
        gfx_TransparentSprite(playerL, player.xpos, player.ypos);
    }
    if (player.dir == 3) {
        gfx_TransparentSprite(playerU, player.xpos, player.ypos);
    }
    if (player.dir == 4) {
        gfx_TransparentSprite(playerD, player.xpos, player.ypos);
    }

    // display coin sprite
    gfx_TransparentSprite(coin, COIN.xpos, COIN.ypos);
}

void drawStatusBar() {
    // set text size
    gfx_SetTextScale(1, 1);
    // print total coins
    gfx_PrintStringXY("Coins: ", 1, 1);
    gfx_SetTextXY(44, 1);
    gfx_PrintInt(player.coinCount, 3);
    // display time
    gfx_PrintStringXY("Time: ", 269, 1);
    gfx_SetTextXY(304, 1);
    gfx_PrintUInt(time, 2);
    // draw black line
    gfx_HorizLine_NoClip(0, 9, 320);
}

void gameOverMenu() {
    // fill screen with gray & set text size
    gfx_FillScreen(12);
    gfx_SetTextScale(2, 2);
    gfx_SetTextBGColor(12);
    // print time
    gfx_PrintStringXY("Time up!", 1, 1);
    gfx_SetTextXY(20, 50);
    // print score
    gfx_PrintStringXY("Your Score: ", 1, 16);
    gfx_PrintInt(player.coinCount, 3);
    gfx_SetTextScale(1, 1);
    // retry or quit
    gfx_PrintStringXY("[2nd]: Retry", 30, 40);
    gfx_PrintStringXY("[clear]: Quit", 30, 50);
    gfx_SwapDraw();
}

void collision() {
    // check x
    if (player.xpos >= 303) {
        player.xpos = 303;
    }
    if (player.xpos <= 1) {
        player.xpos = 1;
    }
    // check y
    if (player.ypos <= 11) {
        player.ypos = 11;
    }
    if (player.ypos >= 223) {
        player.ypos = 223;
    }
}

int main(void) {
    START:

    setup();

    do {
        // get the time
        time = timer_Get(1)/32768;

        scanKeys();

        collision();

        calculateDistance();

        drawSprites();

        drawStatusBar();

        // menu that pops up when you run out of time
        if (time <= 0) {
            gameOverMenu();
            
            while (!(kb_Data[6] & kb_Clear)) {
                kb_Scan();
                if (kb_Data[1] & kb_2nd) {
                    goto START;
                }
            }
        }

        // swap screen and buffer
        gfx_SwapDraw();
    } while (!(kb_Data[6] & kb_Clear));

    while(!os_GetCSC());

    gfx_End();

    return 0;
}
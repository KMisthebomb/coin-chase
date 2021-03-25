#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <math.h>
#include <stdlib.h>
#include "gfx/gfx.h"

int main(void) {
    START:
    srand(rtc_Time());
    uint16_t coinX = randInt(0, 320), dist, coinCount = 0;
    uint8_t coinY = randInt(10, 240), dir = 1;
    int16_t distX, distY, playerY = 50, playerX = 50;
    kb_key_t key;
    timer_Disable(1);
    timer_Set(1, 32768 * 5);
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);
    // time left
    uint24_t time;

    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_SetPalette(mypalette, sizeof_mypalette, 0);
    gfx_SetTransparentColor(0);
    gfx_FillScreen(8);
    gfx_SetTextFGColor(2);
    gfx_SetTextBGColor(8);
    gfx_SetColor(1);

    do {
        kb_Scan();
        key = kb_Data[7];

        time = timer_Get(1)/32768;

        // detect keys
        if (key & kb_Up) {
            playerY -= 2;
            dir = 3;
        }

        if (key & kb_Down) {
            playerY += 2;
            dir = 4;
        }

        if (key & kb_Left) {
            playerX -= 2;
            dir = 2;
        }

        if (key & kb_Right) {
            playerX += 2;
            dir = 1;
        }

        // calculate distance from player and coin with Pythagorean Theorem
        distX = playerX - coinX;
        distY = playerY - coinY;
        dist = sqrt((distX * distX) + (distY * distY));

        // when player hits coin
        if (dist <= 16) { // 16 = player radius + coin radius
            coinCount++;
            coinX = randInt(0, 304);
            coinY = randInt(10, 224);
        }

        // fill screen w/ white
        gfx_FillScreen(8);

        /* code to determine which way the player is facing 
        to display correct sprite

        dir is the direction the fireball is facing
        1 = right
        2 = left
        3 = up
        4 = down
        */

        if (dir == 1) {
        gfx_TransparentSprite(playerR, playerX, playerY);
        }
        if (dir == 2) {
            gfx_TransparentSprite(playerL, playerX, playerY);
        }
        if (dir == 3) {
            gfx_TransparentSprite(playerU, playerX, playerY);
        }
        if (dir == 4) {
            gfx_TransparentSprite(playerD, playerX, playerY);
        }

        // display coin sprite
        gfx_TransparentSprite(coin, coinX, coinY);
        // draw status bar
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("Coins: ", 1, 1);
        gfx_SetTextXY(44, 1);
        gfx_PrintInt(coinCount, 3);
        gfx_PrintStringXY("Time: ", 269, 1);
        gfx_SetTextXY(304, 1);
        gfx_PrintUInt(time, 2);
        gfx_HorizLine_NoClip(0, 9, 320);

        // menu that pops up when you run out of time
        if (time <= 0) {
            gfx_FillScreen(12);
            gfx_SetTextScale(2, 2);
            gfx_SetTextBGColor(12);
            gfx_PrintStringXY("Time up!", 1, 1);
            gfx_SetTextXY(20, 50);
            gfx_PrintStringXY("Your Score: ", 1, 16);
            gfx_PrintInt(coinCount, 3);
            gfx_SetTextScale(1, 1);
            gfx_PrintStringXY("[2nd]: Retry", 30, 40);
            gfx_PrintStringXY("[clear]: Quit", 30, 50);
            gfx_SwapDraw();
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
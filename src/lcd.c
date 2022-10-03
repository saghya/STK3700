#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_lcd.h"
#include "segmentlcd.h"

#define SLEEP 200

typedef enum _segment { a, b, c, d, e, f, g } segment;

typedef struct _position {
    int com;
    int bit;
} position;

position get_pos(int num, segment seg)
{
    position pos;

    bool wtf = num > 4 && num < 7;

    // set bit base
    switch (seg) {
        case a:
        case e:
        case f:
        case g:
            pos.bit = 13;
            break;
        case b:
        case c:
        case d:
            pos.bit = 14;
            break;
        default:
            pos.com = -1;
            pos.bit = -1;
            return pos;
    }

    // set com
    switch (seg) {
        case a:
            pos.com = wtf ? 0 : 1;
            break;
        case e:
            pos.com = wtf ? 6 : 7;
            break;
        case f:
            pos.com = wtf ? 2 : 3;
            break;
        case g:
            pos.com = wtf ? 3 : 4;
            break;
        case b:
            pos.com = 1;
            break;
        case c:
            pos.com = 5;
            break;
        case d:
            pos.com = 7;
            break;
        default:
            pos.com = -1;
            pos.bit = -1;
            return pos;
    }

    // final bit
    if ((num > 4 && pos.bit % 2) || (num > 3 && !(pos.bit % 2))) {
        pos.bit += 8 + 2 * (num - 1);
    } else {
        pos.bit += 2 * (num - 1);
    }

    return pos;
}

bool print_segment(int num, segment seg)
{
    position pos = get_pos(num, seg);
    if (pos.com < 0 || pos.bit < 0) {
        return false;
    }

    LCD_SegmentSet(pos.com, pos.bit, true);

    // g is actually 2 segments
    if (seg == g) {
        if (num < 4 || num == 7)
            LCD_SegmentSet(pos.com, pos.bit + 1, true);
        else if (num == 4)
            LCD_SegmentSet(pos.com, pos.bit + 8 + 1, true);
        else
            LCD_SegmentSet(pos.com + 1, pos.bit + 1, true);
    }

    return true;
}

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Locatl prototypes */
void Delay(uint32_t dlyTicks);

/*******************************************************************************
 * @brief SysTick_Handler
 *   Interrupt Service Routine for system tick counter
 * @note
 *   No wrap around protection
 ******************************************************************************/
void SysTick_Handler(void)
{
    msTicks++; /* increment counter necessary in Delay()*/
}

/*******************************************************************************
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

int main(void)
{
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1);
    }

    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    /* Infinite loop with test pattern. */
    while (1) {

        // loop through all segments
        for (int i = 1; i < 8; i++) {
            SegmentLCD_Number(i);
            for (int j = a; j <= g; j++) {
                print_segment(i, j);
                Delay(200);
            }
        }

        SegmentLCD_AllOff();
        Delay(200);
    }
}

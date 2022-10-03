#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
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

bool print_segment(int num, segment seg, bool enable)
{
    position pos = get_pos(num, seg);
    if (pos.com < 0 || pos.bit < 0) {
        return false;
    }

    LCD_SegmentSet(pos.com, pos.bit, enable);

    // g is actually 2 segments
    if (seg == g) {
        if (num < 4 || num == 7)
            LCD_SegmentSet(pos.com, pos.bit + 1, enable);
        else if (num == 4)
            LCD_SegmentSet(pos.com, pos.bit + 8 + 1, enable);
        else
            LCD_SegmentSet(pos.com + 1, pos.bit + 1, enable);
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
    while ((msTicks - curTicks) < dlyTicks)
        ;
}

int main(void)
{
    CMU_ClockEnable(cmuClock_GPIO, true);  // GPIO clk enable
    CMU_ClockEnable(cmuClock_UART0, true); // UART clk enable

    GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 1); // PF7 output 1
    GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1); // PE0 output 1
    GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);    // PE1 input  0

    USART_InitAsync_TypeDef u = USART_INITASYNC_DEFAULT; // UART config
    USART_InitAsync(UART0, &u);                          // 115200 8N1
    UART0->ROUTE |= (1 << 8 | 1 << 0 | 1 << 1);          // Location1; Tx; Rx

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1)
            ;
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    // test
    char ch;
    int  num = 1, seg = g;
    while (1) {
        print_segment(num, seg, true);
        ch = USART_Rx(UART0);
        SegmentLCD_Number(num);
        print_segment(num, seg, false);
        switch (ch) {
            case 'd':
            case 'l':
                num = num < 7 ? num + 1 : 1;
                break;
            case 'a':
            case 'h':
                num = num > 1 ? num - 1 : 7;
                break;
            case 'w':
            case 'k':
                switch (seg) {
                    case a:
                        seg = e;
                        break;
                    case b:
                        seg = c;
                        break;
                    case c:
                        seg = b;
                        break;
                    case d:
                        seg = e;
                        break;
                    case e:
                        seg = f;
                        break;
                    case f:
                        seg = e;
                        break;
                    case g:
                        seg = f;
                        break;
                }
                break;
        }
    }
}

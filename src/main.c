#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"

#define SLEEP 200

typedef enum _segment { a, b, c, d, e, f, g } segment;
typedef enum _direction { up, down, left, right } direction;

volatile uint32_t  msTicks; /* counts 1ms timeTicks */
volatile direction dir = right;

SegmentLCD_UpperCharSegments_TypeDef
    upperCharSegments[SEGMENT_LCD_NUM_OF_UPPER_CHARS];
SegmentLCD_LowerCharSegments_TypeDef
    lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

void SysTick_Handler(void)
{
    msTicks++; /* increment counter necessary in Delay()*/
}

void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks)
        ;
}

void UART0_RX_IRQHandler(void)
{
    switch (USART_RxDataGet(UART0)) {
        case 'd':
        case 'l':
            dir = right;
            break;
        case 'a':
        case 'h':
            dir = left;
            break;
        case 'w':
        case 'k':
            dir = up;
            break;
        case 's':
        case 'j':
            dir = down;
            break;
    }
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
    UART0->ROUTE |= USART_ROUTE_LOCATION_LOC1;
    UART0->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;

    USART_IntEnable(UART0, UART_IF_RXDATAV);
    NVIC_EnableIRQ(UART0_RX_IRQn);

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1)
            ;
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    // test
    int num = 0, seg = a;
    while (1) {
        SegmentLCD_Number(num);

        lowerCharSegments[num].raw |= 1 << seg;
        SegmentLCD_LowerSegments(lowerCharSegments);
        Delay(500);
        lowerCharSegments[num].raw &= ~(1 << seg);
        SegmentLCD_LowerSegments(lowerCharSegments);
        switch (dir) {
            case right:
                if (seg == e || seg == f) {
                    switch (seg) {
                        case e:
                            seg = g;
                            break;
                        case f:
                            seg = a;
                            break;
                    }
                }
                else {
                    num = num < 6 ? num + 1 : 0;
                }
                break;
            case left:
                num = num > 0 ? num - 1 : 6;
                break;
            case up:
                switch (seg) {
                    case a:
                        num++;
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

            case down:
                switch (seg) {
                    case a:
                        num++;
                        seg = f;
                        break;
                    case b:
                        seg = c;
                        break;
                    case c:
                        seg = b;
                        break;
                    case d:
                        num++;
                        seg = f;
                        break;
                    case e:
                        seg = f;
                        break;
                    case f:
                        seg = e;
                        break;
                    case g:
                        num++;
                        seg = e;
                        break;
                }
        }
    }
}


#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"
#include "display.h"

#define SLEEP 200

typedef enum _segment { a, b, c, d, e, f, g, h, l, m } segment;
typedef enum _direction { up, down, left, right } direction;

volatile uint32_t  msTicks; /* counts 1ms timeTicks */
volatile direction dir      = right;
volatile direction prev_dir = right;

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
    prev_dir = dir;
    switch (USART_RxDataGet(UART0)) {
        case 'd':
        case 'l':
        case 67: // right arrow
            dir = right;
            break;
        case 'a':
        case 'h':
        case 68: // left arrow
            dir = left;
            break;
        case 'w':
        case 'k':
        case 65: // up arrow
            dir = up;
            break;
        case 's':
        case 'j':
        case 66: // down arrow
            dir = down;
            break;
    }
}

__STATIC_INLINE void UART_Init()
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
}

int main()
{
    UART_Init();
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1)
            ;
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);
    map map={{0}};
    /*map map = {{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
               ,{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
               ,{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1}
               ,{0,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
               ,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};*/
    
    drawLine(&map,(pixel){0,4},(pixel){10,4});
    drawLine(&map,(pixel){10,4},(pixel){10,0});
    drawLine(&map,(pixel){10,0},(pixel){14,0});
    displayMap(&map);
    // test
    int num = 0, seg = a;
    while (1) {
        SegmentLCD_Number(num);
        num++;
        Delay(500);
    }
}


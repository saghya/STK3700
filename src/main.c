#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"

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

    // test
    char c;
    while (1) {
        c = USART_Rx(UART0);
        for (int i = 0; i < 30; i++)
            USART_Tx(UART0, c);
    }
}

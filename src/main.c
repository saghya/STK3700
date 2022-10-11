#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"

int main(void)
{
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_UART0, true);

    GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 1);
    GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1);
    GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);

    USART_InitAsync_TypeDef UART0_init = USART_INITASYNC_DEFAULT; // UART config
    USART_InitAsync(UART0, &UART0_init);                          // 115200 8N1
    UART0->ROUTE |= USART_ROUTE_LOCATION_LOC1;
    UART0->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;

    // test
    char c;
    while (1) {
        c = USART_Rx(UART0);
        for (int i = 0; i < 30; i++)
            USART_Tx(UART0, c);
    }
}

#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"

volatile uint8_t iflag = false;
volatile uint8_t data;

void UART0_RX_IRQHandler(void)
{
    iflag = true;
    data = USART_RxDataGet(UART0);
}

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

    USART_IntEnable(UART0, UART_IF_RXDATAV);
    NVIC_EnableIRQ(UART0_RX_IRQn);

    // test
    while (1) {
        if (iflag) {
            iflag = false;
            USART_Tx(UART0, data);
        }
    }
}

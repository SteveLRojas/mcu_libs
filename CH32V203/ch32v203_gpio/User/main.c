#include "debug.h"
#include "system_ch32v20x.h"
#include "uart.h"
#include "ch32v203_gpio.h"

//Pins:
// LED3 = PA8
// TXD = PA9
// RXD = PA10
// UDM = PA11
// UDP = PA12
// LED2 = PC13
// LED1 = PC14
// LED0 = PC15


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//platform_init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1, ENABLE);

	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_AFIO_PP, GPIO_PIN_9);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_IN | GPIO_MODE_FLOAT_IN, GPIO_PIN_10);
	gpio_set_mode(GPIOA, GPIO_DIR_SPD_OUT_50MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_8);
	gpio_set_mode(GPIOC, GPIO_DIR_SPD_OUT_2MHZ | GPIO_MODE_PP_OUT, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    Delay_Init();
    uart_init(115200, 0, 0);

    printf("SystemClk:%d\n", SystemCoreClock);
    printf("Unicorn\n");

    gpio_set_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    Delay_Ms(100);
    gpio_clear_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
    Delay_Ms(100);
    gpio_write_pin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, 1);
    Delay_Ms(100);

    printf("Blink done\n");

    if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
	{
    	printf("Something is fishy with port C\n");
	}

    while(1)
    {
    	gpio_toggle_pin(GPIOA, GPIO_PIN_8);
    	gpio_toggle_pin(GPIOC, GPIO_PIN_15);
    	Delay_Ms(50);
    	gpio_write_pin(GPIOC, GPIO_PIN_14, gpio_read_pin(GPIOC, GPIO_PIN_15));
    	Delay_Ms(50);
    	gpio_write_pin(GPIOC, GPIO_PIN_13, gpio_read_pin(GPIOC, GPIO_PIN_14));
    	Delay_Ms(50);
    }
}

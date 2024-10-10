#include "bsp_gpio.h"

void bsp_gpio_init(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
}



#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define LED(sta){if(sta==1) HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);	\
					else if(sta==2)	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);	\
					else if(!sta) HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);}	 \

void bsp_gpio_init(void);

#ifdef __cplusplus
}
#endif
#endif


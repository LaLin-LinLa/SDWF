#ifndef _BSP_SYSTICK_H_
#define _BSP_SYSTICK_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

void bsp_systick_Init(uint8_t SYSCLK);
void _delay_ms(__IO uint16_t nms);
void _delay_us(__IO uint32_t nus);

#ifdef __cplusplus
}
#endif
#endif


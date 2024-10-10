#ifndef _DRV_ESP_H_
#define _DRV_ESP_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "http_client.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define RX_LEN	1000
#define	TX_LEN	600
#define esp_Delay delay_ms

//传输协议类型定义
typedef enum
{
	enumTCP,
	enumUDP
}ENUM_NetPro_TypeDef;

void esp_Init(void);
void esp_Config(void);
void esp_Uart_printf(char *fmt,...);
uint8_t esp_Send_cmd(const uint8_t *cmd, const uint8_t *ack, uint16_t waittime);
uint8_t esp_ExitTran(void);
uint8_t esp_LinkServer(ENUM_NetPro_TypeDef enumN, uint8_t *ip, uint8_t *port);
uint8_t esp_ConnectAP(char *ssid, char *password);

extern void esp_uart_Rx_handler(void);
extern void esp_timer_handler(void);
extern uint16_t esp_rx_sta;
extern uint8_t esp_Rxbuff[RX_LEN];

#ifdef __cplusplus
}
#endif
#endif

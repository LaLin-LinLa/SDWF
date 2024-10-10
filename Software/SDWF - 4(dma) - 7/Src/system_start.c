#include "system_start.h"
#include "bsp_systick.h"
#include "drv_esp.h"
#include "http_client.h"

char g_wifi_ssid[32] = "CU_UZGb";
char g_wifi_password[32] = "13018899";

/**
	* @brief  系统初始化
	* @retval None
	*/
void System_Init(void){
	bsp_systick_Init(72);
	esp_Init();
}


uint8_t ch = 0;
/**
	* @brief  系统任务循环
	* @retval None
	*/
void System_loop(void){
	switch (ch)
  {
		case 1:
			GET_NowWeather();
			ch=0;
  		break;
  
		
  	default:
  		break;
  }
	
	
}



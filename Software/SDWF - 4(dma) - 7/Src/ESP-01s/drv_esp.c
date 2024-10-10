#include "drv_esp.h"
#include "malloc.h"	 

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim4;
uint8_t esp_Rxbuff[RX_LEN];
uint8_t esp_Txbuff[TX_LEN];
uint16_t esp_rx_sta = 0;
uint16_t esp_rx_timer =0;
uint8_t *p;

/**
	* @brief  ESP初始化
	* @retval None
	*/
void esp_Init(void){
	p=mymalloc(SRAMIN,300);							//申请32字节内存
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)esp_Rxbuff, RX_LEN);
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
	esp_Config();
}

/**
	* @brief  ESP串口打印
	* @retval None
	*/
void esp_Uart_printf(char *fmt,...){
	uint16_t i,j; 
	va_list ap; 
	//memset(esp_Txbuff,0,sizeof(esp_Txbuff));
	va_start(ap,fmt);
	vsprintf((char*)esp_Txbuff,fmt,ap);
	va_end(ap);
	i=strlen((const char*)esp_Txbuff);
	for(j=0;j<i;j++)							//循环发送数据
	{
		while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE)==0);//循环发送,直到发送完毕 		  
		USART2->DR=esp_Txbuff[j];  
	} 
}

/**
	* @brief  ESP发送指令
	* @retval 0：发送成功
	*					1：发送失败
	*/
uint8_t esp_Send_cmd(const uint8_t *cmd, const uint8_t *ack, uint16_t waittime){
	uint8_t res = 0;
	esp_rx_sta = 0;
	esp_Uart_printf("%s\r\n",cmd);
	if(ack&&waittime){		//需要等待应答
		while(--waittime){	//等待倒计时
			esp_Delay(10);
			if(esp_rx_sta&0x8000){	//接收数据结束
				
				if(ack){
					if(strstr((const char *)esp_Rxbuff, (const char *)ack)){
						/*success report*/
						esp_Rxbuff[esp_rx_sta&0x7fff] = 0;		//添加结束符
						break;
					}/*fail report*/
					else
						esp_rx_sta&=~(1<<15);
				}
				
			}
		}if(!waittime) res=1;
	}return res;
}	

/**
	* @brief  ESP配置
	* @retval 0: 正常	其它；错误
	*/
void esp_Config(void){
	
	while(esp_Send_cmd("AT","OK",20))	//检查WIFI模块是否在线
	{
		esp_ExitTran();//退出透传
		esp_Send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		esp_Delay(500);
	}
	
	while(esp_Send_cmd("ATE0","OK",20));	//关闭回显	
	esp_Send_cmd("AT+CWMODE=3","OK",50);	//设置WIFI 1为STA模式,2为AP模式,3为STA与AP模式共存
	esp_Send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	esp_Delay(1000);         //延时3S等待重启成功
	esp_Delay(1000);
	
	esp_ConnectAP("CU_UZGb","13018899");//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	esp_Send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
}

/**
	* @brief  ESP退出传输
	* @retval 0: 正常	其它；错误
	*/
uint8_t esp_ExitTran(void){
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//循环发送,直到发送完毕 		  
		USART2->DR='+';
	esp_Delay(15);
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//循环发送,直到发送完毕 		  
		USART2->DR='+';
	esp_Delay(15);
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//循环发送,直到发送完毕 		  
		USART2->DR='+';
	esp_Delay(100);
	return esp_Send_cmd("AT","OK",20);
}

/**
	* @brief  ESP连接路由
	* @retval 0: 正常	其它；错误
	*/
uint8_t esp_ConnectAP(char *ssid, char *password){
	sprintf((char*)p, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
	return esp_Send_cmd(p, "WIFI GOT IP", 300);
}

/**
	* @brief  ESP连接远程服务器
	* @param	enumN:传输协议 ip:服务器的ip   port:端口
	* @retval 0: 正常	其它；错误
	*/
uint8_t esp_LinkServer(ENUM_NetPro_TypeDef enumN, uint8_t *ip, uint8_t *port){
	switch( enumN )
	{
		case enumTCP:
			sprintf((char*)p, "AT+CIPSTART=\"TCP\",\"%s\",%s", ip, port);
			
			break;
		case enumUDP:
			sprintf((char*)p, "AT+CIPSTART=\"UDP\",\"%s\",%s", ip, port);
			break;
	}
	return esp_Send_cmd(p, "CONNECT", 100);
}

/**
	* @brief  串口接收中断服务函数
	* @retval None
	*/
void esp_uart_Rx_handler(void){
	if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE)!=RESET){
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		if(!esp_rx_sta){
			TIM4->CR1|=1<<0;	//使能定时器中断
		}
		esp_rx_timer=2;
		esp_rx_sta += (RX_LEN - huart2.hdmarx->Instance->CNDTR);
	}
}
/**
	* @brief  定时器中断服务函数
	* @retval None
	*/
void esp_timer_handler(void){
	if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE))
	{
		if(esp_rx_timer)	esp_rx_timer--;
		else{
			if((esp_rx_sta>0)&&(( esp_rx_sta & (1<<15))==0)){	//计时结束，接收到数据且最高位不为1
				esp_rx_sta|=1<<15;				//强制标记接收完成
				__HAL_DMA_DISABLE(huart2.hdmarx);
				huart2.hdmarx->Instance->CNDTR = RX_LEN;
				__HAL_DMA_ENABLE(huart2.hdmarx);
				TIM4->CR1&=~(1<<0);				//关闭定时器
			}
		}
		__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
	}
}



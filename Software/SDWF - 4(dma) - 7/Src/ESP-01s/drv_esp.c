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
	* @brief  ESP��ʼ��
	* @retval None
	*/
void esp_Init(void){
	p=mymalloc(SRAMIN,300);							//����32�ֽ��ڴ�
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)esp_Rxbuff, RX_LEN);
	__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
	esp_Config();
}

/**
	* @brief  ESP���ڴ�ӡ
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
	for(j=0;j<i;j++)							//ѭ����������
	{
		while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE)==0);//ѭ������,ֱ��������� 		  
		USART2->DR=esp_Txbuff[j];  
	} 
}

/**
	* @brief  ESP����ָ��
	* @retval 0�����ͳɹ�
	*					1������ʧ��
	*/
uint8_t esp_Send_cmd(const uint8_t *cmd, const uint8_t *ack, uint16_t waittime){
	uint8_t res = 0;
	esp_rx_sta = 0;
	esp_Uart_printf("%s\r\n",cmd);
	if(ack&&waittime){		//��Ҫ�ȴ�Ӧ��
		while(--waittime){	//�ȴ�����ʱ
			esp_Delay(10);
			if(esp_rx_sta&0x8000){	//�������ݽ���
				
				if(ack){
					if(strstr((const char *)esp_Rxbuff, (const char *)ack)){
						/*success report*/
						esp_Rxbuff[esp_rx_sta&0x7fff] = 0;		//��ӽ�����
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
	* @brief  ESP����
	* @retval 0: ����	����������
	*/
void esp_Config(void){
	
	while(esp_Send_cmd("AT","OK",20))	//���WIFIģ���Ƿ�����
	{
		esp_ExitTran();//�˳�͸��
		esp_Send_cmd("AT+CIPMODE=0","OK",200);  //�ر�͸��ģʽ	
		esp_Delay(500);
	}
	
	while(esp_Send_cmd("ATE0","OK",20));	//�رջ���	
	esp_Send_cmd("AT+CWMODE=3","OK",50);	//����WIFI 1ΪSTAģʽ,2ΪAPģʽ,3ΪSTA��APģʽ����
	esp_Send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	esp_Delay(1000);         //��ʱ3S�ȴ������ɹ�
	esp_Delay(1000);
	
	esp_ConnectAP("CU_UZGb","13018899");//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	esp_Send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
}

/**
	* @brief  ESP�˳�����
	* @retval 0: ����	����������
	*/
uint8_t esp_ExitTran(void){
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//ѭ������,ֱ��������� 		  
		USART2->DR='+';
	esp_Delay(15);
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//ѭ������,ֱ��������� 		  
		USART2->DR='+';
	esp_Delay(15);
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC)==0){};//ѭ������,ֱ��������� 		  
		USART2->DR='+';
	esp_Delay(100);
	return esp_Send_cmd("AT","OK",20);
}

/**
	* @brief  ESP����·��
	* @retval 0: ����	����������
	*/
uint8_t esp_ConnectAP(char *ssid, char *password){
	sprintf((char*)p, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
	return esp_Send_cmd(p, "WIFI GOT IP", 300);
}

/**
	* @brief  ESP����Զ�̷�����
	* @param	enumN:����Э�� ip:��������ip   port:�˿�
	* @retval 0: ����	����������
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
	* @brief  ���ڽ����жϷ�����
	* @retval None
	*/
void esp_uart_Rx_handler(void){
	if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE)!=RESET){
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		if(!esp_rx_sta){
			TIM4->CR1|=1<<0;	//ʹ�ܶ�ʱ���ж�
		}
		esp_rx_timer=2;
		esp_rx_sta += (RX_LEN - huart2.hdmarx->Instance->CNDTR);
	}
}
/**
	* @brief  ��ʱ���жϷ�����
	* @retval None
	*/
void esp_timer_handler(void){
	if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE))
	{
		if(esp_rx_timer)	esp_rx_timer--;
		else{
			if((esp_rx_sta>0)&&(( esp_rx_sta & (1<<15))==0)){	//��ʱ���������յ����������λ��Ϊ1
				esp_rx_sta|=1<<15;				//ǿ�Ʊ�ǽ������
				__HAL_DMA_DISABLE(huart2.hdmarx);
				huart2.hdmarx->Instance->CNDTR = RX_LEN;
				__HAL_DMA_ENABLE(huart2.hdmarx);
				TIM4->CR1&=~(1<<0);				//�رն�ʱ��
			}
		}
		__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
	}
}



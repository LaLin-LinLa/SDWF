#include "http_client.h"
#include "drv_esp.h"
#include "malloc.h"	  
#include "cJSON.h"

Result_T weather_data;

/********************************************************************************************************
** ����: cJSON_NowWeatherParse, ��������ʵ�����ݰ�now.json
**------------------------------------------------------------------------------------------------------
** ����: JSON:����ʵ�����ݰ�  Result_T:�洢�����õ������õ�����
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:�����ɹ� ����������ʧ��		
********************************************************************************************************/
static int cJSON_NowWeatherParse(char *JSON, Result_T *result){
	cJSON *json,*arrayItem,*object,*subobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	
	memset(utf8str, 0, 32);
	memset(gbkstr, 0, 32);
	
	json = cJSON_Parse(JSON); //����JSON���ݰ�
	if(json == NULL)		  //���JSON���ݰ��Ƿ�����﷨�ϵĴ��󣬷���NULL��ʾ���ݰ���Ч
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //��ӡ���ݰ��﷨�����λ��
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //ƥ���ַ���"results",��ȡ��������
		{
			int size = cJSON_GetArraySize(arrayItem);     //��ȡ�����ж������
			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//��ȡ����������
			{
				/* ƥ���Ӷ���1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("--------------------------------Now Weather Data------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //����--��Ҫ�õ�������
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->name, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
				}
				/* ƥ���Ӷ���2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					printf("---------------------------------subobject2-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//����Ԥ������--��Ҫ�õ�������
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->text, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//����Ԥ������--��Ҫ�õ�������
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->code, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //�¶�--��Ҫ�õ�������
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->temperature, gbkstr, strlen(gbkstr)); //�������õ�����
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
				}
				/* ƥ���Ӷ���3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					printf("---------------------------------subobject3-------------------------------\n");
					printf("cJSON_GetObjectItem: %s:%s\n\n",subobject->string,subobject->valuestring);
				}
			} 
		}
	}
	
	cJSON_Delete(json); //�ͷ�cJSON_Parse()����������ڴ�ռ�
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;

}

/********************************************************************************************************
** ����: GET_NowWeather, GET ����ʵ�����ݰ�now.json
**------------------------------------------------------------------------------------------------------
** ����: void
** ˵��: ������Դ����֪������api.seniverse.com��
** ����: 0:��ȡ�ɹ� ��������ȡʧ��	
********************************************************************************************************/
uint8_t GET_NowWeather(void){
	esp_LinkServer(enumTCP, (uint8_t*)WEATHER_IP, (uint8_t*)WEATHER_PORT);
	esp_Send_cmd("AT+CIPMODE=1", "OK", 100);		//����Ϊ͸��ģʽ
	esp_Send_cmd("AT+CIPSEND", ">", 100);	//��ʼ͸��

	esp_rx_sta = 0;
	esp_Uart_printf("GET https://api.seniverse.com/v3/weather/now.json?key=SdNKYPKFBXlB2LlRo&location=beijing&language=zh-Hans&unit=c\r\n\r\n");
	esp_Delay(1000);
	//���ݽ���
	if(esp_rx_sta&0x8000){
		cJSON_NowWeatherParse((char*)esp_Rxbuff, &weather_data);
	}
	esp_ExitTran();//�˳�͸��
	esp_Send_cmd("AT+CIPCLOSE","OK",50);         //�ر�����
	return 0;
}



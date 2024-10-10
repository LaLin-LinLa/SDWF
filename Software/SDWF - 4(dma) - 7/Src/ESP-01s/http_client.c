#include "http_client.h"
#include "drv_esp.h"
#include "malloc.h"	  
#include "cJSON.h"

Result_T weather_data;

/********************************************************************************************************
** 函数: cJSON_NowWeatherParse, 解析天气实况数据包now.json
**------------------------------------------------------------------------------------------------------
** 参数: JSON:天气实况数据包  Result_T:存储解析得到的有用的数据
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:解析成功 其他：解析失败		
********************************************************************************************************/
static int cJSON_NowWeatherParse(char *JSON, Result_T *result){
	cJSON *json,*arrayItem,*object,*subobject,*item;
	char *utf8str,*gbkstr;
	int gbkstr_len;
	
	utf8str = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	gbkstr = (char*)mymalloc(SRAMIN, 32*sizeof(char));
	
	memset(utf8str, 0, 32);
	memset(gbkstr, 0, 32);
	
	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			printf("cJSON_GetArraySize: size=%d\n",size); 
			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("--------------------------------Now Weather Data------------------------------\n");
					printf("---------------------------------subobject1-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   
					{
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,item->valuestring);
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //地名--需要用到的数据
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen((const char*)utf8str), (unsigned char*)gbkstr, &gbkstr_len);	
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->name, gbkstr, strlen(gbkstr)); //保存有用的数据
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
				/* 匹配子对象2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					printf("---------------------------------subobject2-------------------------------\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//天气预报文字--需要用到的数据
					{
						utf8str = item->valuestring;
						SwitchToGbk((const unsigned char*)utf8str, strlen(utf8str), (unsigned char*)gbkstr, &gbkstr_len);
						printf("cJSON_GetObjectItem: %s:%s\n",item->string,gbkstr);
						memcpy(result->text, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//天气预报代码--需要用到的数据
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->code, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //温度--需要用到的数据
					{
						gbkstr = item->valuestring;
						printf("cJSON_GetObjectItem: %s:%s\n",item->string, gbkstr);
						memcpy(result->temperature, gbkstr, strlen(gbkstr)); //保存有用的数据
					}
					memset(utf8str, 0, 32);
					memset(gbkstr, 0, 32);
				}
				/* 匹配子对象3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					printf("---------------------------------subobject3-------------------------------\n");
					printf("cJSON_GetObjectItem: %s:%s\n\n",subobject->string,subobject->valuestring);
				}
			} 
		}
	}
	
	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间
	myfree(SRAMIN,utf8str);
	myfree(SRAMIN,gbkstr);
	
	return 0;

}

/********************************************************************************************************
** 函数: GET_NowWeather, GET 天气实况数据包now.json
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 数据来源：心知天气（api.seniverse.com）
** 返回: 0:获取成功 其他：获取失败	
********************************************************************************************************/
uint8_t GET_NowWeather(void){
	esp_LinkServer(enumTCP, (uint8_t*)WEATHER_IP, (uint8_t*)WEATHER_PORT);
	esp_Send_cmd("AT+CIPMODE=1", "OK", 100);		//设置为透传模式
	esp_Send_cmd("AT+CIPSEND", ">", 100);	//开始透传

	esp_rx_sta = 0;
	esp_Uart_printf("GET https://api.seniverse.com/v3/weather/now.json?key=SdNKYPKFBXlB2LlRo&location=beijing&language=zh-Hans&unit=c\r\n\r\n");
	esp_Delay(1000);
	//数据解析
	if(esp_rx_sta&0x8000){
		cJSON_NowWeatherParse((char*)esp_Rxbuff, &weather_data);
	}
	esp_ExitTran();//退出透传
	esp_Send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
	return 0;
}



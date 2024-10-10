#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

/* 心知天气（api.seniverse.com）--天气数据的来源 */
#define WEATHER_IP 		"116.62.81.138"
#define WEATHER_PORT 	"80"

/* 时间服务器--用于同步网络时间 */
#define TIME_SERVERIP	"www.beijing-time.org"
#define TIME_PORTNUM	"80"

/* 天气数据结构体 */
typedef struct
{
	/* 实况天气数据 */
	char name[32];				//地名
	char text[32];				//天气预报文字
	char code[32];				//天气预报代码
	char temperature[32];   	//气温
	
	/* 今天、明天、后天天气数据 */
	char date[3][32];			//日期
	char text_day[3][64];	    //白天天气现象文字
	char code_day[3][32];		//白天天气现象代码
	char code_night[3][64]; 	//晚间天气现象代码
	char high[3][32];			//最高温
	char low[3][32];			//最低温
	char wind_direction[3][64]; //风向
	char wind_speed[3][32];  	//风速，单位km/h（当unit=c时）
	char wind_scale[3][32];  	//风力等级
	
	char life_suggestion[128][2];	//生活建议
}Result_T;

uint8_t GET_NowWeather(void);

extern Result_T weather_data;
//SwitchToGbk函数在utf8togbk.c中
extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

#ifdef __cplusplus
}
#endif
#endif

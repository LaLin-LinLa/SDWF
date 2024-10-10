#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

/* ��֪������api.seniverse.com��--�������ݵ���Դ */
#define WEATHER_IP 		"116.62.81.138"
#define WEATHER_PORT 	"80"

/* ʱ�������--����ͬ������ʱ�� */
#define TIME_SERVERIP	"www.beijing-time.org"
#define TIME_PORTNUM	"80"

/* �������ݽṹ�� */
typedef struct
{
	/* ʵ���������� */
	char name[32];				//����
	char text[32];				//����Ԥ������
	char code[32];				//����Ԥ������
	char temperature[32];   	//����
	
	/* ���졢���졢������������ */
	char date[3][32];			//����
	char text_day[3][64];	    //����������������
	char code_day[3][32];		//���������������
	char code_night[3][64]; 	//��������������
	char high[3][32];			//�����
	char low[3][32];			//�����
	char wind_direction[3][64]; //����
	char wind_speed[3][32];  	//���٣���λkm/h����unit=cʱ��
	char wind_scale[3][32];  	//�����ȼ�
	
	char life_suggestion[128][2];	//�����
}Result_T;

uint8_t GET_NowWeather(void);

extern Result_T weather_data;
//SwitchToGbk������utf8togbk.c��
extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

#ifdef __cplusplus
}
#endif
#endif

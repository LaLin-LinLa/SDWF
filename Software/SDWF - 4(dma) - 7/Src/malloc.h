#ifndef __MALLOC_H
#define __MALLOC_H
#include "main.h"
 
#ifndef NULL
#define NULL 0
#endif

#define Bank1_SRAM1_ADDR 0x60000000
#define Bank1_SRAM2_ADDR 0x64000000
#define Bank1_SRAM3_ADDR 0x68000000
#define Bank1_SRAM4_ADDR 0x6c000000

//���������ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��
#define SRAMEX   1		//�ⲿ�ڴ��
//#define SRAMCCM  2		//CCM�ڴ��(�˲���SRAM����CPU���Է���!!!)

#define SRAMBANK 	2	//����֧�ֵ�SRAM����.	


//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
//�ڲ�SRAMλ��Block1:0x2000 0000 ~ 0x3fff ffff(stm32f103c8t6��20KBsram����Ѱַ��ΧΪ0x2000 0000 ~ 0x2000 4fec)
#define MEM1_BLOCK_SIZE			32  	  					//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			12*1024  						//�������ڴ� 12K	
#define MEM1_ALLOC_TABLE_SIZE			MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С

//mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define MEM2_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM2_MAX_SIZE			960 *1024  						//�������ڴ�960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//�ڴ���С
		 


//�ڴ���������
struct _m_mallco_dev 
{
	void (*init)(uint8_t);					//��ʼ��
	uint8_t (*perused)(uint8_t);		  	    	//�ڴ�ʹ����
	uint8_t 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
	uint16_t *memmap[SRAMBANK]; 				//�ڴ����״̬��
	uint8_t  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,uint8_t c,uint32_t count);	//�����ڴ�
void mymemcpy(void *des,void *src,uint32_t n);//�����ڴ�     
void my_mem_init(uint8_t memx);				//�ڴ�����ʼ������(��/�ڲ�����)
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	//�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		//�ڴ��ͷ�(�ڲ�����)
uint8_t my_mem_perused(uint8_t memx);				//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(uint8_t memx,void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(uint8_t memx,uint32_t size);			//�ڴ����(�ⲿ����)
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);//���·����ڴ�(�ⲿ����)
void *my_malloc(uint32_t size);
void my_free(void *ptr);
#endif














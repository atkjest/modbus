#include "sys.h"
#include "usart.h"
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵��
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using printf() for debugging, no file handling */
	/* is required. */
};
/* FILE is typedef�� d in stdio.h. */
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
	x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������
	USART3->DR = (u8) ch;
	return ch;
}
#endif
//end
//////////////////////////////////////////////////////////////////
#define EN_USART1_RX
#ifdef EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���

u8 uart_rev_buff[Rev_Buff_Max];     //���ջ���
extern u16 uart_rev_count;	//ADU�����ֽڸ���
extern u8 volatile uart_rev_flag;

void USART3_IRQHandler(void)
{
	u8 res;
	if(USART3->SR&(1<<5))//���յ�����
	{
		// res=USART1->DR;
		// USART1->DR=res;
        // while(!(USART1->SR&(1<<6)));//�ȴ��������
		//if(uart_rev_flag == 1)	//�ȴ�һ֡���ݽ������
		{
			if (uart_rev_count<Rev_Buff_Max)
			{
				res=USART3->DR;
				uart_rev_buff[uart_rev_count]=res;
				uart_rev_count++;
				/*�����յ��ֽں󳬹�3.5���ֽ�ʱ��û���µ��ֽ���Ϊ���ν������*/
				//3.5���ַ�ʱ��=3.5*8*1/9600��=2.917ms
				//3.5���ַ�ʱ�䣬�ж�һ֡�����Ƿ����
				//Timerx_Init(6,7199);	//0.5ms
				Timerx_Init(20,7199);	//0.5ms
			}
			else
			{
				res=USART3->DR;	//�������֡���ȳ������棬�����ַ�
			}
		}
		//else
			;

	}
}
#endif
void uart_send_char(u8 ch)
{
	USART3->DR=ch;
    while(!(USART3->SR&(1<<6)));//�ȴ��������
		//USART1->SR &= ~(1<<6);
}
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
//USART3
void uart_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������
    mantissa<<=4;
	mantissa+=fraction;
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ��
	GPIOB->CRH&=0XFFFF00FF;
	GPIOB->CRH|=0X00008B00;//IO״̬����

	RCC->APB1RSTR|=1<<18;   //��λ����3
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ
	//����������
 	USART3->BRR=mantissa; // ����������
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    //PE�ж�ʹ��
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��
	MY_NVIC_Init(1,1,USART3_IRQChannel,2);//��2��������ȼ�
#endif
}

#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "lcd.h"
#include "rtc.h"
#include "wkup.h"
#include "adc.h"
#include "modbus.h"
//Mini STM32�����巶������13
//ADC ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
int main(void)
{
  Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(36,9600); //����1��ʼ��
	LED_Init();
	Adc_Init();
	MB_Init();

	while(1)
	{
		MB_Poll();
	}
}























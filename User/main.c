/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc ���ԣ���ʾʱ���ʽΪ: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"
#include "./beep/bsp_beep.h"
#include "./Led/bsp_led.h"

// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime =
	{
		0, 0, 0, 1, 1, 2000, 0};

/*ʱ��ṹ�壬����ʱ��2000-01-01 00:00:08*/
struct rtc_time clocktime =
	{
		8, 0, 0, 1, 1, 2000, 0};

extern __IO uint32_t TimeDisplay;
extern __IO uint32_t TimeAlarm;

//��*��ע�����
//��bsp_rtc.h�ļ��У�

//1.�����ú�USE_LCD_DISPLAY�����Ƿ�ʹ��LCD��ʾ
//2.�����ú�RTC_CLOCK_SOURCE_LSI��RTC_CLOCK_SOURCE_LSE����ʹ��LSE������LSI����

//3.STM32��LSE����Ҫ��ǳ��ϸ�ͬ���ĵ�·������������Ʒʱ����Щ��������⡣
//  ��ʵ����Ĭ��ʹ��LSI����
//
//4.��������ϣ��RTC������Դ�������Ȼ���У���Ҫ��������ĵ�ز۰�װť�۵�أ�
//  �������Ҹĳ�ʹ���ⲿ����ģʽRTC_CLOCK_SOURCE_LSE
//  ť�۵���ͺţ�CR1220
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */

static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* �ȴ� HSE ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* ʹ�� PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* �ȴ� PLL ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}

int main()
{
	int i = 0;

	USART_Config();
	LED_GPIO_Config();
	Key_GPIO_Config();

	BEEP_GPIO_Config();

	RTC_NVIC_Config();
	RTC_Configuration();
	/* ����RTC���ж����ȼ� */
	
	//RTC_CheckAndConfig(&systmtime);


	while (1)
	{
		
		if(TimeAlarm)
		{
			LED_RED;

			while(1)
			{
				int j = 300;
				while(j--)
				{
					BEEP(ON);
					for(i = 0; i < 0xf; i++);
					BEEP(OFF);
					for(i = 0; i < 0xaff; i++);
					if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
					{
						BEEP(OFF);
						TimeAlarm = 0;
						
						break;
					}
				}
				if(!TimeAlarm)
					break;
				for(i = 0; i < 0x3ffff; i++);
			}	
		}

		LED_YELLOW;
		while(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_OFF);
		LED_GREEN;


		RTC_SetAlarm(RTC_GetCounter() + 3600);
		RTC_WaitForLastTask();

		LED1_OFF;
		LED2_OFF;
		LED3_OFF;
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
	
		
	}
}

/***********************************END OF FILE*********************************/

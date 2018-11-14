/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "./protocol/uartprotocol.h"
#include "./key/key.h"
#include "./SysTick/delay.h"



uint8_t Rxflag=0;
uint8_t ucTemp;
uint8_t ucPackNum=1;  //package number 1-255

uint16_t ucSalvePackLen=0; //�ӻ����͵İ����ȣ�����֡ͷ������

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
	
uint8_t test[6144]={1};	
uint8_t ucReciveBuffer[512+8];        //�жϽ������ݰ���һ��������󳤶�Ϊ����512+��ͷ8
uint8_t ucSetThreshold[12];           //12�ֽ���ֵ��Ŵ�
uint8_t ucAlarmtest[4]={1,2,3,4};     //������������

uint8_t ucSlaveResponseAlarm[8];      //�ӻ���Ӧ����������հ�
uint8_t ucSlaveResponseCarState[16];  //�ӻ���Ӧ������ȡ����״̬������հ�
uint8_t ucSlaveResponseSendData[8];   //�ӻ���Ӧ������������������հ�


int main(void)
{	
	
	uint16_t usRxCount;
	uint8_t ucUpKeyVaule;
	uint16_t i=0;
	
//	__disable_irq();
//	SCB->VTOR = FLASH_BASE | 0x10000;
//	__enable_irq();
	
	
	for(i=0;i<6144;i++)
	{
		test[i]=1;
	}
	

	DelayInit();
	Key_GPIO_Config();
	/* USART1 ����ģʽΪ 115200 8-N-1���жϽ��� */
	USARTx_Config();
	NVIC_Configuration();
	
	Usart_SendString( macUSARTx, "protocol program!\n" );
	

	/* �򵥵�ͨ��Э�飬�����س����з���Ϊ1������֡ */
	usRxCount = 0;
//	Master_Send_Data(test,6144);
//	Master_Send_Data(test,6144);
//	Master_Send_Data(test,5);
//	Master_Send_Data(test,512);
//	Master_Send_Data(test,513);
//	Master_Send_Data(test,1024);
//	Master_Send_Data(test,6143);
	while(1)
	{
//		DelayS(1);
//		Master_Send_Alarm_Data(ucAlarmtest);
		
		//up
		if(Key_Scan(macKEY1_GPIO_PORT,macKEY1_GPIO_PIN)==KEY_OFF)
			Master_Send_Alarm_Data(ucAlarmtest);
		//down		
		if(Key_Scan(macKEY2_GPIO_PORT,macKEY2_GPIO_PIN)==KEY_OFF)
			Master_Send_GetCarState_data();
		//right
		if(Key_Scan(macKEY2_GPIO_PORT,macKEY3_GPIO_PIN)==KEY_OFF)
			Master_Send_Data(test,6144);
		
		
		if(ucSalvePackLen!=0) //���յ��ӻ����͵�����
		{
			UcAnalysis_RecSlave_Data();
			ucSalvePackLen=0;
		}
	}

	

//		Master_Send_GetCarState_data();
	
//	for(;;)
//	{
//		



//	}
}


/*********************************************END OF FILE**********************/

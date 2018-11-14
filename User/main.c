/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   串口中断接收测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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

uint16_t ucSalvePackLen=0; //从机发送的包长度，包括帧头及数据

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
	
uint8_t test[6144]={1};	
uint8_t ucReciveBuffer[512+8];        //中断接收数据包，一个包内最大长度为数据512+包头8
uint8_t ucSetThreshold[12];           //12字节域值存放处
uint8_t ucAlarmtest[4]={1,2,3,4};     //报警测试数据

uint8_t ucSlaveResponseAlarm[8];      //从机响应报警命令接收包
uint8_t ucSlaveResponseCarState[16];  //从机响应主机获取车辆状态命令接收包
uint8_t ucSlaveResponseSendData[8];   //从机响应主机发送数据命令接收包


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
	/* USART1 配置模式为 115200 8-N-1，中断接收 */
	USARTx_Config();
	NVIC_Configuration();
	
	Usart_SendString( macUSARTx, "protocol program!\n" );
	

	/* 简单的通信协议，遇到回车换行符认为1个命令帧 */
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
		
		
		if(ucSalvePackLen!=0) //接收到从机发送的数据
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

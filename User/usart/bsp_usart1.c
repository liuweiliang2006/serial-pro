#include "bsp_usart1.h"
#include "uartprotocol.h"




 /**
  * @brief  USART1 GPIO ����,����ģʽ���á�9600 8-N-1
  * @param  ��
  * @retval ��
  */
void USARTx_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef *uart;

	/* ��1������GPIO��USART������ʱ�� */
	macUSART_APBxClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_APBxClock_FUN(macUSART_GPIO_CLK, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	
	/* ��4�������ô���Ӳ������ */
	uart = macUSARTx;	
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(uart, &USART_InitStructure);
	USART_ITConfig(uart, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/* 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(uart, ENABLE);		/* ʹ�ܴ��� */ 

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
}

/// ����USART1�����ж�
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = macUSART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*****************  ����һ���ַ� **********************/
 void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  ָ�����ȵķ����ַ��� **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


//extern uint8_t Rxflag;
extern uint8_t ucTemp;
extern uint8_t ucReciveBuffer[50];
extern uint16_t ucSalvePackLen;

/*****************************/

/*****************************/
static uint8_t ucRecCount=0;
static uint16_t ucSlaveDataLen=0;
void macUSART_INT_FUN(void)
{
	if(USART_GetITStatus( macUSARTx, USART_IT_RXNE ) != RESET)
	{	


		ucTemp = USART_ReceiveData( macUSARTx );
		
//		printf("%d\n",ucTemp);

		ucReciveBuffer[ucRecCount]=ucTemp;


		switch(ucRecCount)  //��ȡ�ӻ������͵������򳤶�
		{
			case 2: //slave high address pos  
				ucSlaveDataLen=ucReciveBuffer[ucRecCount];
				break;
			case 3://slave low address pos
				ucSlaveDataLen=(ucSlaveDataLen<<8)+ucReciveBuffer[ucRecCount];
				break;
			default:
				break;
		}
		//�ж��ж��н��յ��Ĵӻ������Ƿ�ﵽ�� ���ݳ���+�����ݵİ�ͷ���� ����ﵽ�����ʼ���ӱ���
		if((ucSlaveDataLen+Slave_Header_Length-1)==ucRecCount)
		{
			ucSalvePackLen=ucSlaveDataLen+Slave_Header_Length;
			ucSlaveDataLen=0;
			ucRecCount=0;
		}else
		  ucRecCount+=1;	
		
	 } 	 
}



/// �ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(USART1);
}
/*********************************************END OF FILE**********************/

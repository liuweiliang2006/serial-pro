#include "uartprotocol.h"
#include "bsp_usart1.h"

extern uint8_t ucPackNum;
//���ݰ���ţ�ÿ����һ����ɣ��Զ���1��ȡֵ��Χ 1-255
void Pack_Num_Count()
{
	if(ucPackNum<255)
		ucPackNum++;
	else
		ucPackNum=1;
}


//����ָ��
#define Alarm_ProtocolHeader_Length 0x08  //�����������Э���е�DATA����Ϊ������ռ�õ��ֽ���
#define Alarm_Protocol_Date_Length (Alarm_ProtocolHeader_Length+\
																	 (Alarm_High_Length<<8)+Alarm_Low_Length) //��������Э���ܳ���

//ucAlarmHeader ��CRC��DATA�⣬��������֡ͷ
static uint8_t ucAlarmHeader[7]={Frame_Header1,\
																Frame_Header2,\
																Alarm_High_Length,\
																Alarm_Low_Length,\
																Cmd_Alarm,\
																0,\
																Alarm_Frame_Num};

/*�ú������ڱ������ݵķ���*/
/*����data ��Ҫ���ڽ�����ײλ�á���ײ����*/
void Master_Send_Alarm_Data(uint8_t *data)
{
	
	uint8_t i;
	uint8_t ucAlarmData[Alarm_Protocol_Date_Length]; 
	uint8_t ucCrcCheckSum=0;
	
  __disable_irq();//�����������ݣ������ж�
	
	/*�����ʼ*/
	for(i=0;i<(sizeof(ucAlarmHeader)/sizeof(uint8_t));i++)
	{
		if(i!=Package_Posi)
			ucAlarmData[i]=ucAlarmHeader[i];
		else
			ucAlarmData[i]=ucPackNum;
		
		(uint8_t) (ucCrcCheckSum+=ucAlarmData[i]);
	}
	for(;i<Alarm_Protocol_Date_Length-1;i++)
	{
		ucAlarmData[i]=*data;
		(uint8_t) (ucCrcCheckSum+=ucAlarmData[i]);
		data++;
	}
	ucAlarmData[i]=ucCrcCheckSum;
	/*�������*/
	
	/*��������*/
	for(i=0;i<Alarm_Protocol_Date_Length;i++)
	{
		Usart_SendByte(macUSARTx,ucAlarmData[i]);
	}
	Pack_Num_Count();
	
	__enable_irq(); //������ɿ��ж�
}




//��ȡ����״ָ̬��
#define GetCarState_ProtocolHeader_Length 0x08  //�����������Э���е�DATA����Ϊ������ռ�õ��ֽ���
#define GetCarState_Protocol_Date_Length (GetCarState_ProtocolHeader_Length+\
																					(GetCarState_High_Length<<8)+GetCarState_Low_Length) //��������Э���ܳ���
//GetCarState ��CRC��DATA�⣬��������֡ͷ
static uint8_t ucGetCarStateHeader[7]={Frame_Header1,\
																			Frame_Header2,\
																			GetCarState_High_Length,\
																			GetCarState_Low_Length,\
																			Cmd_GetCarState,\
																			0,\
																			GetCarState_Frame_Num};

void Master_Send_GetCarState_data(void)
{
	uint8_t i;
	uint8_t ucGetCarStateData[GetCarState_Protocol_Date_Length]; 
	uint8_t ucCrcCheckSum=0;

	__disable_irq();
	
	/*�����ʼ*/
	for(i=0;i<(sizeof(ucGetCarStateData)/sizeof(uint8_t));i++)
	{
		if(i!=Package_Posi)
			ucGetCarStateData[i]=ucGetCarStateHeader[i];
		else
			ucGetCarStateData[i]=ucPackNum;
		(uint8_t) (ucCrcCheckSum+=ucGetCarStateData[i]);
	}
	ucGetCarStateData[i]=ucCrcCheckSum;

	/*�������*/
	
	/*��������*/
	for(i=0;i<GetCarState_Protocol_Date_Length;i++)
	{
		Usart_SendByte(macUSARTx,ucGetCarStateData[i]);
	}
	Pack_Num_Count();
	
	__enable_irq();
}



//������������
#define SendData_ProtocolHeader_Length 0x08  //����Э���е�DATA����Ϊ������ռ�õ��ֽ���
#define SendBuffer_Max_Length 0x200  //ÿ֡���ݵ�������ݳ�����512
//#define SendData_Protocol_Date_Length (SendData_ProtocolHeader_Length+(SendData_High_Length<<8)+SendData_Low_Length) //��������Э���ܳ���
//GetCarState ��CRC��DATA�⣬��������֡ͷ
static uint8_t ucSendDataHeader[7]={Frame_Header1,\
																	Frame_Header2,\
																	0,\
																	0,\
																	Cmd_SendData,\
																	0,\
																	0};

/*packdata:����*/
/*framelen:���ݳ���*/
/*framenum:֡���*/
uint8_t ucSendData[SendBuffer_Max_Length]; 
void Master_Send_Data_Frame(uint8_t *packdata,uint16_t framelen,uint8_t framenum)
{
	uint16_t i;
//	uint8_t ucSendData[SendBuffer_Max_Length]; 
	uint8_t ucCrcCheckSum=0;
//	uint8_t ucSendData[10]; 
	
	
	/*�����ʼ*/
	for(i=0;i<(sizeof(ucSendDataHeader)/sizeof(uint8_t));i++)
	{
		ucSendData[i]=ucSendDataHeader[i];
		
		switch(i)
		{
			case SendData_High_Length_Posi:{
				ucSendData[i]=(framelen>>8)&0xff;
				break;
			}
			case SendData_Low_Length_Posi:{
				ucSendData[i]=framelen&0xff;
				break;
			}
			case Package_Posi:{
				ucSendData[i]=ucPackNum;
				break;
			}
			case SendData_Fram_Posi:{
				ucSendData[i]=framenum;
				break;
			}
			default:{
				ucSendData[i]=ucSendDataHeader[i];
			}
		}
		
		(uint8_t) (ucCrcCheckSum+=ucSendData[i]);
	}
	for(;i<framelen+SendData_ProtocolHeader_Length-1;i++)
	{
		ucSendData[i]=*packdata;
		(uint8_t) (ucCrcCheckSum+=ucSendData[i]);
	}
	ucSendData[i]=ucCrcCheckSum;

	/*�������*/
	
	/*��������*/
	for(i=0;i<(SendData_ProtocolHeader_Length+framelen);i++)
	{
		Usart_SendByte(macUSARTx,ucSendData[i]);
	}
	Pack_Num_Count();
}

//����һ���е�֡��ż�һ���е����ݳ���
void Master_Send_Data(uint8_t *data,uint16_t size)
{
	/*ucFrameTotalNum:���ڱ�ʶ��ǰ�����ݹ���Ҫ��ּ������ݷ���*/
	/*ucFrameCurrentNum�����ڱ�ʶ��ǰ���͵����ܰ����еĵڼ���*/
	/*ucSendBuffer����������Ϊ���ݵ���ת*/
	uint8_t  ucFrameTotalNum=0,ucFrameCurrentNum;
	uint16_t uiFrameLen=0;                         //���ڼ�¼�˰�������Ҫ����֡���п��Է������
	uint8_t  ucSendBuffer[SendBuffer_Max_Length];
	uint16_t i=0,j=0;
	
	__disable_irq();
	
	if(size/SendBuffer_Max_Length==0) //�������ݲ���512�ֽ�ʱ
	{
		ucFrameTotalNum=0;
		uiFrameLen=size%SendBuffer_Max_Length;
		
		Master_Send_Data_Frame(data,uiFrameLen,0);
	}
	else if((size%SendBuffer_Max_Length==0)&&(size/SendBuffer_Max_Length>0))  //�����ֽ���512������ʱ
	{
		ucFrameTotalNum=size/SendBuffer_Max_Length;  
		uiFrameLen=size;       
		for(i=0;i<ucFrameTotalNum;i++)
		{
			ucFrameCurrentNum=i+1;
			for(j=0;j<SendBuffer_Max_Length;j++)
			{
				ucSendBuffer[j]=*data;
				data++;
			}
			if(i==ucFrameTotalNum-1)
				Master_Send_Data_Frame(ucSendBuffer,SendBuffer_Max_Length,0);
			else
				Master_Send_Data_Frame(ucSendBuffer,SendBuffer_Max_Length,ucFrameCurrentNum);
		}
		
	}		
	else if((size%SendBuffer_Max_Length!=0)&&(size/SendBuffer_Max_Length>0)) //�����ֲ��Ǵ���512��������512��������ʱ
	{
		ucFrameTotalNum=size/SendBuffer_Max_Length+1; 
		uiFrameLen=size%SendBuffer_Max_Length;  
		
		for(i=0;i<ucFrameTotalNum;i++)
		{
			ucFrameCurrentNum=i+1;
			if(i<ucFrameTotalNum-1)
			{
				for(j=0;j<SendBuffer_Max_Length;j++)
				{
					ucSendBuffer[j]=*data;
					data++;
				}
				Master_Send_Data_Frame(ucSendBuffer,SendBuffer_Max_Length,ucFrameCurrentNum);	
			}
			else
			{
				for(j=0;j<uiFrameLen;j++)
				{
					ucSendBuffer[j]=*data;
					data++;
				}
				Master_Send_Data_Frame(ucSendBuffer,uiFrameLen,0);
			}				
		}
	}	
	
	__enable_irq();
}



//��Ӧ�ӻ��Ĵ���

extern uint8_t ucReciveBuffer[512+8];
extern uint16_t ucSalvePackLen;
extern uint8_t ucSetThreshold[12]; //�����������ӻ�����ֵ����
uint8_t ucResponse[Response_Max_Length];

//��CRC��ǰ�İ�ͷ��ʽ
//��ͷ1����ͷ2�����ȸߡ����ȵ͡��������������š�֡���
static uint8_t ucSlaveHeader[Slave_Header_Length-1]={Frame_Header1,\
																										Frame_Header2,\
																										0,\
																										0,\
																										0,\
																										0,\
																										0}; 


//����˵����datalength ���Ͱ������ݳ��ȣ�cmd �������
void Master_Response_Slave(uint8_t datalength, uint8_t cmd)
{
	uint8_t i;
	uint8_t ucCrcCheckSum=0;
	uint8_t ucSetThresholdCount=0;
	
	__disable_irq();
/******************�������������ʼ***********************/
	for(i=0;i<Slave_Header_Length-1;i++)
	{
		if(i==Response_High_Length_Posi)
			ucResponse[i]=0x00;
		else if(i==Response_Low_Length_Posi)
			ucResponse[i]=datalength;
		else if(i==PackCmd_Posi)
			ucResponse[i]=cmd;
		else if(i==Package_Posi)
			ucResponse[i]=ucPackNum;
		else if(i==Response_Freme_Num_Posi)
			ucResponse[i]=0x00;
		else
			ucResponse[i]=ucSlaveHeader[i];	
		
		(uint8_t) (ucCrcCheckSum+=ucResponse[i]);
		
	}
	
	if(cmd==Cmd_SetThreshold)
	{
		for(;i<Response_Max_Length;i++)
		{
			ucResponse[i]=ucSetThreshold[ucSetThresholdCount];
			(uint8_t) (ucCrcCheckSum+=ucResponse[i]);
			ucSetThresholdCount++;
		}
		ucResponse[i-1]=ucCrcCheckSum;
	}
	else
		ucResponse[i]=ucCrcCheckSum;
/******************���������������**********************/	
	
/******************��������**********************/	
	if(cmd==Cmd_SetThreshold)
	{
		for(i=0;i<Response_Max_Length;i++)
		{
			Usart_SendByte(macUSARTx,ucResponse[i]);
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			Usart_SendByte(macUSARTx,ucResponse[i]);
		}
	}
	
	Pack_Num_Count();
	__enable_irq();
}



extern uint8_t ucSlaveResponseAlarm[8];
extern uint8_t ucSlaveResponseCarState[16];
extern uint8_t ucSlaveResponseSendData[8];

/*���մӻ���Ӧ����������*/
void Slave_Response_Master(uint16_t lenght,uint8_t cmd)
{
	uint16_t i;
	if(cmd==Cmd_Alarm)
	{
		for(i=0;i<lenght;i++)
			ucSlaveResponseAlarm[i]=ucReciveBuffer[i];
	}
	else if(cmd==Cmd_GetCarState)
	{
		for(i=0;i<lenght;i++)
			ucSlaveResponseCarState[i]=ucReciveBuffer[i];
	}
	else if(cmd==Cmd_SendData)
	{
		for(i=0;i<lenght;i++)
			ucSlaveResponseSendData[i]=ucReciveBuffer[i];
	}
	
}


/*salve recive buff data Analysis */
uint8_t UcAnalysis_RecSlave_Data(void)
{
	uint16_t i;
	uint16_t ucSlaveCrcCheckSum=0;
	for(i=0;i<ucSalvePackLen-1;i++) //except slave data CRC
	{		
		(uint8_t) (ucSlaveCrcCheckSum+=ucReciveBuffer[i]);
	}
	if((ucSlaveCrcCheckSum&0xff)==ucReciveBuffer[ucSalvePackLen-1])  //�жϸ��ݽ��յ������ݵó���CRC��ӻ����͵�CRCУ���Ƿ�һ��
	{
		switch(ucReciveBuffer[PackCmd_Posi])
		{
			case Cmd_Alarm: //��������
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_Alarm);
				break;
			}
			case Cmd_GetCarState: //��ȡ����״̬����
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_GetCarState);
				break;
			}
			case Cmd_SendData:  //������������
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_SendData);
				break;
			}
			case Cmd_SetThreshold: //������ֵ����
			{
				Master_Response_Slave(0x0c,Cmd_SetThreshold);
				break;
			}
			case Cmd_Report: //�ϱ�����
			{
				Master_Response_Slave(0x00,Cmd_Report);
				break;
			}
			case Cmd_SetOTA: //������������
			{
				Master_Response_Slave(0x00,Cmd_SetOTA);
				break;
			}
			default:
			{
				return Error;
			}
		}	
	}
	else 
		return Error;
}





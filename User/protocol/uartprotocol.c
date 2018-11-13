#include "uartprotocol.h"
#include "bsp_usart1.h"

extern uint8_t ucPackNum;
//数据包序号，每发送一包完成，自动加1，取值范围 1-255
void Pack_Num_Count()
{
	if(ucPackNum<255)
		ucPackNum++;
	else
		ucPackNum=1;
}


//报警指令
#define Alarm_ProtocolHeader_Length 0x08  //报警命令除了协议中的DATA数据为其它所占用的字节数
#define Alarm_Protocol_Date_Length (Alarm_ProtocolHeader_Length+\
																	 (Alarm_High_Length<<8)+Alarm_Low_Length) //报警数据协议总长度

//ucAlarmHeader 除CRC及DATA外，报警命令帧头
static uint8_t ucAlarmHeader[7]={Frame_Header1,\
																Frame_Header2,\
																Alarm_High_Length,\
																Alarm_Low_Length,\
																Cmd_Alarm,\
																0,\
																Alarm_Frame_Num};

/*该函数用于报警数据的发送*/
/*参数data 主要用于接收碰撞位置、碰撞参数*/
void Master_Send_Alarm_Data(uint8_t *data)
{
	
	uint8_t i;
	uint8_t ucAlarmData[Alarm_Protocol_Date_Length]; 
	uint8_t ucCrcCheckSum=0;
	
  __disable_irq();//主机发送数据，关总中断
	
	/*组包开始*/
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
	/*组包结束*/
	
	/*发送数据*/
	for(i=0;i<Alarm_Protocol_Date_Length;i++)
	{
		Usart_SendByte(macUSARTx,ucAlarmData[i]);
	}
	Pack_Num_Count();
	
	__enable_irq(); //发送完成开中断
}




//获取车辆状态指令
#define GetCarState_ProtocolHeader_Length 0x08  //报警命令除了协议中的DATA数据为其它所占用的字节数
#define GetCarState_Protocol_Date_Length (GetCarState_ProtocolHeader_Length+\
																					(GetCarState_High_Length<<8)+GetCarState_Low_Length) //报警数据协议总长度
//GetCarState 除CRC及DATA外，报警命令帧头
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
	
	/*组包开始*/
	for(i=0;i<(sizeof(ucGetCarStateData)/sizeof(uint8_t));i++)
	{
		if(i!=Package_Posi)
			ucGetCarStateData[i]=ucGetCarStateHeader[i];
		else
			ucGetCarStateData[i]=ucPackNum;
		(uint8_t) (ucCrcCheckSum+=ucGetCarStateData[i]);
	}
	ucGetCarStateData[i]=ucCrcCheckSum;

	/*组包结束*/
	
	/*发送数据*/
	for(i=0;i<GetCarState_Protocol_Date_Length;i++)
	{
		Usart_SendByte(macUSARTx,ucGetCarStateData[i]);
	}
	Pack_Num_Count();
	
	__enable_irq();
}



//发送数据命令
#define SendData_ProtocolHeader_Length 0x08  //除了协议中的DATA数据为其它所占用的字节数
#define SendBuffer_Max_Length 0x200  //每帧数据的最大数据长度是512
//#define SendData_Protocol_Date_Length (SendData_ProtocolHeader_Length+(SendData_High_Length<<8)+SendData_Low_Length) //报警数据协议总长度
//GetCarState 除CRC及DATA外，报警命令帧头
static uint8_t ucSendDataHeader[7]={Frame_Header1,\
																	Frame_Header2,\
																	0,\
																	0,\
																	Cmd_SendData,\
																	0,\
																	0};

/*packdata:数据*/
/*framelen:数据长度*/
/*framenum:帧序号*/
uint8_t ucSendData[SendBuffer_Max_Length]; 
void Master_Send_Data_Frame(uint8_t *packdata,uint16_t framelen,uint8_t framenum)
{
	uint16_t i;
//	uint8_t ucSendData[SendBuffer_Max_Length]; 
	uint8_t ucCrcCheckSum=0;
//	uint8_t ucSendData[10]; 
	
	
	/*组包开始*/
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

	/*组包结束*/
	
	/*发送数据*/
	for(i=0;i<(SendData_ProtocolHeader_Length+framelen);i++)
	{
		Usart_SendByte(macUSARTx,ucSendData[i]);
	}
	Pack_Num_Count();
}

//计算一包中的帧序号及一包中的数据长度
void Master_Send_Data(uint8_t *data,uint16_t size)
{
	/*ucFrameTotalNum:用于标识当前的数据共需要拆分几包数据发出*/
	/*ucFrameCurrentNum：用于标识当前发送的是总包数中的第几包*/
	/*ucSendBuffer数组用于作为数据的中转*/
	uint8_t  ucFrameTotalNum=0,ucFrameCurrentNum;
	uint16_t uiFrameLen=0;                         //用于记录此包数据需要几个帧序列可以发送完成
	uint8_t  ucSendBuffer[SendBuffer_Max_Length];
	uint16_t i=0,j=0;
	
	__disable_irq();
	
	if(size/SendBuffer_Max_Length==0) //发送数据不满512字节时
	{
		ucFrameTotalNum=0;
		uiFrameLen=size%SendBuffer_Max_Length;
		
		Master_Send_Data_Frame(data,uiFrameLen,0);
	}
	else if((size%SendBuffer_Max_Length==0)&&(size/SendBuffer_Max_Length>0))  //发送字节是512整数倍时
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
	else if((size%SendBuffer_Max_Length!=0)&&(size/SendBuffer_Max_Length>0)) //发送字不是大于512，但不是512的整数倍时
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



//响应从机的处理

extern uint8_t ucReciveBuffer[512+8];
extern uint16_t ucSalvePackLen;
extern uint8_t ucSetThreshold[12]; //主机气馈给从机的域值数据
uint8_t ucResponse[Response_Max_Length];

//除CRC以前的包头格式
//包头1、包头2、长度高、长度低、命令参数、包序号、帧序号
static uint8_t ucSlaveHeader[Slave_Header_Length-1]={Frame_Header1,\
																										Frame_Header2,\
																										0,\
																										0,\
																										0,\
																										0,\
																										0}; 


//参数说明：datalength 发送包的数据长度；cmd 命令参数
void Master_Response_Slave(uint8_t datalength, uint8_t cmd)
{
	uint8_t i;
	uint8_t ucCrcCheckSum=0;
	uint8_t ucSetThresholdCount=0;
	
	__disable_irq();
/******************反馈数据组包开始***********************/
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
/******************反馈数据组包结束**********************/	
	
/******************发送数据**********************/	
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

/*接收从机响应主机的数据*/
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
	if((ucSlaveCrcCheckSum&0xff)==ucReciveBuffer[ucSalvePackLen-1])  //判断根据接收到的数据得出的CRC与从机发送的CRC校验是否一致
	{
		switch(ucReciveBuffer[PackCmd_Posi])
		{
			case Cmd_Alarm: //报警命令
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_Alarm);
				break;
			}
			case Cmd_GetCarState: //获取车辆状态命令
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_GetCarState);
				break;
			}
			case Cmd_SendData:  //发送数据命令
			{
				Slave_Response_Master(ucSalvePackLen,Cmd_SendData);
				break;
			}
			case Cmd_SetThreshold: //设置域值命令
			{
				Master_Response_Slave(0x0c,Cmd_SetThreshold);
				break;
			}
			case Cmd_Report: //上报命令
			{
				Master_Response_Slave(0x00,Cmd_Report);
				break;
			}
			case Cmd_SetOTA: //在线升级命令
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





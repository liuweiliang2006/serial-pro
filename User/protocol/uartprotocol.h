#ifndef __UARTPROTOCOL_H
#define	__UARTPROTOCOL_H

#include "stdint.h"

/*数据包头 5A A5*/      
#define  Frame_Header1   		        0x5A
#define  Frame_Header2   		        0xA5

		
/*主机命令码*/		
#define  Cmd_Alarm       		        0xF1		//报警命令
#define  Cmd_GetCarState 		        0xF5		//获取车辆状态命令
#define  Cmd_SendData    		        0xF4		//发送数据命令

#define  Alarm_High_Length          0x00    // alarm命令数据长度高字节
#define  Alarm_Low_Length           0x04    //alarm命令数据长度低字节
#define  Alarm_Frame_Num            0x00    //帧序号
         
#define  GetCarState_High_Length    0x00
#define  GetCarState_Low_Length     0x00
#define  GetCarState_Frame_Num      0x00
         
#define  SendData_High_Length       0x18
#define  SendData_Low_Length        0x00
#define  SendData_High_Length_Posi  0x02    //senddata 数据长度高字节在发送包的位置 第二个字节
#define  SendData_Low_Length_Posi   0x03    //senddata 数据长度低字节在发送包的位置 第二个字节
#define  SendData_Fram_Posi         0x06    //帧序号所在的位置
         
#define  Package_Posi               0x05    //包序号所在的位置
#define  PackCmd_Posi               0x04    //命令参数所在的位置

//用于标识是否开始接收中断中的数据
#define  Enable                     0x01
#define  Disable                    0x00
#define  Error                      0x00
				
/*从机命令码*/				
#define  Cmd_SetThreshold		        0xE1		//设置域值命令
#define  Cmd_Report      		        0xE2		//上报命令
#define  Cmd_SetOTA      		        0xE3		//在线升级命令

#define  Response_High_Length_Posi  0x02    //主机响应从机发送数据中，数据长度高字节 
#define  Response_Low_Length_Posi   0x03    //主机响应从机发送数据中，数据长度低字节 
#define  Response_Freme_Num_Posi    0x06    //主机响应从机发送数据中，帧序号所在位置

#define  Slave_Header_Length        0x08    //主机响应从机发送数据中，帧头长度
#define  Response_Max_Length        0x14    //主机响应从机发送数据中，包的最大长度，包含帧头及数据、CRC


//主机主动发出
void Master_Send_Alarm_Data(uint8_t *data); //报警命令发送函数
void Master_Send_GetCarState_data(void);    //获取车辆状态命令发送函数
void Master_Send_Data_Frame(uint8_t *data,uint16_t len,uint8_t framenum); //发送数据命令发送函数
void Master_Send_Data(uint8_t *data,uint16_t size);

//主机响应从机命令
void Master_Response_SetThreshold(void);    //主机响应从机设置域值命令
void Master_Response_Report(void);          //主机响应从机上报命令
void Master_Response_SetOTA(void);          //主机响应从机在线升级命令

//接收中断中数据处理函数
uint8_t UcAnalysis_RecSlave_Data(void);
void Response_Slave(uint8_t datalength, uint8_t cmd);//Master recived slave data ,then Response salve function
void Slave_Response_Master(uint16_t lenght,uint8_t cmd);

#endif

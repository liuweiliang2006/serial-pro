#ifndef __UARTPROTOCOL_H
#define	__UARTPROTOCOL_H

#include "stdint.h"

/*���ݰ�ͷ 5A A5*/      
#define  Frame_Header1   		        0x5A
#define  Frame_Header2   		        0xA5

		
/*����������*/		
#define  Cmd_Alarm       		        0xF1		//��������
#define  Cmd_GetCarState 		        0xF5		//��ȡ����״̬����
#define  Cmd_SendData    		        0xF4		//������������

#define  Alarm_High_Length          0x00    // alarm�������ݳ��ȸ��ֽ�
#define  Alarm_Low_Length           0x04    //alarm�������ݳ��ȵ��ֽ�
#define  Alarm_Frame_Num            0x00    //֡���
         
#define  GetCarState_High_Length    0x00
#define  GetCarState_Low_Length     0x00
#define  GetCarState_Frame_Num      0x00
         
#define  SendData_High_Length       0x18
#define  SendData_Low_Length        0x00
#define  SendData_High_Length_Posi  0x02    //senddata ���ݳ��ȸ��ֽ��ڷ��Ͱ���λ�� �ڶ����ֽ�
#define  SendData_Low_Length_Posi   0x03    //senddata ���ݳ��ȵ��ֽ��ڷ��Ͱ���λ�� �ڶ����ֽ�
#define  SendData_Fram_Posi         0x06    //֡������ڵ�λ��
         
#define  Package_Posi               0x05    //��������ڵ�λ��
#define  PackCmd_Posi               0x04    //����������ڵ�λ��

//���ڱ�ʶ�Ƿ�ʼ�����ж��е�����
#define  Enable                     0x01
#define  Disable                    0x00
#define  Error                      0x00
				
/*�ӻ�������*/				
#define  Cmd_SetThreshold		        0xE1		//������ֵ����
#define  Cmd_Report      		        0xE2		//�ϱ�����
#define  Cmd_SetOTA      		        0xE3		//������������

#define  Response_High_Length_Posi  0x02    //������Ӧ�ӻ����������У����ݳ��ȸ��ֽ� 
#define  Response_Low_Length_Posi   0x03    //������Ӧ�ӻ����������У����ݳ��ȵ��ֽ� 
#define  Response_Freme_Num_Posi    0x06    //������Ӧ�ӻ����������У�֡�������λ��

#define  Slave_Header_Length        0x08    //������Ӧ�ӻ����������У�֡ͷ����
#define  Response_Max_Length        0x14    //������Ӧ�ӻ����������У�������󳤶ȣ�����֡ͷ�����ݡ�CRC


//������������
void Master_Send_Alarm_Data(uint8_t *data); //��������ͺ���
void Master_Send_GetCarState_data(void);    //��ȡ����״̬����ͺ���
void Master_Send_Data_Frame(uint8_t *data,uint16_t len,uint8_t framenum); //������������ͺ���
void Master_Send_Data(uint8_t *data,uint16_t size);

//������Ӧ�ӻ�����
void Master_Response_SetThreshold(void);    //������Ӧ�ӻ�������ֵ����
void Master_Response_Report(void);          //������Ӧ�ӻ��ϱ�����
void Master_Response_SetOTA(void);          //������Ӧ�ӻ�������������

//�����ж������ݴ�����
uint8_t UcAnalysis_RecSlave_Data(void);
void Response_Slave(uint8_t datalength, uint8_t cmd);//Master recived slave data ,then Response salve function
void Slave_Response_Master(uint16_t lenght,uint8_t cmd);

#endif

#ifndef I2C1_H
#define I2C1_H
#include "stm32f1_sys.h"

//IO操作函数
#define IIC1_SCL    PAout(4) //SCL
#define IIC1_SDA    PAout(5) //SDA
#define IIC1_READ_SDA   PAin(5)  //输入SDA
//IO方向设置
#define IIC1_SDA_IN()  IIC1_SDA=1	//输入模式
#define IIC1_SDA_OUT()  //输出模式

//IIC1所有操作函数
void IIC1_Init(void);                //初始化IIC1的IO口
void IIC1_Start(void);				//发送IIC1开始信号
void IIC1_Stop(void);	  			//发送IIC1停止信号
void IIC1_Send_Byte(u8 txd);			//IIC1发送一个字节
u8 IIC1_Read_Byte(unsigned char ack);//IIC1读取一个字节
u8 IIC1_Wait_Ack(void); 				//IIC1等待ACK信号
void IIC1_Ack(void);					//IIC1发送ACK信号
void IIC1_NAck(void);				//IIC1不发送ACK信号

#endif


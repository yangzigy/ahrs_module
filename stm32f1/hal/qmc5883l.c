#include "qmc5883l.h"

u8 QMC5883L_Read_Byte(u8 reg);
u8 QMC5883L_Write_Byte(u8 reg,u8 data);
u8 QMC5883L_Read_Len(u8 reg,u8 len,u8 *buf);

/////////////////////////////////////////////////////////////////////
//接口
static u8 qmcbuf[10];
void qmc_ini(void) 	//初始化
{
	u32 res;
	IIC1_Init();//初始化IIC1总线
	QMC5883L_Write_Byte(QMC5883L_SETPERIOD_REG,0X01);	//
	QMC5883L_Write_Byte(QMC5883L_CTRL2_REG,0X01);	//
	QMC5883L_Write_Byte(QMC5883L_CTRL_REG,0X05);	//512,2G,50Hz,continue
	//QMC5883L_Write_Byte(QMC5883L_CTRL_REG,0X1d);	//512,2G,50Hz,continue
	//QMC5883L_Write_Byte(0x20,0X40);
	//QMC5883L_Write_Byte(0x21,0X01);
	res=QMC5883L_Read_Byte(QMC5883L_SETPERIOD_REG);
	res=QMC5883L_Read_Byte(QMC5883L_CTRL_REG);
}
int QMC5883L_Tem=0;
int qmc_update(float *mx, float *my, float *mz)
{
	u8 res=QMC5883L_Read_Byte(QMC5883L_STAT_REG);
	if((res & 1)==0)
	{
		return 1;
	}
	QMC5883L_Read_Len(QMC5883L_DATA_XL_REG,6,qmcbuf);
	*mx = (*(s16*)(qmcbuf+0))/12000.0f;
	*my = (*(s16*)(qmcbuf+2))/12000.0f;
	*mz = (*(s16*)(qmcbuf+4))/12000.0f;
	//qmcbuf[7]=QMC5883L_Read_Byte(7);
	//qmcbuf[8]=QMC5883L_Read_Byte(8);
	//QMC5883L_Tem=*(s16*)(qmcbuf+7);
	return 0;
}
//IIC1连续读
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 QMC5883L_Read_Len(u8 reg,u8 len,u8 *buf)
{
	IIC1_Start();
	IIC1_Send_Byte((QMC5883L_ADDR<<1)|0);//发送器件地址+写命令
	if(IIC1_Wait_Ack())	//等待应答
	{
		IIC1_Stop();
		return 1;
	}
	IIC1_Send_Byte(reg);	//写寄存器地址
	IIC1_Wait_Ack();		//等待应答
	IIC1_Start();
	IIC1_Send_Byte((QMC5883L_ADDR<<1)|1);//发送器件地址+读命令
	IIC1_Wait_Ack();		//等待应答
	while(len)
	{
		if(len==1)
			*buf=IIC1_Read_Byte(0);//读数据,发送nACK
		else
			*buf=IIC1_Read_Byte(1);		//读数据,发送ACK
		len--;
		buf++;
	}
	IIC1_Stop();	//产生一个停止条件
	return 0;
}
//IIC1写一个字节
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 QMC5883L_Write_Byte(u8 reg,u8 data)
{
	IIC1_Start();
	IIC1_Send_Byte((QMC5883L_ADDR<<1)|0);//发送器件地址+写命令
	if(IIC1_Wait_Ack())	//等待应答
	{
		IIC1_Stop();
		return 1;
	}
	IIC1_Send_Byte(reg);	//写寄存器地址
	IIC1_Wait_Ack();		//等待应答
	IIC1_Send_Byte(data);//发送数据
	if(IIC1_Wait_Ack())	//等待ACK
	{
		IIC1_Stop();
		return 1;
	}
	IIC1_Stop();
	return 0;
}
//IIC1读一个字节
//reg:寄存器地址
//返回值:读到的数据
u8 QMC5883L_Read_Byte(u8 reg)
{
	u8 res;
	IIC1_Start();
	IIC1_Send_Byte((QMC5883L_ADDR<<1)|0);//发送器件地址+写命令
	IIC1_Wait_Ack();		//等待应答
	IIC1_Send_Byte(reg);	//写寄存器地址
	IIC1_Wait_Ack();		//等待应答
	IIC1_Start();
	IIC1_Send_Byte((QMC5883L_ADDR<<1)|1);//发送器件地址+读命令
	IIC1_Wait_Ack();		//等待应答
	res=IIC1_Read_Byte(0);//读取数据,发送nACK
	IIC1_Stop();			//产生一个停止条件
	return res;
}


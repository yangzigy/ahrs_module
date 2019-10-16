#include "i2c1.h"

extern void delay_us(u32 us);

//初始化IIC1
void IIC1_Init(void)
{
	//GPIO_Set(GPIOB,PIN0|PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB8/PB9设置
	MGPIOA->CT4=GPIO_OUT_PP;
	MGPIOA->CT5=GPIO_OUT_OD;
	IIC1_SCL=1;
	IIC1_SDA=1;
}
//产生IIC1起始信号
void IIC1_Start(void)
{
	IIC1_SDA_OUT();     //sda线输出
	IIC1_SDA=1;
	IIC1_SCL=1;
	delay_us(4);
	IIC1_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(4);
	IIC1_SCL=0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC1停止信号
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();//sda线输出
	IIC1_SCL=0;
	IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC1_SCL=1;
	IIC1_SDA=1;//发送I2C总线结束信号
	delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC1_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC1_SDA_IN();      //SDA设置为输入
	IIC1_SDA=1;delay_us(1);
	IIC1_SCL=1;delay_us(1);
	while(IIC1_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC1_Stop();
			return 1;
		}
	}
	IIC1_SCL=0;//时钟输出0
	return 0;
}
//产生ACK应答
void IIC1_Ack(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
//void IIC1_Ack(void)
//{
	//IIC1_SDA_OUT();
	//IIC1_SDA=0;
	//IIC1_SCL=0;
	//delay_us(2);
	//IIC1_SCL=1;
	//delay_us(2);
	//IIC1_SCL=0;
//}
//不产生ACK应答
void IIC1_NAck(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
//IIC1发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC1_Send_Byte(u8 txd)
{
	u8 t;
	IIC1_SDA_OUT();
	IIC1_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{
		IIC1_SDA=(txd&0x80)>>7;
		txd<<=1;
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC1_SCL=1;
		delay_us(2);
		IIC1_SCL=0;
		delay_us(2);
	}
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC1_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC1_SDA_IN();//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		IIC1_SCL=0;
		delay_us(2);
		IIC1_SCL=1;
		receive<<=1;
		if(IIC1_READ_SDA)receive++;
		delay_us(1);
	}
	if (!ack)
		IIC1_NAck();//发送nACK
	else
		IIC1_Ack(); //发送ACK
	return receive;
}


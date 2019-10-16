#include "mpu6050.h"

extern void delay_us(u32 us);
//初始化MPU6050
//返回值:0,成功
//其他,错误代码
u8 MPU_Init(void)
{
	u32 res;
	IIC_Init();//初始化IIC总线
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
	for(res=0;res<100000;res++)
	{
		delay_us(1);
	}
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050
	MPU_Set_Gyro_Fsr(0);					//陀螺仪传感器,
	MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU_Write_Byte(MPU_ACCEL_CFG2_REG,(0<<4) | 6); //4-8-16个平均，5Hz
	//MPU_Set_Rate(1000);						//设置采样率1000Hz
	MPU_Write_Byte(MPU_SAMPLE_RATE_REG,0X00);
	MPU_Write_Byte(MPU_CFG_REG,0X01);

	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	//MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	MPU_Write_Byte(MPU_INTBP_CFG_REG,2);	//INT引脚低电平有效
	res=MPU_Read_Byte(MPU_INTBP_CFG_REG);
	res=MPU_Read_Byte(MPU_SAMPLE_RATE_REG);
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	//if(res==MPU_ADDR)//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X03);	//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		//MPU_Set_Rate(1000);						//设置采样率为1000Hz
	}
	res=MPU_Read_Byte(MPU_PWR_MGMT1_REG);
	//else return 1;
	return 0;
}
//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围
}
static u8 buffer[30];
void MPU6050_Update(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float *t) 
{
	//u8 res=0;
	//res=MPU_Read_Byte(MPU_INT_STA_REG);
	//if((res & 1)==0)
		//return ;
	MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,14,buffer);
	*ax = (((int16_t)(((buffer[0]) << 8) | buffer[1])))/16384.0f;
	*ay = (((int16_t)(((buffer[2]) << 8) | buffer[3])))/16384.0f;
	*az = (((int16_t)(((buffer[4]) << 8) | buffer[5])))/16384.0f;
	*t =  ((int16_t)(((buffer[6]) << 8) | buffer[7]))/340.0f + 35.0f;
	*gx = (((int16_t)(((buffer[8]) << 8) | buffer[9])))/131.0f;
	*gy = (((int16_t)(((buffer[10]) << 8) | buffer[11])))/131.0f;
	*gz = (((int16_t)(((buffer[12]) << 8) | buffer[13])))/131.0f;
}
//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	IIC_Start();
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	IIC_Start();
	IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令
	IIC_Wait_Ack();		//等待应答
	while(len)
	{
		if(len==1)
			*buf=IIC_Read_Byte(0);//读数据,发送nACK
		else
			*buf=IIC_Read_Byte(1);		//读数据,发送ACK
		len--;
		buf++;
	}
	IIC_Stop();	//产生一个停止条件
	return 0;
}
//IIC写一个字节
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 reg,u8 data)
{
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	IIC_Send_Byte(data);//发送数据
	if(IIC_Wait_Ack())	//等待ACK
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0;
}
//IIC读一个字节
//reg:寄存器地址
//返回值:读到的数据
u8 MPU_Read_Byte(u8 reg)
{
	u8 res;
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令
	IIC_Wait_Ack();		//等待应答
	IIC_Send_Byte(reg);	//写寄存器地址
	IIC_Wait_Ack();		//等待应答
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令
	IIC_Wait_Ack();		//等待应答
	res=IIC_Read_Byte(0);//读取数据,发送nACK
	IIC_Stop();			//产生一个停止条件
	return res;
}


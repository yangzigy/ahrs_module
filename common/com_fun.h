#ifndef COM_FUN_H
#define COM_FUN_H

#include "main.h"

#pragma pack(1)
typedef struct 
{
	u8	syn;//	同步字 0xaa  
	u8	type;//1
	s16	gx; //陀螺仪x轴角速度，0.01度
	s16	gy; //陀螺仪y轴角速度，0.01度
	s16	gz; //陀螺仪z轴角速度，0.01度
	s16	ax; //加速度x轴，0.0001g
	s16	ay; //加速度y轴，0.0001g
	s16	az; //加速度z轴，0.0001g
	s16	mx; //磁力计x,0.0001G
	s16	my; //磁力计y,0.0001G
	s16	mz; //磁力计z,0.0001G
	u8	dtime; //距上次发送的时间，单位0.1ms
	u8	sum;//	校验和,从头开始累加
} S_OUT_1; //传感器数据包共22字节
#pragma pack()
#pragma pack(1)
typedef struct 
{
	u8	syn;//	同步字 0xaa  
	u8	type;//2
	s16	ux; //航空欧拉角x，0.01度
	s16	uy; //航空欧拉角y，0.01度
	s16	uz; //航空欧拉角z，0.01度
	float	q0; //四元数 w
	float	q1; //四元数 x
	float	q2; //四元数 y
	float	q3; //四元数 z
	u8	res; //保留
	u8	sum;//	校验和,从头开始累加
} S_OUT_2; //输出姿态数据，共26字节
#pragma pack()
#pragma pack(1)
typedef struct 
{
	u8	syn;//	同步字 0xaa  
	u8	type;//3
	s16	mkx; //磁力计比例x0.0001
	s16	moffx; //磁力计0偏x0.0001
	s16	mky; //磁力计比例y0.0001
	s16	moffy; //磁力计0偏y0.0001
	s16	mkz; //磁力计比例z0.0001
	s16	moffz; //磁力计0偏z0.0001
	s16	g_offset_x; //陀螺仪x轴零偏，0.001度
	s16	g_offset_y; //陀螺仪y轴零偏，0.001度
	s16	g_offset_z; //陀螺仪z轴零偏，0.001度
	u8	fre1; //传感数据包输出分频数（实际频率为500除以分频数）
	u8	fre2; //姿态数据包输出分频数（实际频率为500除以分频数）
	u8	is_cali :1;	//是否上电校准: 0：校准，1：不校准
	u8	mod :1;	//0：9轴模式1:6轴模式
	u8 baud	:4; //串口波特率
	u8 res	:2;
	u8	sum;//	校验和,从头开始累加
} S_OUT_3; //配置数据输出：24字节，配置指令包发送后回复
#pragma pack()
#pragma pack(1)
typedef struct 
{
	u8	syn;//	同步字 0xaa  
	u8	type;//4
	s16	data0; //数据内容由cmd决定
	s16	data1; //
	u8	cmd; //指令：
			//0：查询配置
			//1：data0为磁力计x比例0.0001，data1为0偏修正x
			//2：data0为磁力计y比例0.0001，data1为0偏修正y
			//3：data0为磁力计z比例0.0001，data1为0偏修正z
			//4：传感数据包输出分频数（实际频率为500除以分频数）
			//5：姿态数据包输出分频数（实际频率为500除以分频数）
			//6：是否上电校准：
			//data=0：校准，1：不校准，2：立刻校准
			//7：0：9轴模式1:6轴模式
			//8：设置串口波特率，data0=0~9表示
			//	9600,19200,28800,38400,57600,115200,
			//	230400,460800,500000,576000,921600
			//9：设置x轴陀螺零偏0.001度
			//10:设置y轴陀螺零偏0.001度
			//11:设置z轴陀螺零偏0.001度
			//12:设置x轴加计零偏0.001g
			//13:设置y轴加计零偏0.001g
			//14:设置z轴加计零偏0.001g
			//254：恢复出厂设置
			//255：保存当前配置
	u8	sum;//	校验和,从头开始累加
} S_IN_4; //配置指令包：8字节
#pragma pack()

u8 check_sum(u8 *p,int n);

#endif


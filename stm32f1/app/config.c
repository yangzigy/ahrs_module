
#include "config.h"
#include "f1_flash.h"

const S_OUT_3  config_dft=
{
	0xaa, //syn;//	同步字 0xaa  
	3, //type;//3
	10000,//mkx; //磁力计比例x
	0,//moffx; //磁力计0偏x
	10000,//mky; //磁力计比例y
	0,//moffy; //磁力计0偏y
	10000,//mkz; //磁力计比例z
	0,//moffz; //磁力计0偏z
	0,0,0, //陀螺零偏，不起作用
	5, //fre1; //传感数据包输出分频数（实际频率为500除以分频数）
	5, //fre2; //姿态数据包输出分频数（实际频率为500除以分频数）
	0,1,5, //mod; //bit0: 是否上电校准: 0：校准，1：不校准
				//bit1: 0：9轴模式1:6轴模式
				//bit2~bit5：串口波特率
	0, //sum;//	校验和,从头开始累加
}; //默认配置
S_OUT_3 config; //当前配置数据包

void cfg_load(void) //从flash读取配置到当前配置
{
	u8 *p8=(u8*)av_flash_read(); //从均匀擦写Flash中读取配置
	S_OUT_3 *p=(S_OUT_3*)(p8+4);
	if(p8==0)//若没有存储配置
	{
		//给默认配置
		config=config_dft;
		return ;
	}
	config=*p;
}
int cfg_save(void) //将当前配置保存
{
	u8 *p8;
	S_OUT_3 *p;
	int i;
	av_flash_write((u16*)&config,sizeof(config));
	p8=(u8*)av_flash_read(); //从均匀擦写Flash中读取配置
	p=(S_OUT_3*)(p8+4);
	if(p8==0)//若没有存储配置
	{
		return 1;
	}
	//检验是否正确
	for(i=0;i<sizeof(config);i++)
	{
		if(((u8*)&config)[i]!=((u8*)p)[i])
		{
			return 2;
		}
	}
	return 0;
}


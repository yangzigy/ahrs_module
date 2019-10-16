#include "f1_flash.h"

#pragma pack(1)
typedef struct
{
	//u32 ACR;
	u32 LATENCY:3; //<=24M:0, <=48M:1, <=72M:2
	u32 HLFCYA:1; //闪存半周期访问使能
	u32 PRFTBE:1; //预取缓冲区使能
	u32 PRFTBS:1; //预取缓冲区状态
	u32 res:26;
	u32 KEYR;
	u32 OPTKEYR;
	//u32 SR;
	u32 bsy:1; //忙
	u32 erlybsy:1;
	u32 PGERR:1; //失败（写1清零）
	u32 res2:1;
	u32 WRPRTERR:1; //写失败（写1清零）
	u32 EOP:1; //操作成功（中断使能时）
	u32 res3:26;
	//u32 CR;
	u32 PG:1; //MFLASH programming activated.
	u32 PER:1; //页擦除
	u32 MER:1; //Erase activated for all user sectors
	u32 res4:1; 
	u32 OPTPG:1; //烧写选项 
	u32 OPTER:1; //擦除选项 
	u32 STRT:1; //开始 
	u32 LOCK:1; //Write to 1 only
	u32 res5:1; 
	u32 OPTWRE:1; //允许写选项 
	u32 ERRIE:1; //This bit enables the interrupt generation when the OPERR bit in the FLASH_SR register is set to 1.
	u32 res6:1; 
	u32 EOPIE:1; //This bit enables the interrupt generation when the EOP bit in the FLASH_SR register goes to 1
	u32 res7:19; 
	u32 AR; //闪存编程地址
	u32 res8; 
	//u32 OBR; //
	u32 OPTERR:1;
	u32 RDPRT:1;
	u32 WDG_SW:1;
	u32 nRST_STOP:1;
	u32 nRST_STDB:1;
	u32 res9:5;
	u32 data2:8;
	u32 data1:8;
	u32 res10:6;
	u32 WRPR;
} MFLASH_TypeDef; //使用这个结构体造成hardfault
#pragma pack()
#define MFLASH               ((MFLASH_TypeDef *) FLASH_R_BASE)

int flash_erase(void *pf) //擦MFLASH扇区
{
	//解锁：
	FLASH->KEYR=0x45670123;
	FLASH->KEYR=0xCDEF89AB;
	while(FLASH->SR & 1); //bsy
	FLASH->CR |= (1<<1); //PER
	FLASH->AR=(u32)pf;
	FLASH->CR |= (1<<6); //START
	while(FLASH->SR & 1); //bsy
	FLASH->CR &= ~(1<<1); //PER
	//上锁
	FLASH->CR |= (1<<7); //LOCK 
	return 0;
}
int flash_write(u16 *p,int n,void *addr) //直接写入(按2字节)
{
	int i;
	//解锁：
	FLASH->KEYR=0x45670123;
	FLASH->KEYR=0xCDEF89AB;

	while(FLASH->SR & 1); //bsy
	FLASH->CR |= 1; //PG
	for(i = 0; i < n/2; i++)
	{
		((u16*)addr)[i]=p[i];
		while(FLASH->SR & 1); //bsy
	}
	FLASH->CR &= ~1; //PG
	//上锁
	FLASH->CR |= (1<<7); //LOCK 
	return 0;
}
int flash_eraseAwrite(u16 *p,int n,void *pf) //擦除并写入,输入数据指针，数据长度(单位字节)，MFLASH地址
{
	int i;
//擦除
	flash_erase(pf);
//写入
	flash_write(p,n,pf);
//校验
	for(i = 0; i < n/2; i++)
	{
		if(p[i]!=((u16*)pf)[i])
		{
			return 1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//均匀擦写模式
#define AV_FLASH_START 0x0800c800 //50K的位置
#define AV_FLASH_LEN (1024*10) //均匀擦写配置区域总长度(字节)
#define AV_FLASH_END (void*)(AV_FLASH_START+AV_FLASH_LEN)
#pragma pack(1)
typedef struct
{
	u16 h0; //0xffff表示有效或空闲
	u16 h1; //0xffff表示空闲
	u16 data[124/2]; //可用于配置数据存储的区域
} S_AV_FLASH; //大小必须是页大小的公约数
#pragma pack()

void *av_flash_read(void) //在均匀擦写Flash中读取数据，返回有效数据指针
{
	S_AV_FLASH *p=(S_AV_FLASH*)AV_FLASH_START;
	for(;p<AV_FLASH_END;p++)
	{
		if(p->h0==0xffff && p->h1==0) //若有效
		{
			return p;
		}
	}
	return 0;
}
int av_flash_write(u16 *p16,int n) //写，输入数据首指针，长度单位字节,必须在规定的大小内
{
	int w_flag=0; //是否写了
	S_AV_FLASH *p=(S_AV_FLASH*)AV_FLASH_START;
	if(n>sizeof(S_AV_FLASH)-4)
	{
		return -1; //长度错误
	}
start:
	for(;p<AV_FLASH_END;p++)
	{
		u16 tmp=0;
		if(p->h0==0xffff && p->h1==0) //若有效
		{
			flash_write(&tmp,2,(void*)&(p->h0)); //写成无效
			if(p->h0!=0)
			{
				return -2; //写无效失败
			}
		}
		else if(p->h0==0xffff && p->h1==0xffff) //若空闲,写，不管是否已经擦了有效的
		{
			int i;
			flash_write(&tmp,2,(void*)&(p->h1));
			if(p->h1!=0)
			{
				return -3; //写占用失败
			}
			flash_write(p16,n,p->data);
			for(i=0;i<n/2;i++)
			{
				if(p->data[i]!=p16[i])
				{
					return -4; //数据校验错误
				}
			}
			w_flag=1;
			break;
		}
	}
	if(w_flag==0) //若没找到空闲，就擦除所有
	{
		int i;
		for(i=AV_FLASH_START;i<(int)AV_FLASH_END;i+=FLASH_PAGE)
		{
			flash_erase((void *)i);
		}
		w_flag=2; //防止无限循环
		goto start;
	}
	else if(w_flag==2)
	{
		return -5; //擦除错误
	}
	return 0;
}


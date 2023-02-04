/*
文件名：comm_rec.c
时间：2013-9-15
版本：	V1.0

功能：

*/
#include "comm_rec.h"

void rec_sync(u8 b,COMM_SYNC* p)//同步方式接收函数
{
	p->rec_buff[p->rec_p++]=b;
	if(b==p->endc)
	{
		p->rec_buff[p->rec_p]=0;
		//结束，调用处理函数
		p->pro(p->rec_buff,p->rec_p);
		p->rec_p=0;
	}
	if(p->rec_p>=p->buf_len-1)//为了兼容C，数组的最后一个字节要留0
	{
		p->rec_p=p->buf_len-2;
	}
}

void rec_head(u8 b,COMM_HEAD* p)
{
	u8 *pback; //回溯位置
	u32 l=0; //回溯长度
	while(1)
	{
		if(p->pre_p<p->syncbuf_len)//正在寻找包头
		{
			if(b==p->SYNC[p->pre_p])//引导字正确
			{
				p->rec_buff[p->pre_p++]=b;
			}
			else p->pre_p=0;
		}
		else if(p->pre_p==p->pre_offset)//确定不同包的长度
		{
			p->rec_buff[p->pre_p++]=b;
			p->pack_len=p->pre_cb(p->rec_buff,p-> pre_p);
		}
		else//正常接收数据包
		{
			p->rec_buff[p->pre_p++]=b;
			if(p->pre_p>=p->pack_len)
			{
				if(p->pro(p->rec_buff, p->pack_len)) //调用处理函数
				{//若接收不正确
					if(l==0) //若还没开始回溯
					{
						l=p->pre_p-1; //回溯长度,用rec_p可能大于pack_len
					}
					pback=p->rec_buff + 1; //回溯位置
				}
				p->pre_p=0;
			}
		}
		if(l) //若有回溯任务
		{
			b=*pback;
			pback++; l--;
		}
		else return;
	}
}


/*
文件名：comm_rec.h
作者：北京交通大学 自控1102 杨孜
时间：2013-3-21
版本：	V1.0

功能：

*/
#ifndef COMM_REC_H
#define COMM_REC_H

#include "main.h"

typedef struct
{
	u8 *rec_buff;
	int buf_len;//缓冲区长度
	int rec_p;
	u8 (*pro)(u8 * b,int len);//用户处理函数指针
} COMM_SYNC;//同步组包
void comm_sync_ini(u8 *buf,int l,u8(*cb)(u8 * b,int len),COMM_SYNC* p);
void rec_sync(u8 b,COMM_SYNC* p);//同步方式接收函数


typedef struct 
{
	u8 *rec_buff;
	int buf_len;//缓冲区长度
	u8 *SYNC;//同步字
	int syncbuf_len;//同步字长度
	int pack_len;//包全长
	int pre_offset;//确定整包长度的位置
	int pre_p;
	int (*pre_cb)(u8 *b,int len);//返回整包长度
	int (*pro)(u8 * b,int len);//返回是否正确接收
}COMM_HEAD;//不定长数据组包
void comm_head_ini(u8 *buf,int l,u8*sync_p,int sync_n,u8 (*cb)(u8 * b,int len),u8 (*pre_cb)(u8 * b,int len),int pre_offset,COMM_HEAD* p);
void rec_head(u8 b,COMM_HEAD* p);
int pre_cb(u8*b, int len);


#endif

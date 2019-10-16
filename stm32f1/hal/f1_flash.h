/*
flash.h
必须使用C的方式包含
*/
#ifndef FLASH_H
#define FLASH_H

#include "stm32f1_sys.h"

//stm32f103c8t6是中容量产品，Flash为64k，SRAM为20k
//按照不同容量，存储器组织成32个1K字节/页(小容量)、128个1K字节/页(中容量)、128个2K字节/页(互联型)、256个2K字节/页(大容量)的主存储器块和一个信息块
//起始：0x0800 0000 中容量的终止：0x0801 FFFF
//Flash擦除后值为0xffff

#define FLASH_PAGE 1024
#define USER_FLASH_START 0x0800c800 //50K的位置

int flash_erase(void *pf); //擦MFLASH扇区
int flash_write(u16 *p,int n,void *addr); //直接写入(按2字节)
int flash_eraseAwrite(u16 *p,int n,void *pf); //擦除并写入,输入数据指针，数据长度，MFLASH地址
void *av_flash_read(void); //在均匀擦写Flash中读取数据，返回有效数据指针
int av_flash_write(u16 *p16,int n); //写，输入数据首指针，长度单位字节,必须在规定的大小内

#endif


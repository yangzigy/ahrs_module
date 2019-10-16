/*
文件名：config.h
时间：2017-7-29
功能：

*/
#ifndef CONFIG_H
#define CONFIG_H

#include "com_fun.h"

extern S_OUT_3 config; //当前配置
extern const S_OUT_3 config_dft; //默认配置

void cfg_load(void); //从flash读取配置到当前配置
int cfg_save(void); //将当前配置保存

#endif


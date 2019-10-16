#ifndef QMC5883L_H
#define QMC5883L_H

#include "i2c1.h"

#define QMC5883L_DATA_XL_REG		0X00	//x轴低
#define QMC5883L_DATA_XH_REG		0X01	//x轴高
#define QMC5883L_DATA_YL_REG		0X02	//y轴低
#define QMC5883L_DATA_YH_REG		0X03	//y轴高
#define QMC5883L_DATA_ZL_REG		0X04	//z轴低
#define QMC5883L_DATA_ZH_REG		0X05	//z轴高
#define QMC5883L_STAT_REG			0X06	//状态
#define QMC5883L_T_L_REG			0X07	//温度低
#define QMC5883L_T_H_REG			0X08	//温度高
#define QMC5883L_CTRL_REG			0X09	//配置OSR RNG ODR MODE各2bit
#define QMC5883L_CTRL2_REG			0X0a	//配置
#define QMC5883L_SETPERIOD_REG		0X0b	//建议写1
#define QMC5883L_CHIPID_REG			0X0d	//0xff

#define QMC5883L_ADDR				0X0d

void qmc_ini(void); 	//初始化
int qmc_update(float *mx, float *my, float *mz);

#endif


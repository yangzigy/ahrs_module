#ifndef AHRSGUI_H
#define AHRSGUI_H

#include "main.h"
extern "C"
{
#include "comm_rec.h"
#include "com_fun.h"
}
#include "qvector3d.h"

extern S_OUT_1 rxbuf1;
extern S_OUT_2 rxbuf2;
extern S_OUT_3 rxbuf3;
extern COMM_HEAD ahrsguitask_head;
void ahrs_tx_cmd(u8 cmd,s16 d0,s16 d1);

extern vector<QVector3D> cali_m_data; //磁力计校准数据
extern QVector3D cali_m_k; //磁力计校准k值
extern QVector3D cali_m_0; //磁力计校准0偏
void ahrs_cali_m_start(void); //开始磁力计校准
int ahrs_cali_m_stop(void); //停止

#endif


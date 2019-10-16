
#include "ahrsgui.h"

S_OUT_1 rxbuf1;
S_OUT_2 rxbuf2;
S_OUT_3 rxbuf3;
S_IN_4 txbuf;
//通信
int pro_ahrsguitask(u8 *b,int len);//返回整包长度
int pre_ahrsguitask(u8 *b,int len);//返回整包长度
static const u8 synbuf[]={0xaa};
u8 ahrsguitask_headbuf[256];
COMM_HEAD ahrsguitask_head=
{
	ahrsguitask_headbuf,
	sizeof(ahrsguitask_headbuf),
	(u8*)synbuf, //同步字
	sizeof(synbuf),
	sizeof(S_OUT_1),//pack_len
	1,//pre_offset
	0,//pre_p
	pre_ahrsguitask,
	pro_ahrsguitask, //修改处理方法
};
#define P1 (*(S_OUT_1*)p)
#define P2 (*(S_OUT_2*)p)
#define P3 (*(S_OUT_3*)p)
int pre_ahrsguitask(u8 *p,int len)//返回整包长度
{
	int dlen=sizeof(S_OUT_1);
	switch(p[1])
	{
	case 1:
		return sizeof(S_OUT_1);
	case 2:
		return sizeof(S_OUT_2);
	case 3:
		return sizeof(S_OUT_3);
	case 4:
		return sizeof(S_IN_4);
	default:
		break;
	}
	return dlen;
}
extern void ahrs_rx_pro(int type); //设备接收函数
int pro_ahrsguitask(u8 *p,int n)//返回整包长度
{
	int i;
	u8 sum=check_sum(p,n-1);
	if(sum!=p[n-1]) 
	{
		return 1;
	}
	switch(p[1])
	{
	case 1:
		rxbuf1=P1;
		break;
	case 2:
		rxbuf2=P2;
		break;
	case 3:
		rxbuf3=P3;
		break;
	case 4:
		break;
	default:
		break;
	}
	ahrs_rx_pro(p[1]);
	return 0;
}
extern void ahrs_uart_send(u8 *p,int n);
void ahrs_tx_cmd(u8 cmd,s16 d0,s16 d1)
{
	txbuf.syn=0xaa;
	txbuf.type=4;
	txbuf.data0=d0;
	txbuf.data1=d1;
	txbuf.cmd=cmd;
	txbuf.sum=check_sum((u8*)&txbuf,sizeof(txbuf)-1);
	ahrs_uart_send((u8*)&txbuf,sizeof(txbuf));
}

vector<QVector3D> cali_m_data; //磁力计校准数据
QVector3D cali_m_k; //磁力计校准k值
QVector3D cali_m_0; //磁力计校准0偏
void ahrs_cali_m_start(void) //开始磁力计校准
{
	cali_m_data.clear();
}
int ahrs_cali_m_stop(void) //停止
{
	if(cali_m_data.size()<100) return 1; //数量过少
	//找各方向上的次大的那个
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.x()<b.x();
		});
	float min_x=cali_m_data[1].x();
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.x()>b.x();
		});
	float max_x=cali_m_data[1].x();
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.y()<b.y();
		});
	float min_y=cali_m_data[1].y();
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.y()>b.y();
		});
	float max_y=cali_m_data[1].y();
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.z()<b.z();
		});
	float min_z=cali_m_data[1].z();
	nth_element(cali_m_data.begin(),cali_m_data.begin()+1,cali_m_data.end(),
		[](QVector3D &a,QVector3D&b)->bool
		{
			return a.z()>b.z();
		});
	float max_z=cali_m_data[1].z();
	//根据极值，求直线方程     y=k(x-x0)
	cali_m_0.setX((max_x+min_x)/2);
	cali_m_k.setX(1/(max_x-min_x)); //设地磁为0.5G
	cali_m_0.setY((max_y+min_y)/2);
	cali_m_k.setY(1/(max_y-min_y)); //设地磁为0.5G
	cali_m_0.setZ((max_z+min_z)/2);
	cali_m_k.setZ(1/(max_z-min_z)); //设地磁为0.5G
	return 0;
}

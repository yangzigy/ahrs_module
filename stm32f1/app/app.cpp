extern "C"
{
#include "queue.h"
#include "comm_rec.h"
#include "com_fun.h"
#include "uart.h"
#include "cmd.h"
#include "config.h"
void app_ini(void);
void app_poll(void);
#include "mpu6050.h"
#include "qmc5883l.h"
}
#include "ahrsv2.h"

const u32 com_baud_tab[]=//串口波特率表
{
	9600,19200,28800,38400,57600,115200,
	230400,460800,500000,576000,921600,
};

//全局变量
int printflag=0; //打印标志
float procT=0; //处理时间单位ms

CAhrs ahrs;
Vector3D org_gyro; //陀螺数据,(原始值)
Vector3D org_acce; //加计数据,(原始值)
Vector3D org_mag; //罗盘数据,(原始值)
Vector3D cur_gyro; //陀螺数据,(度)(输出接口)
Vector3D cur_acce; //加计数据,(g)(输出接口)
Vector3D cur_mag; //罗盘数据,(ad)(输出接口)
Vector3D cur_eular; //当前欧拉角,(度)(输出接口)

S_OUT_1 out1buf; //发送传感数据包缓冲
S_OUT_2 out2buf; //发送姿态数据包缓冲
void send_out1(u8 dtime) //发送1号协议，传感数据
{
	out1buf.syn=0xaa;
	out1buf.type=1;
	out1buf.gx=cur_gyro.x*100;	//
	out1buf.gy=cur_gyro.y*100;	//
	out1buf.gz=cur_gyro.z*100;	//陀螺数据，校准后，0.01度
	out1buf.ax=cur_acce.x*10000;	//
	out1buf.ay=cur_acce.y*10000;	//
	out1buf.az=cur_acce.z*10000;	//加计数据，校准后，0.0001g
	out1buf.mx=cur_mag.x*10000;	//
	out1buf.my=cur_mag.y*10000;	//
	out1buf.mz=cur_mag.z*10000;	//磁罗盘
	out1buf.dtime=dtime;
	out1buf.sum=check_sum((u8*)&out1buf,sizeof(out1buf)-1);
	uart_send((u8*)&out1buf,sizeof(out1buf),&uart2);
}
void send_out2(void) //发送2号协议，姿态数据
{
	out2buf.syn=0xaa;
	out2buf.type=2;
	out2buf.ux=cur_eular.x*100;	//
	out2buf.uy=cur_eular.y*100;	//
	out2buf.uz=cur_eular.z*100;	//欧拉角，0.01度
	out2buf.q0=ahrs.cur_quat.w;
	out2buf.q1=ahrs.cur_quat.x;
	out2buf.q2=ahrs.cur_quat.y;
	out2buf.q3=ahrs.cur_quat.z;
	out2buf.sum=check_sum((u8*)&out2buf,sizeof(out2buf)-1);
	uart_send((u8*)&out2buf,sizeof(out2buf),&uart2);
}
void send_out3(void) //发送2号协议，姿态数据
{
	config.syn=0xaa;
	config.type=3;
	config.g_offset_x=ahrs.g_offset.x*1000;
	config.g_offset_y=ahrs.g_offset.y*1000;
	config.g_offset_z=ahrs.g_offset.z*1000;
	config.sum=check_sum((u8*)&config,sizeof(config)-1);
	uart_send((u8*)&config,sizeof(config),&uart2);
}

//////////////////////////////////////////////////
//初始化
extern CMD_LIST bl_sys_cmd_list;
void app_ini(void)
{
	cmd_reg((CMD_LIST*)&bl_sys_cmd_list);
	ahrs.ini();
	//将配置文件内容加载到变量
	if(config.is_cali==0) //若需要校准
	{
		ahrs.calibrate_end_tick=1000;
	}
	else ahrs.calibrate_end_tick=0;
	uart_initial(&uart2,com_baud_tab[config.baud]); //工作串口
	ahrs.m_k.x=config.mkx*0.0001f;
	ahrs.m_k.y=config.mky*0.0001f;
	ahrs.m_k.z=config.mkz*0.0001f;
	ahrs.m_0.x=config.moffx*0.0001f;
	ahrs.m_0.y=config.moffy*0.0001f;
	ahrs.m_0.z=config.moffz*0.0001f;
	ahrs.is_use_mag=1-config.mod;
}
//////////////////////////////////////////////////
//功能函数
float gx, gy, gz, mpu_temp, ax, ay, az;
void get_org_data(void) //获取本身的陀螺信息
{
	MPU6050_Update(&ax, &ay, &az, &gx, &gy, &gz, &mpu_temp);
	org_gyro.x=gx;
	org_gyro.y=gy;
	org_gyro.z=gz;
	org_acce.x=ax;
	org_acce.y=ay;
	org_acce.z=az;
	if(qmc_update(&ax, &ay, &az)==0)
	{
		org_mag.x=ax;
		org_mag.y=ay;
		org_mag.z=az;
	}
}
//////////////////////////////////////////////////
//正式运行
int first_time=100; //首次运行轮空
void app_poll(void)
{
	static u32 pre_time=0;
	static u32 start_time=0;
	static u32 tick=0;
	start_time=SYS_time;
	float T=(SYS_time-pre_time)/10000.0f;
	pre_time=SYS_time;
//////////////////////////////////////////////////////////////
//惯导部分
	get_org_data();
	if(first_time)
	{
		first_time--;
		printf("%.2f	%.2f	%.2f	",org_gyro.x,org_gyro.y,org_gyro.z);
		return ;
	}
	ahrs.cur_temp=mpu_temp; //给温度赋值
	ahrs.imu_run_ground(org_gyro,org_acce,org_mag,T);
	//ahrs.imu_pos(T);
	cur_gyro=rad2deg(ahrs.in_gv); //当前角速度
	cur_acce=ahrs.in_av;
	cur_mag=ahrs.in_mv;
	cur_eular=ahrs.cur_quat.toEuler_zyx();
	cur_eular=rad2deg(cur_eular);
	//cur_eular=rad2deg(ahrs.eular);
//////////////////////////////////////////////////////////////
//控制
	if(tick%config.fre1==0) //发送传感包
	{
		static u32 pre_send_t=0;
		u32 t_delta=SYS_time-pre_send_t;
		//发送协议
		send_out1(t_delta);
		pre_send_t=SYS_time;
	}
	if(tick%config.fre2==0) //发送姿态包
	{
		send_out2();
	}
	tick++;
}

//通信
int pro_configtask(u8 *b,int len);//返回整包长度
int pre_configtask(u8 *b,int len);//返回整包长度
static const u8 synbuf[]={0xaa};
u8 configtask_headbuf[256];
COMM_HEAD configtask_head=
{
	configtask_headbuf,
	sizeof(configtask_headbuf),
	(u8*)synbuf, //同步字
	sizeof(synbuf),
	sizeof(S_IN_4),//pack_len
	0,//pre_offset
	0,//pre_p
	pre_configtask,
	pro_configtask, //修改处理方法
};
#define P4 (*(S_IN_4*)p)
int pre_configtask(u8 *p,int len)//返回整包长度
{
	int dlen=sizeof(S_IN_4);
	if(p[1]==4)
	{
		dlen=sizeof(S_IN_4);
	}
	return dlen;
}
int pro_configtask(u8 *p,int n)//返回整包长度
{
	u8 sum=check_sum(p,n-1);
	if(sum!=p[n-1]) 
	{
		return 1;
	}
	if(p[1]!=4) //处理参数设置
	{
		return 0;
	}
	switch(P4.cmd)
	{
	case 1: //data0为磁力计x
		config.mkx=P4.data0;
		ahrs.m_k.x=config.mkx*0.0001f;
		config.moffx=P4.data1;
		ahrs.m_0.x=config.moffx*0.0001f;
		break;
	case 2: //data0为磁力计y
		config.mky=P4.data0;
		ahrs.m_k.y=config.mky*0.0001f;
		config.moffy=P4.data1;
		ahrs.m_0.y=config.moffy*0.0001f;
		break;
	case 3: //data0为磁力计z
		config.mkz=P4.data0;
		ahrs.m_k.z=config.mkz*0.0001f;
		config.moffz=P4.data1;
		ahrs.m_0.z=config.moffz*0.0001f;
		break;
	case 4: //传感数据包输出分频数（实际频率为500除以分频数）
		config.fre1=P4.data0;
		break;
	case 5: //姿态数据包输出分频数（实际频率为500除以分频数）
		config.fre2=P4.data0;
		break;
	case 6: //是否上电校准：//:data=0：校准，1：不校准，2：立刻校准
		if(P4.data0==0) config.is_cali=1;
		else if(P4.data0==1) config.is_cali=0;
		else if(P4.data0==2)
		{
			ahrs.calibrate_end_tick=1000;
		}
		break;
	case 7: //0：9轴模式1:6轴模式
		if(P4.data0==0) config.mod=0;
		else if(P4.data0==1) config.mod=1;
		ahrs.is_use_mag=1-config.mod;
		break;
	case 8: //设置串口波特率，data0=0~9
		if(P4.data0>=0 && P4.data0<=9)
		{
			config.baud=P4.data0;
			//重新初始化串口
			uart_initial(&uart2,com_baud_tab[config.baud]); //工作串口
		}
		break;
	case 9: //设置x轴陀螺零偏
		ahrs.g_offset.x=P4.data0*0.001f;
		break;
	case 10: //设置y轴陀螺零偏
		ahrs.g_offset.y=P4.data0*0.001f;
		break;
	case 11: //设置z轴陀螺零偏
		ahrs.g_offset.z=P4.data0*0.001f;
		break;
	case 12: //设置x轴加计零偏
		ahrs.a_offset.x=P4.data0*0.001f;
		break;
	case 13: //设置y轴加计零偏
		ahrs.a_offset.y=P4.data0*0.001f;
		break;
	case 14: //设置z轴加计零偏
		ahrs.a_offset.z=P4.data0*0.001f;
		break;
	case 254: //恢复出厂设置
		config=config_dft;
		break;
	case 255: //保存当前配置
		cfg_save();
		break;
	default:
		break;
	}
	send_out3();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//			指令
void cmdfun_0(s8 *para) //不显示
{
	printflag=0;
}
void cmdfun_1(s8 *para) { printflag=1; }
void cmdfun_2(s8 *para) { printflag=2; }
void cmdfun_disconfig(s8 *para)
{
	u32 t;
	if(sscanf((char*)para,"%d",&t)==1)
	{
		printflag=t;
	}
}
void cmdfun_save(s8 *para) //保存当前配置
{
	int r=cfg_save();
	if (r)
	{
		printf("save failed");
	}
	else
	{
		printf("save success");
	}
}
static const CMD_STR blcmd_str[]=\
{
	(s8 *)"0",(s8 *)"no display",cmdfun_0,
	(s8 *)"1",(s8 *)"display ",cmdfun_1,
	(s8 *)"2",(s8 *)"display ",cmdfun_2,
	(s8 *)"s",(s8 *)"select a display ",cmdfun_disconfig,
	(s8 *)"save",(s8 *)"save curent cfg",cmdfun_save,
};
CMD_LIST bl_sys_cmd_list={blcmd_str,sizeof(blcmd_str)/sizeof(CMD_STR),0};


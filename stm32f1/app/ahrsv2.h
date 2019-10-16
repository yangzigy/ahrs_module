/*
文件名：ahrs.h
作者：北京交通大学 杨孜
时间：2017-1-1
功能：
	实现姿态解算基本算法
*/
#ifndef AHRS_H
#define AHRS_H

//#include "common.h"
#include "geometric.h"

class CAhrs
{
public:
	Vector3D g_offset; //陀螺零偏
	Vector3D a_offset; //加计零偏
	Vector3D m_k; //磁力计K
	Vector3D m_0; //磁力计0偏
	float g_offset_s; //转子校准标准差
	float cur_temp; //当前温度
//输入
	Vector3D in_gv; //输入角速度
	Vector3D in_av; //输入加速度(g)
	Vector3D in_mv; //输入磁力计
//输出
	QuatVec cur_quat; //当前累积的四元数
	Vector3D eular; //解算欧拉角zyx
	Vector3D speed; //解算速度
	Vector3D pos; //解算位置
//变量
	float a_mag; //加计幅度（滤波后）
	float a_mag_f; //加计幅度标准计算值
	float a_pri; //加计权
	Vector3D cur_ang_a; //当前加计姿态
	Vector3D Gerr;
	Vector3D Gerr_acc; //加计重力累积误差，加速恢复
	Vector3D Gerr_pre;
	Vector3D d_pre;
	Vector3D u;
	Vector3D up;
	Vector3D ui;
	Vector3D ud;

	int is_use_mag; //是否使用罗盘
	float m_mag; //罗盘幅度（滤波后）
	float m_mag_f; //罗盘幅度标准计算值
	float m_pri; //罗盘权
	Vector3D cur_ang_m; //当前罗盘姿态
	Vector3D Merr;
	Vector3D Merr_acc; //罗盘累积误差，加速恢复
	Vector3D mu;
	Vector3D mup;
	Vector3D mui;
	float delta_yaw;
//参数
	float kp; //加计矫正P
	float ki; //加计矫正I
	float kd; //加计矫正D
	float cur_a_k; //加计姿态滤波系数
	float evel_a_k; //加计评估滤波系数
	float cur_m_k; //罗盘姿态滤波系数
	float evel_m_k; //罗盘评估滤波系数
//函数
	CAhrs()
	{
		g_offset.x=0; //陀螺零偏
		g_offset.y=0; //陀螺零偏
		g_offset.z=0; //陀螺零偏
		a_offset.x=0; //加计零偏
		a_offset.y=0; //加计零偏
		a_offset.z=0; //加计零偏
		m_k.x=1; m_k.y=1; m_k.z=1;
		m_0.x=0; m_0.y=0; m_0.z=0;
		kp=0.3f; //
		ki=0.0001f; //
		kd=30;
		cur_a_k=0.1; //加计姿态滤波系数
		evel_a_k=0.001; //加计评估滤波系数
		cur_m_k=0.008f; //罗盘姿态滤波系数
		evel_m_k=0.005; //罗盘评估滤波系数
		is_first=2000; //是否是第一次
		speed=Vector3DEmpty;
		pos=Vector3DEmpty;
		g_offset_s=0;
		//cur_temp=-1000; //若为无效值则使用校准值
		cur_temp=0; //若为无效值则使用校准值
		Gerr=Vector3DEmpty;
		Gerr_acc=Vector3DEmpty;
		Merr=Vector3DEmpty;
		Merr_acc=Vector3DEmpty;
		a_mag_f=1;

		is_use_mag=0; //是否使用罗盘
	}
	~CAhrs(){}
	int is_first; //是否是第一次
	void ini(void);
	int calibrate_times;
	int calibrate_end_tick; //截止标定次数
	Vector3D calibrate_g_buf; //标定陀螺平均值
	Vector3D calibrate_g_err; //标定陀螺误差值
	int calibrate_g(Vector3D &gv); //校准陀螺,返回是否完成初始化，0完成
	float get_a_pri(float d); //获得加计权值,滤波后幅值
	float get_m_pri(float d); //获得罗盘权值,滤波后幅值
	void imu_run_ground(Vector3D &gv,Vector3D &av,Vector3D &mv,float T); //输入为角速度(弧度)和加速度(g)
	void imu_pos(float T); //位置解算
};
#endif


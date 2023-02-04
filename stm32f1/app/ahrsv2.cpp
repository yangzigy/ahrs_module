/*
文件名：ahrs.cpp
作者：北京交通大学 杨孜
时间：2017-1-1
功能：
	实现姿态解算基本算法
*/
#include "ahrsv2.h"

float sqr(float d)
{
	return d*d;
}
void CAhrs::ini(void)
{
	int i;
	cur_quat.w=1;
	cur_quat.x=0;
	cur_quat.y=0;
	cur_quat.z=0;
	cur_ang_a.x=0;
	cur_ang_a.y=0;
	cur_ang_a.z=0;
	calibrate_times=0; //标定
	calibrate_end_tick=1000;
}
int CAhrs::calibrate_g(Vector3D &gv) //校准陀螺,输入角度
{
	if(calibrate_end_tick)
	{
		if (fabs(gv.x)>5 || fabs(gv.y)>5 || fabs(gv.z)>5)
		{
			return 1;
		}
		if(calibrate_times==0)
		{
			calibrate_g_buf=gv;
			calibrate_g_err=Vector3DEmpty;
		}
		else
		{
			calibrate_g_buf=calibrate_g_buf*0.999f+gv*0.001f;
			calibrate_g_err.x=calibrate_g_err.x+fabs(calibrate_g_buf.x-gv.x);
			calibrate_g_err.y=calibrate_g_err.y+fabs(calibrate_g_buf.y-gv.y);
			calibrate_g_err.z=calibrate_g_err.z+fabs(calibrate_g_buf.z-gv.z);
		}
		calibrate_times++;
		if (calibrate_times>=calibrate_end_tick) //标定完成
		{
			calibrate_g_err=calibrate_g_err/(float)calibrate_times;
			printf("calibrate_g_err:%.2f,%.2f,%.2f\r\n",
				calibrate_g_err.x,calibrate_g_err.y,calibrate_g_err.z);
			if(calibrate_g_err.magnitude()<3) //若误差合格
			{
				g_offset=calibrate_g_buf;
			}
			calibrate_end_tick=0; //标定完成
			calibrate_times=0;
		}
		return 1;
	}
	in_gv=deg2rad(gv-g_offset);
	return 0;
}

float CAhrs::get_a_pri(float d) //获得加计权值,滤波后幅值
{
	d=fabs(d-a_mag_f); //与重力误差的绝对值
	if(d>0.01f) return 0.0001f; //若太大就不算
	d/=0.01f;
	d=1-sqrt(sqrt(d));
	return d;
}
float CAhrs::get_m_pri(float d) //获得罗盘权值,滤波后幅值
{
	d=fabs(d-m_mag_f); //与罗盘标准值误差的绝对值
	if(d>0.05f) return 0.0001f; //若太大就不算
	d/=0.05f;
	d=1-sqrt(sqrt(d));
	return d;
}
void CAhrs::imu_run_ground(Vector3D &gv,Vector3D &av,Vector3D &mv,float T) //输入为角速度(弧度)和加速度(g),磁力计,时间单位s
{
	static u32 tick=0;
	float P=kp,I=ki,D=kd;
/////////////////////////////////////////////////////////
//			输入矫正
/////////////////////////////////////////////////////////
	in_av.x=av.x-a_offset.x;
	in_av.y=av.y-a_offset.y;
	in_av.z=av.z-a_offset.z;
	in_mv.x=m_k.x*(mv.x - m_0.x);
	in_mv.y=m_k.y*(mv.y - m_0.y);
	in_mv.z=m_k.z*(mv.z - m_0.z);
	if(calibrate_g(gv))
	{
		return ; //若没完成初始化，就返回
	}
/////////////////////////////////////////////////////////
//			评估加计,与罗盘权
/////////////////////////////////////////////////////////
	cur_ang_a=cur_ang_a*(1-cur_a_k) + in_av*cur_a_k; //滤波重力向量
	a_mag=cur_ang_a.magnitude();
	cur_ang_m=cur_ang_m*(1-cur_m_k) + in_mv*cur_m_k;
	m_mag=cur_ang_m.magnitude();
	if(tick++%10==0)
	{
		if(a_mag>0.8f && a_mag<1.2f)
		{
			a_mag_f=a_mag_f*(1-evel_a_k) + a_mag*evel_a_k;
		}
		m_mag_f=m_mag_f*(1-evel_m_k) + m_mag*evel_m_k;
	}
	a_pri=get_a_pri(a_mag); //得到当前重力信度
	P*=a_pri;
	I*=a_pri;
	D*=a_pri;
/////////////////////////////////////////////////////////
//			求当前重力向量
/////////////////////////////////////////////////////////
	Vector3D z1;
	z1.x=0;
	z1.y=0;
	z1.z=1;
	//陀螺反方向旋转得到重力向量
	QuatVec tmpq;
	tmpq=cur_quat;
	tmpq.x=-tmpq.x;
	tmpq.y=-tmpq.y;
	tmpq.z=-tmpq.z; //反方向旋转
	tmpq.rot(z1); //陀螺传感的重力方向(都是载体坐标系)
/////////////////////////////////////////////////////////
//			加计重力与当前重力误差
/////////////////////////////////////////////////////////
	//Gerr=Gerr*0.99+(cur_ang_a^z1)*0.01; //误差
	Gerr=cur_ang_a^z1; //误差
	Gerr_acc=Gerr_acc+Gerr*I; //积分，加速最后小范围收敛
	if(is_first) //若是第一次
	{
		P=3;
		a_mag_f=a_mag;
		is_first--;
	}
	float eabs_x=fabs(Gerr.x);
	float eabs_y=fabs(Gerr.y);
	float eabs_z=fabs(Gerr.z);
	up=Gerr*P*1000;
	d_pre=d_pre*0.99f+(Gerr-Gerr_pre)*0.01f;
	ud=d_pre*D*1000;
	Gerr_pre=Gerr;
	//太大了，不积分
	if(eabs_x>deg2rad(1.0f) || eabs_x<deg2rad(0.1f)) Gerr_acc.x-=Gerr.x*I;
	if(eabs_y>deg2rad(1.0f) || eabs_y<deg2rad(0.1f)) Gerr_acc.y-=Gerr.y*I;
	if(eabs_z>deg2rad(1.0f) || eabs_z<deg2rad(0.1f)) Gerr_acc.z-=Gerr.z*I;
	//简单积分，超调抑制
	if(Gerr.x*Gerr_acc.x<0)
	{
		if(eabs_x<deg2rad(0.1f))
		{
			up.x*=3;
		}
		Gerr_acc.x+=Gerr.x*I*3; //超调加速积分
		//Gerr_acc.x*=0.98f; //大范围内直接削弱
	}
	if(Gerr.y*Gerr_acc.y<0)
	{
		if(eabs_y<deg2rad(0.1f))
		{
			up.y*=3;
		}
		Gerr_acc.y+=Gerr.y*I*3; //超调加速积分
		//Gerr_acc.y*=0.98f; //大范围内直接削弱
	}
	if(Gerr.z*Gerr_acc.z<0)
	{
		if(eabs_z<deg2rad(0.1f))
		{
			up.z*=3;
		}
		Gerr_acc.z+=Gerr.z*I*3; //超调加速积分
		//Gerr_acc.z*=0.98f; //大范围内直接削弱
	}
	//简单积分积分限幅；
	if(Gerr_acc.x>deg2rad(0.3f)) {Gerr_acc.x=deg2rad(0.3f);}
	else if(Gerr_acc.x<-deg2rad(0.3f)) {Gerr_acc.x=-deg2rad(0.3f);}
	if(Gerr_acc.y>deg2rad(0.3f)) {Gerr_acc.y=deg2rad(0.3f);}
	else if(Gerr_acc.y<-deg2rad(0.3f)) {Gerr_acc.y=-deg2rad(0.3f);}
	if(Gerr_acc.z>deg2rad(0.3f)) {Gerr_acc.z=deg2rad(0.3f);}
	else if(Gerr_acc.z<-deg2rad(0.3f)) {Gerr_acc.z=-deg2rad(0.3f);}
	ui=Gerr_acc*1000;
	//小范围内，增加P作用
	if(eabs_x<deg2rad(0.3f)) {up.x*=2*sqr(1-eabs_x/deg2rad(0.3f))+1;ui.x/=2;}
	if(eabs_y<deg2rad(0.3f)) {up.y*=2*sqr(1-eabs_y/deg2rad(0.3f))+1;ui.y/=2;}
	if(eabs_z<deg2rad(0.3f)) {up.z*=2*sqr(1-eabs_z/deg2rad(0.3f))+1;ui.z/=2;}
	u= (up+ui+ud)/1000; //控制器输出
/////////////////////////////////////////////////////////
//			磁力计部分
/////////////////////////////////////////////////////////
	//if(tick%10==0 || is_first)
	{
		m_pri=get_m_pri(m_mag); //得到当前罗盘信度
		P=kp*m_pri;
		I=ki*m_pri;

		Vector3D bm;
		bm=cur_ang_m;
		bm.norm();
		cur_quat.rot(bm); //转到东北天大地坐标下
		delta_yaw=rad_range(atan2(bm.y,bm.x)-PI/2);
		mup.z=delta_yaw*P;
		Merr_acc.z+=delta_yaw*I;
		if(is_first)
		{
			mup=mup*3;//若是第一次
		}
		eabs_z=fabs(Merr.z);
		if(Merr.z*Merr_acc.z<0) Merr_acc.z*=0.98f; //大范围内直接削弱
		if(eabs_z>deg2rad(8.0f) || eabs_z<deg2rad(0.2f)) Merr_acc.z-=Merr.z*I;
		if(Merr_acc.z>deg2rad(1.0f)) {Merr_acc.z=deg2rad(1.0f);}
		else if(Merr_acc.z<-deg2rad(1.0f)) {Merr_acc.z=-deg2rad(1.0f);}
		mui.z=Merr_acc.z;
		if(eabs_z<deg2rad(0.5f)) {mup.z*=2*sqr(1-eabs_z/deg2rad(0.5f))+1;mui.z/=2;}
		mu.z=(mup.z+mui.z)/1000.0f;
	}
/////////////////////////////////////////////////////////
//			更新当前姿态
/////////////////////////////////////////////////////////
	QuatVec delta_rot;
	delta_rot.w=1;
	delta_rot.x=(u.x+in_gv.x)/2.0f*T;
	delta_rot.y=(u.y+in_gv.y)/2.0f*T;
	delta_rot.z=(u.z+in_gv.z)/2.0f*T;
	//delta_rot.x=(u.x+mu.x+in_gv.x)/2.0f*T;
	//delta_rot.y=(u.y+mu.y+in_gv.y)/2.0f*T;
	//delta_rot.z=(u.z+mu.z+in_gv.z)/2.0f*T;
	cur_quat=cur_quat*delta_rot;  //融合进最终输出
	cur_quat.norm();
	if(is_use_mag) //是否使用罗盘
	{
		eular=cur_quat.toEuler_zyx();
		eular.z=rad_range(eular.z-mu.z);
		cur_quat.fromEuler_zyx(eular);
	}
}

void CAhrs::imu_pos(float T) //位置解算
{
	Vector3D unit;
	if (is_first)
	{
		return;
	}
	unit=in_av;
	cur_quat.rot(unit);
	unit.z-=1;
	speed=speed+unit*(T*9.8f);
	Vector3D v;
	v.x=((int)(speed.x*10))/10.0f;
	v.y=((int)(speed.y*10))/10.0f;
	v.z=((int)(speed.z*10))/10.0f;
	pos=pos+v*T;
}


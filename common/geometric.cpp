/*
 * 文件名：geometric.cpp
  designed by yz , 2013-9-18
  修改时间：2015年 11月 26日 19:02:19 CST	fb
版本：	V1.1			2019-01-15
	
  功能：
	解析几何库，提供基本计算几何功能。
	使用C++，不使用STL，若有可能，向单片机\DSP移植
	基本功能：
	1、向量基本操作
	2、点、线、面的方程和生成
	3、平移、旋转变换
	4、相交、其他功能
*/

#include"geometric.h"
#include<math.h>

GEO_FLOAT deg_range(GEO_FLOAT d) //角度限制在正负180度
{
	while(d>180)
	{
		d-=360;
	}
	while(d<=-180)
	{
		d+=360;
	}
	return d;
}
GEO_FLOAT rad_range(GEO_FLOAT d) //弧度限制在正负180度
{
	while(d>PI)
	{
		d-=PI*2;
	}
	while(d<=-PI)
	{
		d+=PI*2;
	}
	return d;
}

//2D向量基本运算
#ifndef USECPP11 //4.9.2
Vector2D operator+(Vector2D p1,Vector2D p2) { return Vector2D(p1.x+p2.x,p1.y+p2.y); }
#else
Vector2D operator+(Vector2D &p1,Vector2D &p2)
{
	return Vector2D(p1.x+p2.x,p1.y+p2.y);
}
Vector2D operator+(Vector2D &&p1,Vector2D &&p2) { return p1+p2; }
Vector2D operator+(Vector2D &p1,Vector2D &&p2) { return p1+p2; }
Vector2D operator+(Vector2D &&p1,Vector2D &p2) { return p1+p2; }
#endif

#ifndef USECPP11 //4.9.2
Vector2D operator-(Vector2D p1,Vector2D p2) { return Vector2D(p1.x-p2.x,p1.y-p2.y); }
#else
Vector2D operator-(Vector2D &p1,Vector2D &p2)
{
	return Vector2D(p1.x-p2.x,p1.y-p2.y);
}
Vector2D operator-(Vector2D &&p1,Vector2D &&p2) { return p1-p2;}
Vector2D operator-(Vector2D &p1,Vector2D &&p2) { return p1-p2;}
Vector2D operator-(Vector2D &&p1,Vector2D &p2) { return p1-p2;}
#endif

#ifndef USECPP11 //4.9.2
GEO_FLOAT operator*(Vector2D p1,Vector2D p2) { return p1.x*p2.x+p1.y*p2.y; } //点乘
#else
GEO_FLOAT operator*(Vector2D &p1,Vector2D &p2) //点乘
{
	return p1.x*p2.x+p1.y*p2.y;
}
GEO_FLOAT operator*(Vector2D &&p1,Vector2D &&p2) { return p1*p2;}
GEO_FLOAT operator*(Vector2D &p1,Vector2D &&p2) { return p1*p2;}
GEO_FLOAT operator*(Vector2D &&p1,Vector2D &p2) { return p1*p2;}
#endif

Vector2D operator+(Vector2D &p1,GEO_FLOAT k)
{
	return Vector2D(p1.x+k,p1.y+k);
}
Vector2D operator-(Vector2D &p1,GEO_FLOAT k)
{
	return Vector2D(p1.x-k,p1.y-k);
}
Vector2D operator*(Vector2D &p1,GEO_FLOAT k)
{
	return Vector2D(p1.x*k,p1.y*k);
}
Vector2D operator/(Vector2D &p1,GEO_FLOAT k)
{
	return Vector2D(p1.x/k,p1.y/k);
}

#ifdef USECPP11 //4.9.2以上
Vector2D operator+(Vector2D &&p1,GEO_FLOAT k) { return p1+k; }
Vector2D operator-(Vector2D &&p1,GEO_FLOAT k) { return p1-k; }
Vector2D operator*(Vector2D &&p1,GEO_FLOAT k) { return p1*k; }
Vector2D operator/(Vector2D &&p1,GEO_FLOAT k) { return p1/k; }
#endif

GEO_FLOAT Vector2D::magnitude(void) //2D向量幅度
{
	return sqrt(x*x+y*y);
}
//判Eps，失败时不动作，需要外部判断返回的幅度
GEO_FLOAT Vector2D::norm(void) //2D向量归一化
{
	GEO_FLOAT mag=magnitude();
	if (fabs(mag)>Eps)
	{
		x/=mag;
		y/=mag;
	}
	return mag;
}
GEO_FLOAT Vector2D::distance(Vector2D &p1) //2D向量距离
{
	Vector2D p=p1-*this;
	return p.norm();
}
//调用方判断，若有除0，则导致结果NAN
GEO_FLOAT Vector2D::angle(Vector2D &p1) //两向量夹角
{
	return acos((p1.x*x+p1.y*y)/sqrt(p1.x*p1.x+p1.y*p1.y)/sqrt(x*x+y*y));
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Vector2D::distance(Vector2D &&p1) { return distance(p1);}
GEO_FLOAT Vector2D::angle(Vector2D &&p1) { return angle(p1); }
#endif

//3D向量基本运算
#ifndef USECPP11 //4.9.2
Vector3D operator+(Vector3D p1,Vector3D p2) { return Vector3D(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z); }
Vector3D operator-(Vector3D p1,Vector3D p2) { return Vector3D(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z); }
GEO_FLOAT operator*(Vector3D p1,Vector3D p2) { return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z; } //点乘
Vector3D operator^(Vector3D p1,Vector3D p2)
{
	Vector3D p=Vector3DEmpty;
	p.x=p1.y*p2.z-p1.z*p2.y;
	p.y=p1.z*p2.x-p1.x*p2.z;
	p.z=p1.x*p2.y-p1.y*p2.x;
	return p;
}
Vector3D operator+(Vector3D p1,GEO_FLOAT k)
{
	return Vector3D(p1.x+k,p1.y+k,p1.z+k);
}
Vector3D operator-(Vector3D p1,GEO_FLOAT k)
{
	return Vector3D(p1.x-k,p1.y-k,p1.z-k);
}
Vector3D operator*(Vector3D p1,GEO_FLOAT k)
{
	return Vector3D(p1.x*k,p1.y*k,p1.z*k);
}
Vector3D operator/(Vector3D p1,GEO_FLOAT k)
{
	return Vector3D(p1.x/k,p1.y/k,p1.z/k);
}
#else
Vector3D operator+(Vector3D &p1,Vector3D &p2)
{
	return Vector3D(p1.x+p2.x,p1.y+p2.y,p1.z+p2.z);
}
Vector3D operator-(Vector3D &p1,Vector3D &p2)
{
	return Vector3D(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z);
}
GEO_FLOAT operator*(Vector3D &p1,Vector3D &p2) //点乘
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}
Vector3D operator^(Vector3D &p1,Vector3D &p2) //叉乘
{
	Vector3D p=Vector3DEmpty;
	p.x=p1.y*p2.z-p1.z*p2.y;
	p.y=p1.z*p2.x-p1.x*p2.z;
	p.z=p1.x*p2.y-p1.y*p2.x;
	return p;
}
Vector3D operator+(Vector3D &&p1,Vector3D &&p2) { return p1+p2; }
Vector3D operator+(Vector3D &p1,Vector3D &&p2) { return p1+p2; }
Vector3D operator+(Vector3D &&p1,Vector3D &p2) { return p1+p2; }
Vector3D operator-(Vector3D &&p1,Vector3D &&p2) { return p1-p2; }
Vector3D operator-(Vector3D &p1,Vector3D &&p2) { return p1-p2; }
Vector3D operator-(Vector3D &&p1,Vector3D &p2) { return p1-p2; }
GEO_FLOAT operator*(Vector3D &&p1,Vector3D &&p2) { return p1*p2; }
GEO_FLOAT operator*(Vector3D &p1,Vector3D &&p2) { return p1*p2; }
GEO_FLOAT operator*(Vector3D &&p1,Vector3D &p2) { return p1*p2; }
Vector3D operator^(Vector3D &&p1,Vector3D &&p2) { return p1^p2; }
Vector3D operator^(Vector3D &p1,Vector3D &&p2) { return p1^p2; }
Vector3D operator^(Vector3D &&p1,Vector3D &p2) { return p1^p2; }
Vector3D operator+(Vector3D &p1,GEO_FLOAT k)
{
	return Vector3D(p1.x+k,p1.y+k,p1.z+k);
}
Vector3D operator-(Vector3D &p1,GEO_FLOAT k)
{
	return Vector3D(p1.x-k,p1.y-k,p1.z-k);
}
Vector3D operator*(Vector3D &p1,GEO_FLOAT k)
{
	return Vector3D(p1.x*k,p1.y*k,p1.z*k);
}
Vector3D operator/(Vector3D &p1,GEO_FLOAT k)
{
	return Vector3D(p1.x/k,p1.y/k,p1.z/k);
}
Vector3D operator+(Vector3D &&p1,GEO_FLOAT k) { return p1+k; }
Vector3D operator-(Vector3D &&p1,GEO_FLOAT k) { return p1-k; }
Vector3D operator*(Vector3D &&p1,GEO_FLOAT k) { return p1*k; }
Vector3D operator/(Vector3D &&p1,GEO_FLOAT k) { return p1/k; }
#endif

GEO_FLOAT Vector3D::magnitude(void) //3D向量幅度
{
	return sqrt(x*x+y*y+z*z);
}
//判Eps，失败时不动作，需要外部判断返回的幅度
GEO_FLOAT Vector3D::norm(void) //3D向量归一化
{
	GEO_FLOAT mag=magnitude();
	if (fabs(mag)>Eps)
	{
		x/=mag;
		y/=mag;
		z/=mag;
	}
	return mag;
}
GEO_FLOAT Vector3D::distance(Vector3D &p1) //3D向量距离
{
	Vector3D p=p1-*this;
	return p.norm();
}
//调用方判断，若有除0，则导致结果NAN
GEO_FLOAT Vector3D::angle(Vector3D &p1) //两向量夹角
{
	return acos((p1.x*x+p1.y*y+p1.z*z)/sqrt(p1.x*p1.x+p1.y*p1.y+p1.z*p1.z)
		/sqrt(x*x+y*y+z*z));
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Vector3D::distance(Vector3D &&p1) { return distance(p1); }
GEO_FLOAT Vector3D::angle(Vector3D &&p1) { return angle(p1); }
#endif
///////////////////////////////////////////////////////////////
void Line2D::create(Vector2D &p1,Vector2D &p2) //两点生成线
{
	k=p2-p1;
	p=p1;
}
#ifdef USECPP11 //4.9.2以上
void Line2D::create(Vector2D &&p1,Vector2D &&p2) { create(p1,p2); }
void Line2D::create(Vector2D &p1,Vector2D &&p2)  { create(p1,p2); }
void Line2D::create(Vector2D &&p1,Vector2D &p2)  { create(p1,p2); }
#endif

Vector2D Line2D::vertical(void) //求此直线的垂线向量
{
	Vector2D vec;
	vec.x=-k.y;
	vec.y=k.x;
	return vec;
}
Line2D Line2D::vertical(Vector2D &p1) //求此直线的垂线（过已知点）
{
	Line2D ret;
	ret.k=vertical();
	ret.p=p1;
	return ret;
}
#ifdef USECPP11 //4.9.2以上
Line2D Line2D::vertical(Vector2D &&p1) { return vertical(p1); }
#endif

bool Line2D::cross(Line2D &l,Vector2D &p1) //求两直线的交点，false无交点
{
	GEO_FLOAT t=0; //以解方程的方法来求
	t=(k.x*l.k.y - k.y*l.k.x); //t1的系数,本身为1
	if(fabs(t)<Eps)
	{
		return false;
	}
	t=(l.k.y*(l.p.x-p.x)+l.k.x*(p.y-l.p.y))/t;
	p1.x=k.x*t+p.x;
	p1.y=k.y*t+p.y;
	return true;
}
#ifdef USECPP11 //4.9.2以上
bool Line2D::cross(Line2D &&l,Vector2D &p1)  { return cross(l,p1); }
#endif

Vector2D Line2D::Cal_P_Dis(Vector2D &p1,GEO_FLOAT d) //在直线上，距离给定点一定距离的点。（距离有正负之分）
{
	GEO_FLOAT t;
	t=k.magnitude();
	if(t<Eps) { return p1; } //若不符合条件，就返回那个点本身
	t=d/t; //然后得到参数
	return k*t+p1;//认为给定点严格在直线上
}
#ifdef USECPP11 //4.9.2以上
Vector2D Line2D::Cal_P_Dis(Vector2D &&p1,GEO_FLOAT d) { return Cal_P_Dis(p1,d); }
#endif

Vector2D Line2D::vert_point(Vector2D &p1) //点到直线的垂点
{
	Line2D lv;
	lv=vertical(p1); //求垂线
	Vector2D p;
	bool r=cross(lv,p); //求交点
	if(r==false) { return p1; } //若没有交点，说明交点就是自己
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector2D Line2D::vert_point(Vector2D &&p1) { return vert_point(p1); }
#endif

GEO_FLOAT Line2D::distance(Vector2D &p1) //点到直线距离
{
	Vector2D p2=vert_point(p1); //求垂点
	return p2.distance(p1);
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Line2D::distance(Vector2D &&p1) { return distance(p1); }
#endif

////////////////////////////////////////////////////////////
void Line3D::create(Vector3D &p1,Vector3D &p2) //两点生成线
{
	k=p2-p1;
	p=p1;
}
#ifdef USECPP11 //4.9.2以上
void Line3D::create(Vector3D &&p1,Vector3D &&p2) { create(p1,p2); }
void Line3D::create(Vector3D &p1,Vector3D &&p2) { create(p1,p2); }
void Line3D::create(Vector3D &&p1,Vector3D &p2) { create(p1,p2); }
#endif

Line3D Line3D::vertical(Vector3D &p1) //求此直线的垂线（过已知点）
{
	Vector3D tmp;
	tmp=vert_point(p1); //求垂点
	Line3D l;
	l.create(tmp,p1);
	return l;
}
#ifdef USECPP11 //4.9.2以上
Line3D Line3D::vertical(Vector3D &&p1) { return vertical(p1); }
#endif

bool Line3D::cross(Line3D &l,Vector3D &p1) //求两直线的交点(相距最近的点)
{
	//首先求公垂线
	//列方程：K1*t1+B1 - K2*t2-B2 = Kv*tv
	//t1 t2 为最短垂线段的两个端点,K3为公垂线向量
	//方程：[k1 -k2 -kv]*[t1;t2;tv]=[b2-b1]
	//[t1;t2;tv]=[k1 -k2 -kv]^-1*[b2-b1]
	Vector3D kv;
	kv=k^l.k; //求垂线
	GEO_FLOAT d=kv.norm(); //归一化
	if (fabs(d)<Eps) //若两线平行
	{
		return false;
	}
	//解方程，一定有解，d非0
	d=	k.x*(l.k.y*kv.z - l.k.z*kv.y) - 
		k.y*(l.k.x*kv.z - l.k.z*kv.x) + 
		k.z*(l.k.x*kv.y - l.k.y*kv.x); //行列式值
	GEO_FLOAT bx=l.p.x-p.x;
	GEO_FLOAT by=l.p.y-p.y;
	GEO_FLOAT bz=l.p.z-p.z; //B2-B1
	GEO_FLOAT t1=	(l.k.y*kv.z-l.k.z*kv.y)*bx +
				(l.k.z*kv.x-l.k.x*kv.z)*by +
				(l.k.x*kv.y-l.k.y*kv.x)*bz;
	GEO_FLOAT t2=	(k.y*kv.z-k.z*kv.y)*bx +
				(k.z*kv.x-k.x*kv.z)*by +
				(k.x*kv.y-k.y*kv.x)*bz;
	t1/=d;
	t2/=d;
	p1=(k*t1+p + l.k*t2+l.p)/2; //求线段中点
	return true; //算法需要求方程,以后实现
}
#ifdef USECPP11 //4.9.2以上
bool Line3D::cross(Line3D &&l,Vector3D &p1) { return cross(l,p1); }
#endif

//此函数要求调用方保证不是平行线
GEO_FLOAT Line3D::line_distance(Line3D &l) //求两直线间距
{
	Vector3D kv;
	kv=k^l.k; //求垂线
	kv.norm(); //归一化
	//此方法无法求平行线
	return kv*(l.p-p); //求参数
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Line3D::line_distance(Line3D &&l) { return line_distance(l); }
#endif

Vector3D Line3D::Cal_P_Dis(Vector3D &p1,GEO_FLOAT d) //在直线上，距离给定点一定距离的点。（距离有正负之分）
{
	GEO_FLOAT t;
	t=k.magnitude();
	if(t<Eps) { return p1; } //若不符合条件，就返回那个点本身
	t=d/t; //然后得到参数
	return k*t+p1;//认为给定点严格在直线上
}
#ifdef USECPP11 //4.9.2以上
Vector3D Line3D::Cal_P_Dis(Vector3D &&p1,GEO_FLOAT d) { return Cal_P_Dis(p1,d); }
#endif

Vector3D Line3D::vert_point(Vector3D &p1) //点到直线的垂点
{
	Surface sf;
	sf.create_vert(*this,p1); //求垂面
	Vector3D p;
	sf.cross(*this,p); //求线面交点
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector3D Line3D::vert_point(Vector3D &&p1) { return vert_point(p1); }
#endif

GEO_FLOAT Line3D::distance(Vector3D &p1) //点到直线距离
{
	Vector3D tmp;
	tmp=vert_point(p1);
	return p1.distance(tmp);
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Line3D::distance(Vector3D &&p1) { return distance(p1); }
#endif

///////////////////////////////////////////////////////////
void Surface::create(Line3D &l,Vector3D &p1) //点线确定面
{
	k=l.k^(p1-l.p); //叉乘取垂线
	b=k*p1;
}
#ifdef USECPP11 //4.9.2以上
void Surface::create(Line3D &&l,Vector3D &&p1) { create(l,p1); }
void Surface::create(Line3D &l,Vector3D &&p1)  { create(l,p1); }
void Surface::create(Line3D &&l,Vector3D &p1)  { create(l,p1); }
#endif

void Surface::create_vert(Line3D &l,Vector3D &p1) //点线确定面,线的垂面
{
	k=l.k;
	b=k*p1;
}
#ifdef USECPP11 //4.9.2以上
void Surface::create_vert(Line3D &&l,Vector3D &&p1) { create_vert(l,p1); }
void Surface::create_vert(Line3D &l,Vector3D &&p1) { create_vert(l,p1); }
void Surface::create_vert(Line3D &&l,Vector3D &p1) { create_vert(l,p1); }
#endif

bool Surface::cross(Line3D &l,Vector3D &p1) //线面交点，返回是否有交点
{
	//面方程：A·X=d
	//线方程：X=Kt+P
	//将X代入，A·Kt+A·P=b，求得t，进而求得X
	GEO_FLOAT AK=k*l.k;
	if (fabs(AK)<Eps)
	{
		return false;
	}
	GEO_FLOAT t=(b-k*l.p)/AK;
	p1=l.k*t+l.p;
	return true;
}
#ifdef USECPP11 //4.9.2以上
bool Surface::cross(Line3D &&l,Vector3D &p1) { return cross(l,p1); }
#endif

Vector3D Surface::vert_point(Vector3D &p1) //点面垂点
{
	Line3D l;
	l.k=k;
	l.p=p1;
	Vector3D p;
	bool r=cross(l,p);//垂线与平面的交点
	if(r==false) { return p1; } //若没有交点，说明交点就是自己
	return p;
}
#ifdef USECPP11 //4.9.2以上
Vector3D Surface::vert_point(Vector3D &&p1) { return vert_point(p1); }
#endif

GEO_FLOAT Surface::distance(Vector3D &p1) //点面距
{
	Vector3D tmp;
	tmp=vert_point(p1);//求垂点
	return tmp.distance(p1);
}
#ifdef USECPP11 //4.9.2以上
GEO_FLOAT Surface::distance(Vector3D &&p1) { return distance(p1); }
#endif

///////////////////////////////////////////////////////////
//有向线段
Vector2D Segment2D::proj_point(Vector2D &p1) //点到线段距离最短的点（点到线段距离）
{//线段AB以外P点到AB最短距离的点为C
	Vector2D unit_ab=end-st; //AB单位向量
	GEO_FLOAT val_ab=unit_ab.norm(); //norm除0时不产生nan，后续运算正常
	GEO_FLOAT val_ac=(p1-st)*unit_ab; //AP * (AB/abs(AB))
	if(val_ac>val_ab) return end; //投影点在线段终点B以外
	if(val_ac<0) return st; //投影点在线段终点B以外
	return unit_ab*val_ac+st;
}
#ifdef USECPP11 //4.9.2以上
Vector2D Segment2D::proj_point(Vector2D &&p1) { return proj_point(p1); } //点到线段距离最短的点（点到线段距离）
#endif

Vector3D Segment3D::proj_point(Vector3D &p1) //点到线段距离最短的点（点到线段距离）
{//线段AB以外P点到AB最短距离的点为C
	Vector3D unit_ab=end-st; //AB单位向量
	GEO_FLOAT val_ab=unit_ab.norm(); //norm除0时不产生nan，后续运算正常
	GEO_FLOAT val_ac=(p1-st)*unit_ab; //AP * (AB/abs(AB))
	if(val_ac>val_ab) return end; //投影点在线段终点B以外
	if(val_ac<0) return st; //投影点在线段终点B以外
	return unit_ab*val_ac+st;
}
#ifdef USECPP11
Vector3D Segment3D::proj_point(Vector3D &&p1) { return proj_point(p1); } //点到线段距离最短的点（点到线段距离）
#endif
///////////////////////////////////////////////////////////
//2D矩阵变换
Vector2D matrix_trans2D(Vector2D &k,GEO_FLOAT t[][3])
{
	int i,s;
	Vector2D ret;
	GEO_FLOAT m[3] = {k.x,k.y,1};
	GEO_FLOAT r[3] = {0,0,0};
	for(i=0;i<3;i++)
	{
		for(s=0;s<3;s++)
		{
			r[i] = m[s] * t[s][i] + r[i];
		}
	}
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//3D矩阵变换
Vector3D matrix_trans3D(Vector3D &k,GEO_FLOAT t[][4])
{
	int i,s;
	Vector3D ret;
	GEO_FLOAT m[4] = {k.x,k.y,k.z,1};
	GEO_FLOAT r[4] = {0,0,0,0};
	for(i=0;i<4;i++)
	{
		for(s=0;s<4;s++)
		{
			r[i] = m[s] * t[s][i] + r[i];
		}
	}
	ret.x = r[0];
	ret.y = r[1];
	ret.z = r[2];
	return ret;
}

//2D向量平移变换
Vector2D translate2D(Vector2D &k,GEO_FLOAT tx,GEO_FLOAT ty)
{
	int i,s;
	Vector2D ret;
	GEO_FLOAT m[3] = {k.x,k.y,1};
	GEO_FLOAT t[3][3] = {{1,0,0},{0,1,0},{tx,ty,1}};
	GEO_FLOAT r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于(0,0)比例变换
Vector2D scale2D(Vector2D &k,GEO_FLOAT a,GEO_FLOAT d)
{
	int i,s;
	Vector2D ret;
	GEO_FLOAT m[3] = {k.x,k.y,1};
	GEO_FLOAT t[3][3] = {{a,0,0},{0,d,0},{0,0,1}};
	GEO_FLOAT r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于C(cx,cy)比例变换
Vector2D scaleToVector2D(Vector2D &k,Vector2D &c,GEO_FLOAT a,GEO_FLOAT d)
{
	int i,s;
	Vector2D ret;
	GEO_FLOAT m[3] = {k.x,k.y,1};
	GEO_FLOAT t[3][3] = {{a,0,0},{0,d,0},{c.x * (1-a),c.y * (1-d),1}};
	GEO_FLOAT r[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			r[i] = m[s] * t[s][i] + r[i];
	ret.x = r[0];
	ret.y = r[1];
	return ret;
}

//2D相对于(0,0)旋转变换
Vector2D rotate2D(Vector2D &k,GEO_FLOAT r) //弧度
{
	return Vector2D( k.x*cos(r)-k.y*sin(r), k.x*sin(r)+k.y*cos(r));
}
#ifdef USECPP11 //4.9.2以上
Vector2D rotate2D(Vector2D &&k,GEO_FLOAT r) { return rotate2D(k,r); }
#endif

//2D相对于C(cx,cy)旋转变换
Vector2D rotateToVector2D(Vector2D &k,Vector2D &c,GEO_FLOAT r)
{
	int i,s;
	Vector2D ret;
	GEO_FLOAT m[3] = {k.x,k.y,1};
	GEO_FLOAT t[3][3] = {{cos(r),sin(r),0},{-sin(r),cos(r),0},{c.x - c.x * cos(r) + c.y * sin(r),c.y - c.x * sin(r) - c.y * cos(r),1}};
	GEO_FLOAT n[3] = {0,0,0};
	for(i=0;i<3;i++)
		for(s=0;s<3;s++)
			n[i] = m[s] * t[s][i] + n[i];
	ret.x = n[0];
	ret.y = n[1];
	return ret;
}

//////////////////////////////////////////////////////////////////////////
//3D旋转:输入点和欧拉角单个角
Vector3D rotate3D_euler_x(Vector3D &p,GEO_FLOAT ang)//弧度制
{
	GEO_FLOAT sa=sin(ang);
	GEO_FLOAT ca=cos(ang);
	return Vector3D(
		p.x,
		p.y*ca - p.z*sa,
		p.y*sa + p.z*ca
	);
}
Vector3D rotate3D_euler_y(Vector3D &p,GEO_FLOAT ang)//弧度制
{
	GEO_FLOAT sa=sin(ang);
	GEO_FLOAT ca=cos(ang);
	return Vector3D(
		p.x*ca + p.z*sa,
		p.y,
		-p.x*sa + p.z*ca
	);
}
Vector3D rotate3D_euler_z(Vector3D &p,GEO_FLOAT ang)//弧度制
{
	GEO_FLOAT sa=sin(ang);
	GEO_FLOAT ca=cos(ang);
	return Vector3D(
		p.x*ca - p.y*sa,
		p.x*sa + p.y*ca,
		p.z
	);
}
//3D旋转:z-x-y内旋 输入点和欧拉角
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &euler)//弧度制
{
	GEO_FLOAT sx=sin(euler.x);
	GEO_FLOAT sy=sin(euler.y);
	GEO_FLOAT sz=sin(euler.z);
	GEO_FLOAT cx=cos(euler.x);
	GEO_FLOAT cy=cos(euler.y);
	GEO_FLOAT cz=cos(euler.z);
	return Vector3D(
		p.x*(cz*cy-sz*sx*sy	)+p.y*(-sz*cx	)+p.z*(cz*sy+sz*sx*cy	),
		p.x*(sz*cy+cz*sx*sy	)+p.y*(cx*cz	)+p.z*(sz*sy-cz*sx*cy	),
		p.x*(-cx*sy			)+p.y*(sx		)+p.z*(cy*cx	)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_zxy(p,euler); }
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_zxy(p,euler); }
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_zxy(p,euler); }
#endif

//3D旋转:z-y-x内旋(航空欧拉角) 输入点和欧拉角
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &euler)//弧度制
{
	GEO_FLOAT sx=sin(euler.x);
	GEO_FLOAT sy=sin(euler.y);
	GEO_FLOAT sz=sin(euler.z);
	GEO_FLOAT cx=cos(euler.x);
	GEO_FLOAT cy=cos(euler.y);
	GEO_FLOAT cz=cos(euler.z);
	return Vector3D(
		p.x*(cz*cy	)+p.y*(-sz*cx+cz*sy*sx	)+p.z*(sz*sx+cz*sy*cx	),
		p.x*(sz*cy	)+p.y*(cx*cz+sz*sy*sx	)+p.z*(-cz*sx+sz*sy*cx	),
		p.x*(-sy	)+p.y*(cy*sx			)+p.z*(cy*cx			)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_zyx(p,euler); }
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_zyx(p,euler); }
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_zyx(p,euler); }
#endif

//3D旋转:x-y-z内旋 输入点和欧拉角
Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &euler)//弧度制
{
	GEO_FLOAT sx=sin(euler.x);
	GEO_FLOAT sy=sin(euler.y);
	GEO_FLOAT sz=sin(euler.z);
	GEO_FLOAT cx=cos(euler.x);
	GEO_FLOAT cy=cos(euler.y);
	GEO_FLOAT cz=cos(euler.z);
	return Vector3D(
		p.x*(cy*cz+sx*sy*sz )+p.y*(cz*sy*sx-cy*sz)+p.z*(cx*sy),
		p.x*(cx*sz			)+p.y*(cx*cz		 )+p.z*(-sx	 ),
		p.x*(cy*sx*sz-cz*sy )+p.y*(cy*cz*sx+sy*sz)+p.z*(cx*cy)
	);
}
#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &&euler) { return rotate3D_euler_yxz(p,euler); }
Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &&euler) { return rotate3D_euler_yxz(p,euler); }
Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &euler) { return rotate3D_euler_yxz(p,euler); }
#endif

//四元数的归一化，判Eps，失败时不动作，需要外部判断返回的幅度
GEO_FLOAT QuatVec::norm(void)
{
	GEO_FLOAT s=sqrt(w*w + x*x + y*y + z*z);
	if (fabs(s)>Eps)
	{
		w/=s; x/=s; y/=s; z/=s;
	}
	return s;
}
QuatVec QuatVec::inv(void) //逆，就是共轭，认为自己是单位四元数
{
	QuatVec q;
	q.w=w; q.x=-x; q.y=-y; q.z=-z;
	return q;
}
//四元数的乘法
QuatVec operator*(QuatVec &q1,QuatVec &q2) //乘号右侧的先转，恰好与旋转矩阵一样
{
	QuatVec q;
	q.w=q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	q.x=q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	q.y=q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	q.z=q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	q.norm();
	return q;
}
#ifdef USECPP11 //4.9.2以上
QuatVec operator*(QuatVec &&q1,QuatVec &&q2) { return q1*q2; }
QuatVec operator*(QuatVec &q1,QuatVec &&q2) { return q1*q2; }
QuatVec operator*(QuatVec &&q1,QuatVec &q2) { return q1*q2; }
#endif

//四元数旋转,原位操作
void QuatVec::rot(Vector3D &p)
{
	GEO_FLOAT x2 = x * x;
	GEO_FLOAT y2 = y * y;
	GEO_FLOAT z2 = z * z;
	GEO_FLOAT xy = x * y;
	GEO_FLOAT xz = x * z;
	GEO_FLOAT yz = y * z;
	GEO_FLOAT wx = w * x;
	GEO_FLOAT wy = w * y;
	GEO_FLOAT wz = w * z;
	GEO_FLOAT px=(1-2*(y2+z2)) * p.x + 2*(xy-wz)     * p.y + 2*(xz+wy)     * p.z;
	GEO_FLOAT py=2*(xy+wz)     * p.x + (1-2*(x2+z2)) * p.y + 2*(yz-wx)     * p.z;
	GEO_FLOAT pz=2*(xz-wy)     * p.x + 2*(yz+wx)     * p.y + (1-2*(x2+y2)) * p.z;
	p.x=px;
	p.y=py;
	p.z=pz;
}
Vector3D QuatVec::toEuler_zyx(void) //转换为欧拉角(内旋，航空欧拉角)
{
	Vector3D p;
	p.x = atan2(2 * (w * x + z * y) , 1 - 2 * (y * y + x * x));
	p.y = asin (2 * (w * y - x * z));
	p.z = atan2(2 * (w * z + y * x) , 1 - 2 * (y * y + z * z));
	return p;
}
Vector3D QuatVec::toEuler_zxy(void) //转换为欧拉角，内旋
{
	Vector3D p;
	p.x = asin(2 * (w * x + y * z));
	p.y = -atan2(2 * (x * z - w * y) , 1 - 2 * (y * y + x * x));
	p.z = -atan2(2 * (x * y - w * z) , 1 - 2 * (x * x + z * z));
	return p;
}
Vector3D QuatVec::toEuler_yxz(void) //转换为欧拉角，内旋
{
	Vector3D p;
	p.x = -asin(2 * (y * z - w * x));
	p.y = atan2(2 * (x * z + w * y) , 1 - 2 * (y * y + x * x));
	p.z = atan2(2 * (x * y + w * z) , 1 - 2 * (x * x + z * z));
	return p;
}
Vector3D QuatVec::toEuler_xzy() //转换为欧拉角内旋
{
	Vector3D p;
	p.x = atan2(2 * (y * z + w * x), 1 - 2 * (z * z + x * x));
	p.y = atan2(2 * (x * z + w * y), 1 - 2 * (y * y + z * z));
	p.z = -asin(2 * (x * y - w * z));
	return p;
}
Vector3D QuatVec::toEuler_yzx() //转换为欧拉角内旋
{
	Vector3D p;
	p.x = -atan2(2 * (y * z - w * x), 1 - 2 * (z * z + x * x));
	p.y = -atan2(2 * (x * z - w * y), 1 - 2 * (y * y + z * z));
	p.z = asin(2 * (x * y + w * z));
	return p;
}
void QuatVec::fromEuler_zyx(Vector3D &p) //欧拉角转四元数内旋(航空欧拉角)
{
	GEO_FLOAT cx = cos(p.x/2);
	GEO_FLOAT sx = sin(p.x/2);
	GEO_FLOAT cy = cos(p.y/2);
	GEO_FLOAT sy = sin(p.y/2);
	GEO_FLOAT cz = cos(p.z/2);
	GEO_FLOAT sz = sin(p.z/2);
	w = cx * cy * cz + sx * sy * sz;
	x = sx * cy * cz - cx * sy * sz;
	y = cx * sy * cz + sx * cy * sz;
	z = cx * cy * sz - sx * sy * cz;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_zyx(Vector3D &&p) { fromEuler_zyx(p); }
#endif
void QuatVec::fromEuler_zxy(Vector3D &p) //欧拉角转四元数内旋
{
	GEO_FLOAT cx = cos(p.x/2);
	GEO_FLOAT sx = sin(p.x/2);
	GEO_FLOAT cy = cos(p.y/2);
	GEO_FLOAT sy = sin(p.y/2);
	GEO_FLOAT cz = cos(p.z/2);
	GEO_FLOAT sz = sin(p.z/2);
	w = cz * cy * cx - sz * sy * sx;
	x = cz * sy * cx - sz * cy * sx;
	y = cz * cy * sx + sz * sy * cx;
	z = cz * sy * sx + sz * cy * cx;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_zxy(Vector3D &&p) { fromEuler_zxy(p); }
#endif
void QuatVec::fromEuler_yxz(Vector3D &p) //欧拉角转四元数内旋
{
	GEO_FLOAT cx = cos(p.x/2);
	GEO_FLOAT sx = sin(p.x/2);
	GEO_FLOAT cy = cos(p.y/2);
	GEO_FLOAT sy = sin(p.y/2);
	GEO_FLOAT cz = cos(p.z/2);
	GEO_FLOAT sz = sin(p.z/2);
	w = cy * cx * cz + sy * sx * sz;
	x = cy * sx * cz + sy * cx * sz;
	y = sy * cx * cz - cy * sx * sz;
	z = cy * cx * sz - sy * sx * cz;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_yxz(Vector3D &&p) { fromEuler_yxz(p); }
#endif
void QuatVec::fromEuler_xzy(Vector3D &p) //欧拉角转四元数内旋
{
	double cx = cos(p.x / 2);
	double sx = sin(p.x / 2);
	double cy = cos(p.y / 2);
	double sy = sin(p.y / 2);
	double cz = cos(p.z / 2);
	double sz = sin(p.z / 2);
	w = cx * cz * cy + sx * sz * sy;
	x = cy * sx * cz - sy * cx * sz;
	y = sy * cx * cz - cy * sx * sz;
	z = cy * cx * sz + sy * sx * cz;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_xzy(Vector3D &&p) { fromEuler_yxz(p); }
#endif
void QuatVec::fromEuler_yzx(Vector3D &p) //欧拉角转四元数内旋
{
	double cx = cos(p.x / 2);
	double sx = sin(p.x / 2);
	double cy = cos(p.y / 2);
	double sy = sin(p.y / 2);
	double cz = cos(p.z / 2);
	double sz = sin(p.z / 2);
	w = cx * cz * cy - sx * sz * sy;
	x = cy * sx * cz + sy * cx * sz;
	y = sy * cx * cz + cy * sx * sz;
	z = cy * cx * sz - sy * sx * cz;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromEuler_yzx(Vector3D &&p) { fromEuler_yxz(p); }
#endif

void QuatVec::fromAxis(Vector3D &axis,GEO_FLOAT angle) //从转轴和转角构造
{
	angle/=2;
	w=cos(angle);
	x=sin(angle)*axis.x;
	y=sin(angle)*axis.y;
	z=sin(angle)*axis.z;
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromAxis(Vector3D &&axis,GEO_FLOAT angle) { fromAxis(axis,angle); }
#endif

void QuatVec::fromVector(Vector3D &v) //从向量构造，不完全约束，按直接转过去
{
	Vector3D u=Vector3D(1,0,0);
	GEO_FLOAT vm=v.magnitude();
	if(vm < Eps) return ; //若除零，不动作
	u=u^v; //得到直接旋转的轴
	GEO_FLOAT a=u.norm()/vm; //化为sin值，u为0没关系
	a=asin(a);
	fromAxis(u,a);
}
#ifdef USECPP11 //4.9.2以上
void QuatVec::fromVector(Vector3D &&v) { fromVector(v); }
#endif

//换参照系：已知两个姿态的大地坐标，求在A为参照系下B的姿态
QuatVec change_coord(QuatVec &u,QuatVec &coord) //输入载荷姿态、载体姿态（参照系）
{
	QuatVec dq; //载荷相对载体的姿态
	dq=coord.inv()*u; //乘号右侧的先转，就是载荷姿态本身，向载体姿态反向旋转
	return dq;
}

////////////////////////////////////////////////////////////////
//地理算法
////////////////////////////////////////////////////////////////
//局部直角坐标转经纬度，高程不动。输入中央经纬线
Vector3D localxy_2_blh(Vector3D p,Vector3D &cen) //要求外部保证不除0，判断纬度范围
{
	p.x=xy2wgs(p.x);
	p.y=xy2wgs(p.y) + cen.y;
	p.x=cen.x + p.x/cos(deg2rad(p.y)); //桑逊投影
	return p;
}
//经纬度转局部直角坐标，高程不动。输入中央经纬线
Vector3D blh_2_localxy(Vector3D p,Vector3D &cen)
{
	p.x-=cen.x;
	p.x=wgs2xy(p.x)*cos(deg2rad(p.y)); //桑逊投影
	p.y-=cen.y;
	p.y=wgs2xy(p.y);
	return p;
}
double EarthDistance(double lng1, double lat1, double lng2, double lat2) //地球两点距离
{
	double radLat1 = deg2rad(lat1);
	double radLat2 = deg2rad(lat2);
	double a = radLat1 - radLat2;
	double b = deg2rad(lng1) - deg2rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +
		cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
	s = s * EARTH_R;
	return s;
}
double EarthDistance(Vector2D p0, Vector2D p1) //地球两点距离
{
	return EarthDistance(p0.x, p0.y, p1.x, p1.y);
}
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz_ellipse(Vector3D blh) //椭圆法，输入经纬度高程单位：度、米
{
	//首先转为弧度
	blh.x=deg2rad(blh.x); //L:经度
	blh.y=deg2rad(blh.y); //B:纬度
	const double a2=EARTH_R*EARTH_R; //地球半径平方
	const double b2=EARTH_R_SHORT*EARTH_R_SHORT; //地球短半轴平方
	const double e2=(a2-b2)/a2; //椭球第一偏心率的平方
	double sinB=sin(blh.y); 
	double w=sqrt(1-(e2*sinB*sinB));
	double n=EARTH_R/w;
	Vector3D tmp; //直角坐标
	tmp.x=(n+blh.z)*cos(blh.y)*cos(blh.x);
	tmp.y=(n+blh.z)*cos(blh.y)*sin(blh.x);
	tmp.z=(n*(1-e2)+blh.z)*sinB;
	return tmp;
}
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh_ellipse(Vector3D xyz) //椭圆法，输出经纬度高程单位：度、米
{
	double x2=xyz.x*xyz.x;
	double y2=xyz.y*xyz.y;
	const double a2=EARTH_R*EARTH_R; //地球半径平方
	const double b2=EARTH_R_SHORT*EARTH_R_SHORT; //地球短半轴平方
	const double e2=(a2-b2)/a2; //椭球第一偏心率的平方
	const double e12=(a2-b2)/b2; //椭球第二偏心率的平方
	double sqrt_x2_y2=sqrt(x2+y2);
	double theata=atan2(EARTH_R*xyz.z,EARTH_R_SHORT*sqrt_x2_y2);
	double sintheata=sin(theata);
	double costheata=cos(theata);
	Vector3D tmp; //经纬度
	tmp.x=atan2(xyz.y,xyz.x); //经度没变换，直接求
	tmp.y=atan2(xyz.z+EARTH_R_SHORT*e12*sintheata*sintheata*sintheata,
				sqrt_x2_y2-EARTH_R*e2*costheata*costheata*costheata);
	double sinB=sin(tmp.y);
	double n=EARTH_R/sqrt(1-(e2*sinB*sinB)); //一定非0
	tmp.z=sqrt_x2_y2/cos(tmp.y)-n; //前边atan2里边不出INF，这里就不会是0
	tmp.x=rad2deg(tmp.x);
	tmp.y=rad2deg(tmp.y);
	return tmp; //算法未完成
}
//简化版
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz(Vector3D blh) //输入经纬度高程单位：度、米
{
	//首先转为弧度
	blh.x=deg2rad(blh.x); //L:经度
	blh.y=deg2rad(blh.y); //B:纬度
	Vector3D tmp; //直角坐标
	double sinB=sin(blh.y);
	double cosB=cos(blh.y);
	tmp.z=(EARTH_R+blh.z); //绝对高度
	tmp.x=tmp.z*cosB*cos(blh.x);
	tmp.y=tmp.z*cosB*sin(blh.x);
	tmp.z=tmp.z*sinB;
	return tmp;
}
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh(Vector3D xyz) //输出经纬度高程单位：度、米
{
	double x2=xyz.x*xyz.x;
	double y2=xyz.y*xyz.y;
	double z2=xyz.z*xyz.z;
	Vector3D tmp; //经纬度
	tmp.x=atan2(xyz.y,xyz.x); //经度没变换，直接求
	tmp.y=atan2((double)xyz.z,sqrt(x2+y2)); //纬度先求角度
	tmp.z=sqrt(x2+y2+z2)-EARTH_R;
	tmp.x=rad2deg(tmp.x);
	tmp.y=rad2deg(tmp.y);
	return tmp;
}

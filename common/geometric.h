/*
 * 文件名：geometric.h
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
 
使用的方程：
				2D			3D
	直线方程：y=kx+b	x=Kx*t+X0 |
						y=Ky*t+Y0 |-> X=Kt+X0
						z=Kz*t+Z0 | 
	面方程：|	X=K1t1 + K2t2 + X0 
			|	AX=b	A为面的垂线向量
	四元数的基本数学方程为:
	q = cos (a/2) + i(x * sin(a/2)) + j(y * sin(a/2)) + k(z * sin(a/2))
	其中a表示旋转角度，(x,y,z)表示旋转轴。

除0的避免：
	总结库中所有的除法：
	1、Vector的标量除法：		调用方判断
	2、Vector\QuatVec的norm归一化：		在库中判Eps，失败时不动作，需要外部判断返回的幅度
	3、Vector的angle夹角：		调用方判断
	4、Line的Cal_P_Dis直线上距离给定点一定距离的点		调用方保证直线k值非0，否则返回那个点本身
	5、Line的cross两直线交点：	除零时返回无交点
	6、Surface::cross线面交点：	除零时返回无交点
	7、QuatVec::fromVector:		除零时不动作，调用方保证输入非0
	8、Line3D::line_distance:	使用了norm，要求调用方保证不是平行线
	9、
*/

#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include "main.h"

const float PI=3.1415926f; //PI
#define deg2rad(a)	((a)*(PI/180.0f))
#define rad2deg(a)	((a)*(180.0f/PI))

#ifndef GEO_FLOAT 
#define GEO_FLOAT double //默认使用double，除非用户定义
#endif

GEO_FLOAT deg_range(GEO_FLOAT d); //角度限制在正负180度
GEO_FLOAT rad_range(GEO_FLOAT d); //弧度限制在正负180度

//定义2D点结构
struct Vector2D
{
	GEO_FLOAT x;
	GEO_FLOAT y;
	Vector2D() { x=0; y=0; }
	Vector2D(GEO_FLOAT xx,GEO_FLOAT yy)
	{
		x=xx; y=yy;
	}
	GEO_FLOAT magnitude(void); //取模
	GEO_FLOAT norm(void); //对自己进行归一化
	GEO_FLOAT distance(Vector2D &p1); //距离
	GEO_FLOAT angle(Vector2D &p1); //两向量夹角
#ifdef USECPP11 //4.9.2以上
	GEO_FLOAT distance(Vector2D &&p1); //距离
	GEO_FLOAT angle(Vector2D &&p1); //两向量夹角
#endif
};
const Vector2D Vector2DEmpty(0,0);

Vector2D operator+(Vector2D &p1,GEO_FLOAT k);
Vector2D operator-(Vector2D &p1,GEO_FLOAT k);
Vector2D operator*(Vector2D &p1,GEO_FLOAT k);
Vector2D operator/(Vector2D &p1,GEO_FLOAT k);

#ifndef USECPP11 //4.9.2
Vector2D operator+(Vector2D p1,Vector2D p2);
Vector2D operator-(Vector2D p1,Vector2D p2);
GEO_FLOAT operator*(Vector2D p1,Vector2D p2); //点乘
#else
Vector2D operator+(Vector2D &p1,Vector2D &p2);
Vector2D operator+(Vector2D &&p1,Vector2D &&p2);
Vector2D operator+(Vector2D &p1,Vector2D &&p2);
Vector2D operator+(Vector2D &&p1,Vector2D &p2);
Vector2D operator-(Vector2D &&p1,Vector2D &&p2);
Vector2D operator-(Vector2D &p1,Vector2D &&p2);
Vector2D operator-(Vector2D &&p1,Vector2D &p2);
Vector2D operator-(Vector2D &p1,Vector2D &p2);
GEO_FLOAT operator*(Vector2D &p1,Vector2D &p2); //点乘
GEO_FLOAT operator*(Vector2D &&p1,Vector2D &&p2); //点乘
GEO_FLOAT operator*(Vector2D &p1,Vector2D &&p2); //点乘
GEO_FLOAT operator*(Vector2D &&p1,Vector2D &p2); //点乘
Vector2D operator+(Vector2D &&p1,GEO_FLOAT k);
Vector2D operator-(Vector2D &&p1,GEO_FLOAT k);
Vector2D operator*(Vector2D &&p1,GEO_FLOAT k);
Vector2D operator/(Vector2D &&p1,GEO_FLOAT k);
#endif

//定义3D点结构
struct Vector3D
{
	GEO_FLOAT x;
	GEO_FLOAT y;
	GEO_FLOAT z;
	Vector3D() { x=0; y=0; z=0; }
	Vector3D(GEO_FLOAT xx,GEO_FLOAT yy,GEO_FLOAT zz)
	{
		x=xx; y=yy; z=zz;
	}
	GEO_FLOAT magnitude(void); //取模
	GEO_FLOAT norm(void); //对自己进行归一化
	GEO_FLOAT distance(Vector3D &p1); //距离
	GEO_FLOAT angle(Vector3D &p1); //两向量夹角
#ifdef USECPP11 //4.9.2以上
	GEO_FLOAT distance(Vector3D &&p1); //距离
	GEO_FLOAT angle(Vector3D &&p1); //两向量夹角
#endif
};
const Vector3D Vector3DEmpty(0,0,0);

#ifndef USECPP11 //4.9.2
Vector3D operator+(Vector3D p1,Vector3D p2);
Vector3D operator-(Vector3D p1,Vector3D p2);
GEO_FLOAT operator*(Vector3D p1,Vector3D p2); //点乘
Vector3D operator^(Vector3D p1,Vector3D p2);
Vector3D operator+(Vector3D p1,GEO_FLOAT k);
Vector3D operator-(Vector3D p1,GEO_FLOAT k);
Vector3D operator*(Vector3D p1,GEO_FLOAT k);
Vector3D operator/(Vector3D p1,GEO_FLOAT k);
#else
Vector3D operator+(Vector3D &p1,Vector3D &p2);
Vector3D operator+(Vector3D &&p1,Vector3D &&p2);
Vector3D operator+(Vector3D &p1,Vector3D &&p2);
Vector3D operator+(Vector3D &&p1,Vector3D &p2);
Vector3D operator-(Vector3D &p1,Vector3D &p2);
Vector3D operator-(Vector3D &&p1,Vector3D &&p2);
Vector3D operator-(Vector3D &p1,Vector3D &&p2);
Vector3D operator-(Vector3D &&p1,Vector3D &p2);
GEO_FLOAT operator*(Vector3D &p1,Vector3D &p2); //点乘
GEO_FLOAT operator*(Vector3D &&p1,Vector3D &&p2); //点乘
GEO_FLOAT operator*(Vector3D &p1,Vector3D &&p2); //点乘
GEO_FLOAT operator*(Vector3D &&p1,Vector3D &p2); //点乘
Vector3D operator^(Vector3D &p1,Vector3D &p2); //叉乘
Vector3D operator^(Vector3D &&p1,Vector3D &&p2); //叉乘
Vector3D operator^(Vector3D &p1,Vector3D &&p2); //叉乘
Vector3D operator^(Vector3D &&p1,Vector3D &p2); //叉乘
Vector3D operator+(Vector3D &p1,GEO_FLOAT k);
Vector3D operator+(Vector3D &&p1,GEO_FLOAT k);
Vector3D operator-(Vector3D &p1,GEO_FLOAT k);
Vector3D operator-(Vector3D &&p1,GEO_FLOAT k);
Vector3D operator*(Vector3D &p1,GEO_FLOAT k);
Vector3D operator*(Vector3D &&p1,GEO_FLOAT k);
Vector3D operator/(Vector3D &p1,GEO_FLOAT k);
Vector3D operator/(Vector3D &&p1,GEO_FLOAT k);
#endif

//定义2D线结构
struct Line2D
{
	Vector2D k;
	Vector2D p;
	void create(Vector2D &p1,Vector2D &p2); //两点生成线
	Vector2D vertical(void); //求此直线的垂线向量
	Line2D vertical(Vector2D &p1); //求此直线的垂线（过已知点）
	bool cross(Line2D &l,Vector2D &p1); //求两直线的交点
	Vector2D Cal_P_Dis(Vector2D &p1,GEO_FLOAT d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector2D vert_point(Vector2D &p1); //点到直线的垂点
	GEO_FLOAT distance(Vector2D &p1); //点到直线距离

#ifdef USECPP11 //4.9.2以上
	void create(Vector2D &&p1,Vector2D &&p2); //两点生成线
	void create(Vector2D &p1,Vector2D &&p2); //两点生成线
	void create(Vector2D &&p1,Vector2D &p2); //两点生成线
	Line2D vertical(Vector2D &&p1); //求此直线的垂线（过已知点）
	bool cross(Line2D &&l,Vector2D &p1); //求两直线的交点
	Vector2D Cal_P_Dis(Vector2D &&p1,GEO_FLOAT d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector2D vert_point(Vector2D &&p1); //点到直线的垂点
	GEO_FLOAT distance(Vector2D &&p1); //点到直线距离
#endif
};
const Line2D Line2DEmpty={Vector2DEmpty,Vector2DEmpty};

//定义3D线结构
struct Line3D
{
	Vector3D k;
	Vector3D p;
	void create(Vector3D &p1,Vector3D &p2); //两点生成线
	Line3D vertical(Vector3D &p1); //求此直线的垂线（过已知点）
	bool cross(Line3D &l,Vector3D &p1); //求两直线的交点(相距最近的点)
	GEO_FLOAT line_distance(Line3D &l); //求两直线间距
	Vector3D Cal_P_Dis(Vector3D &p1,GEO_FLOAT d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector3D vert_point(Vector3D &p1); //点到直线的垂点
	GEO_FLOAT distance(Vector3D &p1); //点到直线距离

#ifdef USECPP11 //4.9.2以上
	void create(Vector3D &&p1,Vector3D &&p2); //两点生成线
	void create(Vector3D &p1,Vector3D &&p2); //两点生成线
	void create(Vector3D &&p1,Vector3D &p2); //两点生成线
	Line3D vertical(Vector3D &&p1); //求此直线的垂线（过已知点）
	bool cross(Line3D &&l,Vector3D &p1); //求两直线的交点(相距最近的点)
	GEO_FLOAT line_distance(Line3D &&l); //求两直线间距
	Vector3D Cal_P_Dis(Vector3D &&p1,GEO_FLOAT d); //在直线上，距离给定点一定距离的点。（距离有正负之分）
	Vector3D vert_point(Vector3D &&p1); //点到直线的垂点
	GEO_FLOAT distance(Vector3D &&p1); //点到直线距离
#endif
};
const Line3D Line3DEmpty={Vector3DEmpty,Vector3DEmpty};

//定义面结构
struct Surface
{
	Vector3D k;
	GEO_FLOAT b;
	void create(Line3D &l,Vector3D &p1); //点线确定面
	void create_vert(Line3D &l,Vector3D &p1); //点线确定面,线的垂面
	bool cross(Line3D &l,Vector3D &p1); //线面交点，返回是否有交点
	Vector3D vert_point(Vector3D &p1); //点面垂点
	GEO_FLOAT distance(Vector3D &p1); //点面距

#ifdef USECPP11 //4.9.2以上
	void create(Line3D &&l,Vector3D &&p1); //点线确定面
	void create(Line3D &l,Vector3D &&p1); //点线确定面
	void create(Line3D &&l,Vector3D &p1); //点线确定面
	void create_vert(Line3D &&l,Vector3D &&p1); //点线确定面,线的垂面
	void create_vert(Line3D &l,Vector3D &&p1); //点线确定面,线的垂面
	void create_vert(Line3D &&l,Vector3D &p1); //点线确定面,线的垂面
	bool cross(Line3D &&l,Vector3D &p1); //线面交点，返回是否有交点
	Vector3D vert_point(Vector3D &&p1); //点面垂点
	GEO_FLOAT distance(Vector3D &&p1); //点面距
#endif
};
const Surface SurfaceEmpty={Vector3DEmpty,0};

//定义2D有向线段
struct Segment2D
{
	Vector2D st;
	Vector2D end;
	Segment2D(){}
	Segment2D(const Vector2D &p1,const Vector2D &p2)
	{
		st=p1;
		end=p2;
	}
	Vector2D proj_point(Vector2D &p1); //点到线段距离最短的点（点到线段距离）
#ifdef USECPP11 //4.9.2以上
	Segment2D(Vector2D &&p1,Vector2D &&p2):Segment2D(p1,p2) {} //两点生成线
	Segment2D(Vector2D &p1,Vector2D &&p2):Segment2D(p1,p2) {} //两点生成线
	Segment2D(Vector2D &&p1,Vector2D &p2):Segment2D(p1,p2) {} //两点生成线
	Vector2D proj_point(Vector2D &&p1); //点到线段距离最短的点（点到线段距离）
#endif
};
const Segment2D Segment2DEmpty(Vector2DEmpty,Vector2DEmpty);

//定义3D有向线段
struct Segment3D
{
	Vector3D st;
	Vector3D end;
	Segment3D(){}
	Segment3D(const Vector3D &p1,const Vector3D &p3)
	{
		st=p1;
		end=p3;
	}
	Vector3D proj_point(Vector3D &p1); //点到线段距离最短的点（点到线段距离）
#ifdef USECPP11
	Segment3D(Vector3D &&p1,Vector3D &&p3):Segment3D(p1,p3) {} //两点生成线
	Segment3D(Vector3D &p1,Vector3D &&p3):Segment3D(p1,p3) {} //两点生成线
	Segment3D(Vector3D &&p1,Vector3D &p3):Segment3D(p1,p3) {} //两点生成线
	Vector3D proj_point(Vector3D &&p1); //点到线段距离最短的点（点到线段距离）
#endif
};
const Segment3D Segment3DEmpty(Vector3DEmpty,Vector3DEmpty);

////////////////////////////////////////////////////////////////
//					旋转变换
////////////////////////////////////////////////////////////////
//四元数
struct QuatVec
{
	GEO_FLOAT w;
	GEO_FLOAT x;
	GEO_FLOAT y;
	GEO_FLOAT z;
	QuatVec() { w=1; x=0; y=0; z=0; }
	QuatVec(GEO_FLOAT ww,GEO_FLOAT xx,GEO_FLOAT yy,GEO_FLOAT zz)
	{
		w=ww; x=xx; y=yy; z=zz;
	}
	GEO_FLOAT norm(void); //对自己进行归一化
	QuatVec inv(void); //逆，就是共轭，认为自己是单位四元数
	void rot(Vector3D &p); //旋转一个点
	Vector3D toEuler_zyx(void); //转换为欧拉角内旋(航空欧拉角)
	Vector3D toEuler_zxy(void); //转换为欧拉角内旋
	Vector3D toEuler_yxz(void); //转换为欧拉角内旋
	Vector3D toEuler_xzy(void); //转换为欧拉角内旋
	Vector3D toEuler_yzx(void); //转换为欧拉角内旋
	void fromEuler_zyx(Vector3D &p); //欧拉角转四元数内旋(航空欧拉角)
	void fromEuler_zxy(Vector3D &p); //欧拉角转四元数内旋
	void fromEuler_yxz(Vector3D &p); //欧拉角转四元数内旋
	void fromEuler_xzy(Vector3D &p); //欧拉角转四元数内旋
	void fromEuler_yzx(Vector3D &p); //欧拉角转四元数内旋
	void fromAxis(Vector3D &axis,GEO_FLOAT angle); //从转轴和转角构造
	void fromVector(Vector3D &v); //从向量构造，不完全约束，按直接转过去

#ifdef USECPP11 //4.9.2以上
	void fromEuler_zyx(Vector3D &&p); //欧拉角转四元数内旋(航空欧拉角)
	void fromEuler_zxy(Vector3D &&p); //欧拉角转四元数内旋
	void fromEuler_yxz(Vector3D &&p); //欧拉角转四元数内旋
	void fromEuler_xzy(Vector3D &&p); //欧拉角转四元数内旋
	void fromEuler_yzx(Vector3D &&p); //欧拉角转四元数内旋
	void fromAxis(Vector3D &&axis,GEO_FLOAT angle); //从转轴和转角构造
	void fromVector(Vector3D &&v); //从向量构造，不完全约束，按直接转过去
#endif
};
const QuatVec QuatVecEmpty(1,0,0,0);

QuatVec operator*(QuatVec &q1,QuatVec &q2); //乘号右侧的先转，恰好与旋转矩阵一样

#ifdef USECPP11 //4.9.2以上
QuatVec operator*(QuatVec &&q1,QuatVec &&q2);
QuatVec operator*(QuatVec &q1,QuatVec &&q2);
QuatVec operator*(QuatVec &&q1,QuatVec &q2);
#endif

//换坐标：已知两个姿态的大地坐标，求在A为参照系下B的姿态
QuatVec change_coord(QuatVec &u,QuatVec &coord); //输入载荷姿态、载体姿态（参照系）

Vector2D matrix_trans2D(Vector2D &k,GEO_FLOAT t[][3]); //2D矩阵变换
Vector3D matrix_trans3D(Vector3D &k,GEO_FLOAT t[][4]); //3D矩阵变换
Vector2D translate2D(Vector2D &k,GEO_FLOAT tx,GEO_FLOAT ty); //2D向量平移变换
Vector2D scale2D(Vector2D &k,GEO_FLOAT a,GEO_FLOAT d); //2D相对于(0,0)比例变换
Vector2D scaleToVector2D(Vector2D &k,Vector2D &c,GEO_FLOAT a,GEO_FLOAT d); //2D相对于C(cx,cy)比例变换

Vector2D rotate2D(Vector2D &k,GEO_FLOAT r); //2D相对于(0,0)旋转变换 //弧度
#ifdef USECPP11 //4.9.2以上
Vector2D rotate2D(Vector2D &&k,GEO_FLOAT r); //2D相对于(0,0)旋转变换 //弧度
#endif

Vector2D rotateToVector2D(Vector2D &k,Vector2D &c,GEO_FLOAT r); //2D相对于C(cx,cy)旋转变换
//3D旋转:输入点和欧拉角单个角
Vector3D rotate3D_euler_x(Vector3D &p,GEO_FLOAT ang); //弧度制
Vector3D rotate3D_euler_y(Vector3D &p,GEO_FLOAT ang); //弧度制
Vector3D rotate3D_euler_z(Vector3D &p,GEO_FLOAT ang); //弧度制

//3D旋转:z-x-y输入点和欧拉角
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &euler);//弧度制
//3D旋转:z-y-x输入点和欧拉角(航空欧拉角)
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &euler);//弧度制
//3D旋转:y-x-z输入
Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &euler);//弧度制

#ifdef USECPP11 //4.9.2以上
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zxy(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zxy(Vector3D &&p,Vector3D &euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_zyx(Vector3D &&p,Vector3D &euler);//弧度制
Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_yxz(Vector3D &p,Vector3D &&euler);//弧度制
Vector3D rotate3D_euler_yxz(Vector3D &&p,Vector3D &euler);//弧度制
#endif

////////////////////////////////////////////////////////////////
//地理算法
////////////////////////////////////////////////////////////////
const double EARTH_R=6378137.0; //地球半径
const double EARTH_R_SHORT=6356752.0; //地球短半轴

#define wgs2xy(x)	((x)*EARTH_R*PI/180) //经纬度转距离
#define xy2wgs(x)	((x)/EARTH_R/PI*180) //距离转经纬度

//局部直角坐标转经纬度，输入中央经纬线
Vector3D localxy_2_blh(Vector3D p,Vector3D &cen);//要求外部保证不除0，判断纬度范围
//经纬度转局部直角坐标，输入中央经纬线
Vector3D blh_2_localxy(Vector3D p,Vector3D &cen);
//球面距离
double EarthDistance(double lng1, double lat1, double lng2, double lat2); //地球两点距离
double EarthDistance(Vector2D p0, Vector2D p1); //地球两点距离

//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz_ellipse(Vector3D blh); //椭圆法，输入经纬度高程单位：度、米
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh_ellipse(Vector3D xyz); //椭圆法，输出经纬度高程单位：度、米
//简化版
//经纬度转,B L H->纬度，精度，高度，输入顺序为经纬高
Vector3D blh_2_xyz(Vector3D blh); //输入经纬度高程单位：度、米
//XYZ转经纬度,B L H->纬度，精度，高度，顺序为经纬高 
Vector3D xyz_2_blh(Vector3D xyz); //输出经纬度高程单位：度、米

# endif


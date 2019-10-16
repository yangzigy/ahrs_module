ahrs_module
======
# 模块  
使用stm32f103t8u6，6轴惯导icm20602和QMC5883L，双排插针引出3.3V电源，和TX、RX；SWD两条线，调试串口TX、RX  
![image](https://github.com/yangzigy/ahrs_module/raw/master/pictures/module0.jpg)  
1. 电压：3.2V~3.4V  
2. 电流：<40mA  
3. 体积：20.32mm X 13.97mm X 2mm  
4. 测量维度：加速度：3 维，角速度：3 维，磁场：3 维，角度：3 维  
5. 量程：加速度:±2g，角速度:±250°/s，角度±180°  
6. 分辨率：加速度：0.0001g，角速度:0.01°/s  
7. 稳定性：加速度：0.05g，角速度 1°/s  
8. 姿态测量稳定度：0.2°抗加速影响  
9. 数据输出内容：加速度、角速度、角度、磁场  
10. 数据输出频率2Hz~500Hz  
11. 数据接口：串口（TTL 电平，波特率支持 2400、4800、9600、19200、38400、57600、115200、230400、460800、921600）  
规定x轴为前方，y轴为左方，z轴为上方。欧拉角为航空欧拉角，轴序为zyx  
# 界面  
配置ahrs模块，显示传感数据，绘制曲线，并提供罗盘校准功能  
![image](https://github.com/yangzigy/ahrs_module/raw/master/pictures/gui0.png)  
![image](https://github.com/yangzigy/ahrs_module/raw/master/pictures/gui.png)  
# 协议  
输出9轴传感数据，共22字节  

| **位置** | **类型** | **名称** | **功能**                    |
| -------- | -------- | -------- | --------------------------- |
| 0        | u8       | syn      | 同步字 0xaa                 |
| 1        | u8       | type     | 类型：1：9轴数据包          |
| 2        | s16      | gx       | 陀螺仪x轴角速度，0.01度     |
| 4        | s16      | gy       | 陀螺仪y轴角速度，0.01度     |
| 6        | s16      | gz       | 陀螺仪z轴角速度，0.01度     |
| 8        | s16      | ax       | 加速度x轴，0.0001g          |
| 10       | s16      | ay       | 加速度y轴，0.0001g          |
| 12       | s16      | az       | 加速度z轴，0.0001g          |
| 14       | s16      | mx       | 磁力计x,0.0001G             |
| 16       | s16      | my       | 磁力计y,0.0001G             |
| 18       | s16      | mz       | 磁力计z,0.0001G             |
| 20       | u8       | dtime    | 距上次发送的时间，单位0.1ms |
| 21       | u8       | sum      | 校验和,从头开始累加         |
输出姿态数据，共26字节  

| **位置** | **类型** | **名称** | **功能**            |
| -------- | -------- | -------- | ------------------- |
| 0        | u8       | syn      | 同步字 0xaa         |
| 1        | u8       | type     | 类型：2：姿态数据包 |
| 2        | s16      | ux       | 航空欧拉角x，0.01度 |
| 4        | s16      | uy       | 航空欧拉角y，0.01度 |
| 6        | s16      | uz       | 航空欧拉角z，0.01度 |
| 8        | float    | q0       | 四元数 w            |
| 12       | float    | q1       | 四元数 x            |
| 16       | float    | q2       | 四元数 y            |
| 20       | float    | q3       | 四元数 z            |
| 24       | u8       | res      | 保留                |
| 25       | u8       | sum      | 校验和,从头开始累加 |
配置数据输出：24字节，配置指令包发送后回复  

| **位置** | **类型** | **名称**   | **功能**                                                     |
| -------- | -------- | ---------- | ------------------------------------------------------------ |
| 0        | u8       | syn        | 同步字 0xaa                                                  |
| 1        | u8       | type       | 类型：3：配置数据包                                          |
| 2        | s16      | mkx        | 磁力计比例x,0.0001                                           |
| 4        | s16      | moffx      | 磁力计0偏x,0.0001                                            |
| 6        | s16      | mky        | 磁力计比例y,0.0001                                           |
| 8        | s16      | moffy      | 磁力计0偏y,0.0001                                            |
| 9        | s16      | mkz        | 磁力计比例z,0.0001                                           |
| 10       | s16      | moffz      | 磁力计0偏z,0.0001                                            |
| 12       | s16      | g_offset_x | 陀螺仪x轴零偏，0.01度                                        |
| 14       | s16      | g_offset_y | 陀螺仪y轴零偏，0.01度                                        |
| 16       | s16      | g_offset_z | 陀螺仪z轴零偏，0.01度                                        |
| 18       | u8       | fre1       | 传感数据包输出分频数（频率为500除以分频数）                  |
| 19       | u8       | fre2       | 姿态数据包输出分频数（频率为500除以分频数）                  |
| 20       | u8       | mod        | bit0: 是否上电校准: 0：校准，1：不校准  bit1: 0：9轴模式1:6轴模式  bit2~bit5：串口波特率，0~9表示    9600,19200,28800,38400,57600,115200,    230400,460800,500000,576000,921600 |
| 21       | u8       | sum        | 校验和,从头开始累加                                          |
输出500Hz时，传感数据为11000B/s，姿态数据为13000B/s。  
配置指令包：8字节  

| **位置** | **类型** | **名称** | **功能**                                                     |
| -------- | -------- | -------- | ------------------------------------------------------------ |
| 0        | u8       | syn      | 同步字 0xaa                                                  |
| 1        | u8       | type     | 类型：4：配置指令包                                          |
| 2        | s16      | data0    | 数据内容由cmd决定                                            |
| 4        | s16      | data1    |                                                              |
| 6        | u8       | cmd      | 指令：  0：查询配置  1：data0为磁力计x比例0.001，data1为0偏修正x  2：data0为磁力计y比例0.001，data1为0偏修正y  3：data0为磁力计z比例0.001，data1为0偏修正z  4：传感数据包输出分频数（实际频率为500除以分频数）  5：姿态数据包输出分频数（实际频率为500除以分频数）  6：是否上电校准：  data=0：校准，1：不校准，2：立刻校准  7：0：9轴模式1:6轴模式  8：设置串口波特率，data0=0~9表示    9600,19200,28800,38400,57600,115200,    230400,460800,500000,576000,921600  9：设置x轴陀螺零偏0.001  10:设置y轴陀螺零偏0.001  11:设置z轴陀螺零偏0.001  12:设置x轴加计零偏0.001  13:设置y轴加计零偏0.001  14:设置z轴加计零偏0.001  254：恢复出厂设置  255：保存当前配置 |
| 7        | u8       | sum      | 校验和,从头开始累加                                          |


#include "main.h"
#include "stm32f1_sys.h" 	
#include "uart.h"
#include "cmd.h"
#include "comm_rec.h"
#include "mpu6050.h"
#include "qmc5883l.h"
#include "config.h"
#include "led.h"

extern void app_ini(void);
extern void app_poll(void);
extern COMM_HEAD configtask_head;

//系统变量
u32 SYS_task=0;
u16 SYS_idle=0;
u16 SYS_idle_dis=0;
u32 SYS_time=0;

//void delay_us(u32 us)
//{
	//s32 pre=SysTick->VAL;
	//s32 tmp=0;
	//us=us*9;
	//while(tmp<us)
	//{
		//tmp=pre-SysTick->VAL;
		//if(tmp<0) tmp=pre+SysTick->LOAD-SysTick->VAL;
	//}
//}
void delay_us(u32 us)
{
	wait(2);
}
float tickT=(1/500.0f); //单位秒

void systime_initial(u32 frq)//输入为中断频率
{
	//输入频率默认HCLK/8=9MHz=9000000
	SysTick->LOAD=9000000/frq;
	SysTick->CTRL |= 2;//开中断
	SysTick->CTRL |= 1;
}
int main(void)
{
	Stm32_Clock_Init(9);//系统时钟,所有都必须在此函数后
	RCC->APB2ENR|=0x1ff;   //使能所有PORT口时钟
	systime_initial(10000);//Hz
	MGPIOC->CT6=GPIO_OUT_PP; //LED
	uart_initial(&uart1,921600);
	cmd_ini(&(uart1.que_rx)); //使用串口1作为命令串口
	printf(HelloString);
	led_ini();
	MPU_Init();
	qmc_ini();
	cfg_load();
	app_ini();
	while(1)
	{
		int i;
		for(i=0;i<200;i++)
		{
			u8 tt;
			if(uart1.que_rx.dlen>0) //这个只读不用锁
			{
				cmd();
				SYS_task &= ~TASK_CMD;
			}
			if(uart2.que_rx.dlen>0) //这个只读不用锁
			{
				while(get_que_data(&tt,&uart2.que_rx)==0)
				{
					rec_head(tt,&configtask_head);
				}
			}
			if(SYS_task & TASK_POOL) //500Hz
			{
				static u32 pre_time=0;
				tickT=(SYS_time-pre_time)/10000.0f;
				pre_time=SYS_time;
				SYS_task &= ~TASK_POOL;
				app_poll();
			}
		}
		SYS_idle++;
	}
}

void SysTick_Handler(void)
{
	SYS_time++;
	if(SYS_time%20==1) //500Hz
	{
		START_TASK(TASK_POOL);
	}
	if(SYS_time%1000==2) //10Hz
	{
		SYS_idle_dis=(SYS_idle_dis+SYS_idle)/2;
		SYS_idle=0;
		led_tick();
	}
}

void wait(vu32 i)
{
	while(i--);
}

int void_putchar(int c)
{
	return 0;
}

int (*IO_putchar)(int)=uart1_putchar;	//初始化的函数

int fputc(int c, FILE *f)
{
	return IO_putchar(c);
}
int get_que_data(u8 *p,Queue *q)
{
	int rst=1;
	OS_CLOSE_INT;
	rst=Queue_get_1(p,q);
	OS_OPEN_INT;
	return rst;
}



#include "led.h"

S_LED_CTRL led_list[]=
{
	GPIOB,5,1,  1,2,  0, //
};
void led_ini(void)
{
	int i;
	for(i = 0; i < sizeof(led_list)/sizeof(S_LED_CTRL); i++)
	{
		if(led_list[i].pin<8)
		{
			led_list[i].port->CRL &= ~(0x0f<<led_list[i].pin*4);
			led_list[i].port->CRL |= (GPIO_OUT_PP<<led_list[i].pin*4);
		}
		else if(led_list[i].pin<16)
		{
			u8 pin=led_list[i].pin-8;
			pin*=4;
			led_list[i].port->CRH &= ~(0x0f<<pin);
			led_list[i].port->CRH |= (GPIO_OUT_PP<<pin);
		}
		//初始化为不亮
		led_list[i].port->BSRR=(1<<led_list[i].pin)<<(16*led_list[i].light_lev);
	}
}
void led_toggle(int offset) //对于非周期的，触发led
{
	if(offset<sizeof(led_list)/sizeof(S_LED_CTRL))
	{
		led_list[offset].tick=0;
	}
}
void led_period(int offset,int t) //设置led周期，单位tick，若为0则为关闭
{
	if(offset<sizeof(led_list)/sizeof(S_LED_CTRL))
	{
		led_list[offset].T=t;
		if(t==0)
		{
			led_list[offset].tick=led_list[offset].duty;
		}
	}
}
void led_tick(void)
{
	int i;
	for(i = 0; i < sizeof(led_list)/sizeof(S_LED_CTRL); i++)
	{
		if(led_list[i].tick<led_list[i].duty)//若需要亮
		{
			led_list[i].port->BSRR=(1<<led_list[i].pin)<<(16*(1-led_list[i].light_lev));
			led_list[i].tick++;
		}
		else //若需要灭
		{
			//是否归零？
			if(led_list[i].T>=led_list[i].duty) //若不是触发型
			{
				if(led_list[i].tick<led_list[i].T)
				{
					led_list[i].tick++;
				}
				else
				{
					led_list[i].tick=0;
				}
			}
			led_list[i].port->BSRR=(1<<led_list[i].pin)<<(16*led_list[i].light_lev);
		}
	}
}

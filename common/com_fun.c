#include "com_fun.h"

u8 check_sum(u8 *p,int n)
{
	u8 acc=0;
	int i;
	for(i = 0; i < n; i++)
	{
		acc+=p[i];
	}
	return acc;
}


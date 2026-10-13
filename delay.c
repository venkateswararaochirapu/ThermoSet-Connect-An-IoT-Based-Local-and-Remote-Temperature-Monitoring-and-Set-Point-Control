#include "types.h"

void delay_us(unsigned int dly)
{
	dly=12;
	while(dly--);
}
void delay_ms(unsigned int dly)
{
	for(dly*=12000;dly>0;dly--);
}
void delay_s(unsigned int dly)
{
	dly=12000000;
	while(dly--);
}

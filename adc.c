//adc.c
#include "types.h"
#include "defines.h"
#include "adc_defines.h"
#include <LPC21xx.h>
#include "delay.h"

void Init_ADC(void)
{
	//cfg between p0.27-p0.30 as
	//analog input pins
	PINSEL1&=(u32)~0xFF<<((27-16)*2);
	PINSEL1|=AIN0_PIN|AIN1_PIN;
	
	//cfg ADCR
	ADCR|=(1<<PDN_BIT)|(CLKDIV<<CLKDIV_BITS);
}

void Read_ADC(u8 chNo,u16 *dVal,f32 *eAR)
{
	//clear prev channel selection
	ADCR&=0xFFFFFF00;
	//select ch,& start conversion
	ADCR|=(1<<ADC_START_BIT)|(1<<chNo);
	//wait for conversion time
	delay_us(3);
	while(READBIT(ADDR,DONE_BIT)==0);
	//stop adc conversion
	CLRBIT(ADCR,ADC_START_BIT);
	*dVal=(ADDR>>RESULT_BITS)&1023;
	//*eAR= (*dVal * (3.3/1023));
	*eAR=0.0;
}

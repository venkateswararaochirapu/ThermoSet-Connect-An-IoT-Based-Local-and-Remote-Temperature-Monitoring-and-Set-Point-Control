//lm35.c
#include "types.h"
#include "adc.h"
u32 Read_Temperature(void)
{
		u16 dVal;  f32 eAR; 
    static u32 flag	;
	  if(flag==0)
		{	
		  Init_ADC();
			flag=1;
		}	
		Read_ADC(1,&dVal,&eAR);   //LM35 on AD0.1 (P0.28), per your earlier pin choice
		//return (eAR*100);
		return ((dVal * 330)+511)/1023;
}

s32 Read_TemperatureNP(void)
{
		u16 dVal1,dVal2;  
	  f32 eAR1,eAR2;
		s32 diff;
    static u32 flag	;
	  if(flag==0)
		{	
		  Init_ADC();
			flag=1;
		}	
		Read_ADC(0,&dVal1,&eAR1);
		Read_ADC(1,&dVal2,&eAR2);
		//return ((eAR1-eAR2)*100);
		
	  diff = (s32)dVal1 - (s32)dVal2;
		
		if(diff>=0)
			return ((diff*330)+511)/1023;
		else
			return ((diff*330)-680)/1023;
}

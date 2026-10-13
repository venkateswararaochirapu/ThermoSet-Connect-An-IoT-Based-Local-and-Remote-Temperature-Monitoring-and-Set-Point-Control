#include <lpc21xx.h>
#include "lcd_defines.h"
#include "types.h"
#include "delay.h"
#include "defines.h"

void WriteLCD(u8 Byte)
{
	//write any byte on data pins
	WRITEBYTE(IOPIN0,LCD_DATA,Byte);
	//sel write operation
	IOCLR0 = 1<<LCD_RW;
	//provide high to low pulse on enable pin
	IOSET0 = 1<<LCD_EN;
	delay_ms(1);
	IOCLR0 = 1<<LCD_EN;
	delay_ms(2);
}

void CmdLCD(u8 cmd)
{
	//sel cmd mode
	IOCLR0 = 1<<LCD_RS;
	//write to cmd register
	WriteLCD(cmd);
}

void InitLCD(void)
{
	//cfg p0.8~p0.15,p0.16,p0.17,p0.17 as gpio out pins
	IODIR0 |= (0xFF<<LCD_DATA) | (1<<LCD_RS) | (1<<LCD_RW) | (1<<LCD_EN);
	delay_ms(15);
	CmdLCD(0x30);
	delay_ms(4);
	delay_us(500);
	CmdLCD(0x30);
	delay_us(100);
	CmdLCD(0x30);
	CmdLCD(MODE_8BIT_2LINE);
	CmdLCD(DISP_ON_CUR_ON);
	CmdLCD(CLEAR_LCD);
	CmdLCD(SHIFT_CUR_RIGHT);
}

void CharLCD(u8 asciival)
{
	//sel data register
	IOSET0 = 1<<LCD_RS;
	//write to display
	WriteLCD(asciival);
}

void StrLCD(s8 *str)
{
	while(*str)
		CharLCD(*str++);
}

void U32LCD(u32 n)
{
	u8 a[16];
	s32 i=0;
	if(n==0)
	{
		CharLCD('0');
	}
	else
	{
		while(n)
		{
			a[i]=(n%10)+48;
			n/=10;
			i++;
		}
		a[i]='\0';
		for(--i;i>=0;i--)
		{
			CharLCD(a[i]);
		}
		
	}
}

void S32LCD(s32 n)
{
	if(n<0)
	{
		CharLCD('-');
		n=-n;
	}
	U32LCD(n);
}

void F32LCD(f32 fn,u8 nDP)
{
	u32 n;
	s32 i;
	if(fn<0)
	{
		CharLCD('-');
		fn = -fn;
	}
	n=fn;
	U32LCD(n);
	CharLCD('.');
	for(i=0;i<nDP;i++)
	{
		fn=(fn-n)*10;
		n=fn;
		CharLCD(n+48);
	}
}

void BuildCGRAM(u8 *p,u8 nBytes)
{
	u8 i;
	//cmd for goto cgram start
	CmdLCD(GOTO_CGRAM_START);
	//write into cgram
	//select data reg
	IOSET0 = 1<<LCD_RS;
	for(i=0;i<nBytes;i++)
	{
		WriteLCD(p[i]);
	}
	//point back to ddram start
	CmdLCD(GOTO_LINE1_POS0);
}

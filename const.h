#ifndef __CONST_H__
#define __CONST_H__

typedef unsigned char Bool2;
typedef unsigned char bool2;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#define FALSE 				( 0 )
#define TRUE  				( 1 )

#define LOGIC_FALSE     	( 0 )
#define LOGIC_TRUE      	( 1 )

sbit SDA = P2^3;  
sbit SCL = P2^2; 

sbit DE = P3^2;

sbit Out1 = P2^7;
sbit Out2 = P2^6;
sbit Speak = P3^2;

sbit LCD_CS = P1^5;
sbit LCD_RD = P1^4;
sbit LCD_WR = P1^3;
sbit LCD_DATA = P1^2;

sbit DATA = P2^5;
sbit SCK = P2^4;

sbit Key_Menu = P2^0;
sbit Key_Up = P1^6;
sbit Key_Down = P1^7;
sbit Key_Enter = P2^1;

#define T0_20MS_INIT_VALUE	( 0xfa00 )	// 5ms

#define KEY_NO			    ( 0 )
#define KEY_ENTER		    ( 4 )
#define KEY_UP			    ( 8 )
#define KEY_DOWN		    ( 1 )
#define	KEY_MENU		    ( 2 )
#define KEY_ESC			    KEY_MENU
#define KEY_MENUADDENTER	( 6 )
#define KEY_MENUADDDOWN		( 3 )

#define	BAUD1200			0xf8
#define	BAUD2400			0xfc
#define	BAUD4800			0xfe
#define	BAUD9600			0xff

#define ADR_FLASH_TEMPOFFSET    ( 0x0000 )
#define ADR_FLASH_HUMIOFFSET    ( 0x0010 )

//#define ADR_FLASH_HUMIHIGH    ( 0x0020 )
//#define ADR_FLASH_HUMILOW		( 0x0030 )

#define ADR_FLASH_HUMISET    	( 0x0040 )
#define ADR_FLASH_TEMPSET		( 0x0050 )
#define ADR_FLASH_BAUD			( 0x0060 )
#define ADR_FLASH_ADDRESS		( 0x0070 )

#define ADR_FLASH_CHECK      (0x0020)    // E2PROM初始化校验

#define FLASH_CHECK          0x55aa

#define HUMIOFFSET          ( 0 )
#define TEMPOFFSET          ( 1 )

#define HUMIOFFSETMAX       ( 150 )
#define HUMIOFFSETMIN       ( -150 )
#define TEMPOFFSETMAX       ( 25 )
#define TEMPOFFSETMIN       ( -25 )

#define HUMISETMAX			( 999 )
#define HUMISETMIN			( 1 )
#define TEMPSETMAX			( 1400 )
#define TEMPSETMIN			( -200 )

#define SYS_HALT			( 0 )
#define SYS_SET_OFFSETWAIT  ( 1 )
#define SYS_SET_HUMIOFFSET  ( 2 )
#define SYS_SET_TEMPOFFSET  ( 3 )
#define	SYS_SET_TEMP		( 4 )
#define SYS_SET_HUMI		( 5 )
#define SYS_SET_COMMUWAIT	( 6 )
#define SYS_SET_BAUD		( 7 )
#define SYS_SET_ADR			( 8 )

//	PORT
#define STH_DATA_IN()	DATA = 1
#define STH_DATA_OUT()
#define STH_DATA_0()	DATA = 0
#define STH_DATA_1()	DATA = 1
#define ReadSth10Sda()  ( DATA )

#define STH_SCK_OUT()
#define STH_SCK_0()		SCK = 0
#define STH_SCK_1()		SCK = 1

#define LCD_OUT()
#define LCD_DATA_1()	LCD_DATA = 1
#define LCD_DATA_0()	LCD_DATA = 0
#define LCD_WR_1()		LCD_WR = 1
#define LCD_WR_0()		LCD_WR = 0
#define LCD_RD_1()		LCD_RD = 1
#define LCD_RD_0()		LCD_RD = 0
#define LCD_CS_1()		LCD_CS = 1
#define LCD_CS_0()		LCD_CS = 0

#define Set485Receive()	{ DE = 1; }
#define Set485Send()	{ DE = 0; }

#define Dog()			WDT_CONTR = 0x3e;
#define WatchDog()		WDT_CONTR = 0x3e;
#define _NOP()			_nop_()

#endif



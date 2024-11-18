
#include <intrins.h>
#include <string.h>
#include "STC12C5410AD.H"
#include "const.h"

#define VERSION 25

unsigned char a[14];
unsigned char select_mode=0,set_mode=0;
bit	select_menu_flag=0,set_mode_flag=0;
bit set_offset_flag=0,set_temp_offset_flag=0;
unsigned char baud;
unsigned char address;
unsigned int xdata	count_time=0,count_time2=0,count_time3=0;
unsigned int xdata	key_time_cont=0,key_time_cont2=0;
unsigned char xdata	key_time_cont3=0,key_time_cont4=0;

bit	flash_flag=1;
bit	xiaozhun_humi_falg=1,xiaozhun_temp_falg=1;

unsigned char xdata	humi_offset_hig=0,temp_offset_hig=0;
int	xdata	humi_offset=0,temp_offset=0;
int	xdata	humi_offset_buf=0,temp_offset_buf=0;


uchar	data	data_buf=0;
uchar	pdata	rcv_buf[24],txd_buff[38];		//接收数据存储区,发送数据存储区
uchar	pdata	rxd_lengh=0,txd_lengh=0; //接收数据个数,发送的数据字节数
uchar	pdata	slave_addres=1;		//本机地址
uchar  	pdata   count_rcv_time=0,uart_count=0,uart_count2=0;
uint  	pdata	crc_value=0;
bit	uart_rx_flag=0,uart_tx_flag=0;

void Long_Delay ( void )
{
	unsigned char i,j;

	WatchDog();
	for ( i = 0; i < 255; i++ ) {
		for ( j = 0; j < 255; j++ );
	}
}

#include "24c04.h"
#include "key.c"
#include "fl602.c"
#include "th10.c"
#include "rs485.c"

void display ( void )
{
	if ( select_menu_flag ) {
		if ( select_mode == 1 ) {
			a[13]=0x0a;
			a[12]=0;//0x30;
			
			a[0]=0;
			a[1]=0;
			a[2]=0;
			a[3]=0;
		}

		if (select_mode == 2 ) {
			a[12]=0x08;
			a[13]=0x02;
			
			a[0]=0;
			a[1]=0;
			a[2]=0;
			a[3]=0;
		}
	} else {
		if (set_mode_flag) {
			if (set_mode==1) {
				if (flash_flag) {
					switch (baud) {
					case 0:
						a[0]=num[2];
						a[1]=num[4];
						a[2]=num[0];
						a[3]=num[0];
						break;
					case 1:
						a[0]=num[4];
						a[1]=num[8];
						a[2]=num[0];
						a[3]=num[0];
						break;
					case 2:
						a[0]=num[9];
						a[1]=num[6];
						a[2]=num[0];
						a[3]=num[0];
						break;
					case 3:
						a[0]=0;
						a[1]=num[1];
						a[2]=num[9]+0x10;
						a[3]=num[2];
						break;
					default: 
						break;
					}
				} else {
					a[0]=0;
					a[1]=0;
					a[2]=0;
					a[3]=0;
					flash_flag=1;
				}
				a[13]=0x0a;
				a[12]=0x00;
			}

			if (set_mode==2) {
				a[0]=0;
				if (flash_flag) {
					a[1]=num[address/100];
					a[2]=num[address%100/10];
					a[3]=num[address%100%10];
				} else {
					a[1]=0;
					a[2]=0;
					a[3]=0;
					flash_flag=1;
				}

				a[12]=0x08;
				a[13]=0x02;
			}
		} else {
			a[12]=0x30;
			a[13]=0x00;
			a[0]=0;
			a[1]=0;
			a[2]=0;
			a[3]=0;
		}
	}

	if(( set_offset_flag == 1 ) && ( set_temp_offset_flag == 0 )) {
		a[12]=0x10;
		a[13]=0x02;
	}

	if((set_offset_flag == 1 ) && ( set_temp_offset_flag == 1 )) {
		a[12]=0x20;
		a[13]=0x02;
	}

	if( uart_rx_flag ) {
		uart_count++;
		a[12] |= 0x01;
	} else {
		a[12] &= 0xf8;
	}

	if( uart_count > 5 ) {
		uart_rx_flag = 0;
		uart_count = 0;
		a[12] &= 0xf8;
	}

	if( uart_tx_flag ) {
		uart_count2++;
		a[12] |= 0x07;
	}

	if( uart_count2 > 5 ) {
		uart_tx_flag = 0;
		uart_count2 = 0;
		a[12] &= 0xf8;
	}

	WriteAll_1621(0,a,14); 
}

void main( void )
{
	unsigned char i;
	int noname_int;

	memset( a, 0xff, 14 );
    
	WatchDog();

	TMOD = 0x21;
	TH0 = 0xb7;
	TL0 = 0xff;
	TR0 = 1;       		// 启动计时器0
	ET0 = 1;       		// 开定时0中断  

	DE = 1;
	cs = 0; 			// 将端口设为输出
	SendCmd( BIAS ); 	// 设置偏压和占空比
	SendCmd( SYSEN );	// 打开系统振荡器
	SendCmd( LCDON );	// 打开LCD 偏压发生器

	WriteAll_1621( 0,a,14 );
	for (i = 0; i < 20; i++) {
	    WatchDog()
        delay5ms(100);
	}

    // 显示软件版本号
	memset( a, 0x00, 14 );
    a[11] = 0xc4;   // 显示u
	a[10] = 0x00;
	a[9] = num2[VERSION / 10] + 0x08;// 0x08为小数点
	a[8] = num2[VERSION % 10];
	WriteAll_1621(0, a, 14);
	for (i = 0; i < 5; i++) {
	    WatchDog()
        delay5ms(40);
	}

	// 读取EEPROM检验字，不相符初始化EEPROM
	i2c_readpage(ADR_FLASH_CHECK, 2, (unsigned char *)&noname_int);
	if (noname_int != FLASH_CHECK) {
	    I2cWriteInt( ADR_FLASH_BAUD, 2);
		I2cWriteInt( ADR_FLASH_ADDRESS, 1);
		I2cWriteInt( ADR_FLASH_HUMIOFFSET,0 );
		I2cWriteInt( ADR_FLASH_TEMPOFFSET,0 );
        noname_int = FLASH_CHECK;
		i2c_writepage(ADR_FLASH_CHECK, 2, (unsigned char *)&noname_int);
	}

	noname_int = I2cReadInt( ADR_FLASH_BAUD );
	if (( noname_int > 3 ) || ( noname_int < 0 )) {
	   baud = 2;
	    noname_int = baud;
	   I2cWriteInt( ADR_FLASH_BAUD, noname_int );
	} else {
		baud = ( noname_int & 0xff );
	}

	noname_int = slave_addres = I2cReadInt( ADR_FLASH_ADDRESS );
	if (( noname_int > 255 )  || ( noname_int < 0 )) {
		address = slave_addres = 1;
		noname_int = address;
		I2cWriteInt( ADR_FLASH_ADDRESS, noname_int );
	} else {
		address = slave_addres = noname_int;
	}

	humi_offset = I2cReadInt( ADR_FLASH_HUMIOFFSET );
	if ( ( humi_offset > 100) || ( humi_offset < -100 ) ) {
		humi_offset = 0;
		I2cWriteInt( ADR_FLASH_HUMIOFFSET,0 );
	}

	temp_offset = I2cReadInt( ADR_FLASH_TEMPOFFSET );
	if ( ( temp_offset > 100 ) || ( temp_offset < -100 ) ) {
		temp_offset = 0;
		I2cWriteInt( ADR_FLASH_TEMPOFFSET,0 );
	}

	for ( i = 0; i < 3; i++ ) {
		Long_Delay();
		Long_Delay();
		Long_Delay();
	}

	a[12] = 0x30;
	a[13] = 0x00;
	a[0] = 0;
	a[1] = 0;
	a[2] = 0;
	a[3] = 0;

	Initial_UART();

	WatchDog();

	EA = 1;

	while (1) {
		display();
        delay5ms(20);   // 100ms周期采集温湿度
		WatchDog();
		th10_main();
		key_service( scan_key() );
		WatchDog();
	}
}



void Timer0INT( void ) interrupt 1
{
	TR0 = 0;
	TH0 = 0xf2;
	TL0 = 0xff;
	TR0 = 1;
	
	if ( rxd_lengh != 0 ) {		//判断接收是否超时
		count_rcv_time++;
		if (count_rcv_time >= 2 ){
			rxd_lengh = 0;
			count_rcv_time = 0;
		}
	} else {
		count_rcv_time=0;
	}
	
}




#include "Math2.h"

extern void delay5ms(unsigned char n);
extern void delay5us(unsigned char n);


/* CRC 高位字节值表 */ 
static unsigned char code auchCRCHi[256] = { 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 
	
/* CRC低位字节值表*/ 
static char code auchCRCLo[256] = { 
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
}; 


unsigned short CRC16(unsigned puchMsg, unsigned int usDataLen) 
{ 
	unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */ 
	unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */ 
	unsigned uIndex ; /* CRC循环中的索引 */ 

	while (usDataLen--) { /* 传输消息缓冲区 */ 
		uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return (uchCRCHi << 8 | uchCRCLo) ;
}

void Initial_UART (void)
{
    //SCON    Bit:    7        6     5        4    3        2    1    0
    //        SM0/FE        SM1    SM2        REN    TB8    RB8      TI    RI
    SCON=0x50;        //0101,0000  8位可变baud,无奇偶效验
    PCON=0;

    if (baud==0) {
        TH1=BAUD2400;
        TL1=BAUD2400;
    }

    if (baud==1) {
        TH1=BAUD4800;
        TL1=BAUD4800;
    }

    if (baud==2) {
        TH1=BAUD9600;
        TL1=BAUD9600;
    }

    if (baud==3) {
        TH1=BAUD9600;
        TL1=BAUD9600;
        PCON=0x80;        //BUD 加倍
    }

    AUXR&=0xbf;        //定时器工作在I2T模式
    TR1=1;            //启动定时器1
    ES=1;
    EA=1;
}


void UART_Interrupt(void)interrupt 4 
{
	uint i = 0;

	if (0 == RI) {
		return ;
	}

	data_buf=SBUF;
	RI=0;
	TI=0;

	count_rcv_time=0;
	uart_rx_flag=1;
	uart_count=0;

	switch(rxd_lengh) {
	case 0:
		if (data_buf==slave_addres) {    //判断第一个字节是否与本机地址相等
			rcv_buf[0]=data_buf;
			rxd_lengh=1;
		} else {
			goto rest_out;
		}
	break;

    case 1:   
		if (data_buf==4) {    //判断第二个数据是否为4功能码
			rcv_buf[1]=data_buf;
			rxd_lengh=2;
		} else {
			goto rest_out;
		}
	break;

    case 2: 
		if (data_buf==0) {        //第三个数据＝00H？不等，就退出
			rcv_buf[2]=data_buf;
			rxd_lengh=3;
		} else {
			goto rest_out;
		}
	break;

	case 3: 
		rcv_buf[rxd_lengh]=data_buf;        // 保存SBUF到rcv_buf[rxd_lengh]位置
		rxd_lengh=4;                         // 数据长度＋1
	break;

	case 4: 
		rcv_buf[rxd_lengh]=data_buf; 
		rxd_lengh=5;  
	break;

	case 5: 
		rcv_buf[rxd_lengh]=data_buf; 
		rxd_lengh=6;  
	break;

	case 6: 
		rcv_buf[rxd_lengh]=data_buf; 
		rxd_lengh=7;  
	break;

	case 7: 
		rcv_buf[rxd_lengh]=data_buf; 
		goto    uart_rec_over; 
	break;

	default: 
		goto rest_out; 
	break;
	}

	return;

rest_out:
     
    rxd_lengh=0;   
    ES=1; 
    RI=0;
	TI=0; 

    return;

deal_error:
    ES=0;
    txd_buff[0]=slave_addres;
    txd_buff[1]=0x04;
    txd_buff[2]=0x04;
    txd_buff[3]=temp_buf>>8;
    txd_buff[4]=temp_buf;
    txd_buff[5]=humi_buf>>8;
    txd_buff[6]=humi_buf&0x0ff;
    crc_value=CRC16(txd_buff, 7);
    txd_buff[7]=crc_value>>8;
    txd_buff[8]=crc_value&0xff;
    txd_lengh=9;

    DE=0;                    //置位DE
    while(DE) {
		DE=0;
	};
    delay5ms(4);
    for (i=0;i<txd_lengh;i++) {
        SBUF=txd_buff[i];
        while(!TI);
        TI=0;
	}
    rxd_lengh=0;
    RI=0;
	TI=0; 
    ES=1;
    DE=1;
    return;

uart_rec_over:
    if (sensor_error >= 4) {
		goto deal_error;
	}
    ES=0;                   // 关“串口0”中断    
    uart_tx_flag=1;
    
    crc_value=CRC16(rcv_buf, 6);        //crc
    if (crc_value!=(rcv_buf[6] << 8 | rcv_buf[7])) {
        goto rest_out;
	}

    if (rcv_buf[1]!=4)        //检验功能码
        goto rest_out;

    if (rcv_buf[2]!=0)        //判数据地址高位
        goto rest_out;

    if (rcv_buf[3]>2)        //判数据地址低位,数据地址是否<=2?
        goto rest_out; 

    if (rcv_buf[4]!=0)        //判数据长度高位
        goto rest_out;

    if (rcv_buf[5]>2)        //判数据长度低位,数据长度<=2 ?
        goto rest_out; 

    txd_buff[0]=slave_addres;    //发送从机地址
    txd_buff[1]=0x04;            //发送从机功能码
    txd_buff[2]=rcv_buf[5]*2;    //发送数据字节LENGH
    
    txd_lengh=9;

    if (temp_buf<0) {
        txd_buff[3]=((0-temp_buf)>>8)|0x80;
        txd_buff[4]=(0-temp_buf)&0x0ff;
	} else {
        txd_buff[3]=temp_buf>>8;
        txd_buff[4]=temp_buf&0x0ff;
	}

    txd_buff[5]=humi_buf>>8;
    txd_buff[6]=humi_buf&0x0ff;

    crc_value=CRC16(txd_buff, 7);
    txd_buff[7]=crc_value>>8;        //crc值
    txd_buff[8]=crc_value&0xff;

    
    DE=0;                    //置位DE
    while(DE) {DE=0;};
    delay5ms(4);
    for (i=0;i<txd_lengh;i++) {
        SBUF=txd_buff[i];
        while(!TI);
        TI=0;
	}

    rxd_lengh=0;
    RI=0;
	TI=0; 
    ES=1;
    DE=1;
}



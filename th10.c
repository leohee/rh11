//#include <AT89x51.h>
#include <intrins.h> //Keil library (is used for _nop()_ operation)   
#include <math.h>    //Keil library   
#include <stdio.h>   //Keil library 

uchar code num[10]={0xeb,0x60,0xc7,0xe5,0x6c,0xad,0xaf,0xe0,0xef,0xed};
uchar code num1[10]={0xbe,0x06,0x7c,0x5e,0xc6,0xda,0xfa,0x0e,0xfe,0xde};
uchar code num2[10]={0xd7,0x06,0xe3,0xa7,0x36,0xb5,0xf5,0x07,0xf7,0xb7};

int xdata humi_val_buff[8]={555,555,555,555,555,555,555,555},humi=555,humi_buf=555;
int xdata temp_val_buff[8]={250,250,250,250,250,250,250,250},temp=250,temp_buf=250;

uchar xdata count_ad1=0,count_ad2=0;

typedef value union {
	unsigned int i; 
	int f; 
};

value humi_val, temp_val; 

unsigned char error,checksum; 

unsigned char xdata sensor_error = 0;   // 指示出错并计数
  
enum {TEMP,HUMI}; 

sbit         data_reverse = P3^5;

sbit         HUM_D0 = P2^6;
sbit         TEMP_D1 = P2^7;
#define noACK 0 
#define ACK   1 
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 

#define MILLION 1000000


bit Crc8_check (bit T_H,uchar low_crc,uchar hig_crc)
{
    uchar crc;
    uchar crc1,crc2;
    uchar i;
    if (T_H == TEMP) {    //T_H == TEMP
        crc  = 0x53;        //对命令字节校验完后的结果（读温度）
        crc1 = low_crc;
        crc2 = hig_crc;
    } else {    //T_H == HUMI
        crc  = 0xf5;        //对命令字节校验完后的结果（读湿度）
        crc1 = low_crc;
        crc2 = hig_crc;
    }

    for (i=0;i<8;i++) {
        if ((crc1 & 0x80)==(crc & 0x80)) {
            crc <<= 1;
        } else {
            crc <<= 1;
            crc ^= 0x30;
            crc += 0x01;
        }
        crc1 <<= 1;
    }

    for (i=0;i<8;i++) {
        if ((crc2 & 0x80)==(crc & 0x80)) {
            crc <<= 1;
        } else {
            crc <<= 1;
            crc ^= 0x30;
            crc += 0x01;
        }
        crc2 <<= 1;
    }

    for (i=0;i<8;i++) {   //crc颠倒顺序后成为crc1
        crc1 >>= 1;
        if(crc & 0x80)
            crc1|=0x80;
        crc <<= 1;
    }

    if (crc1 == checksum) {
		return 1;
	} else {
		return 0;
	}
}

char s_write_byte(unsigned char value) 
{  
	unsigned char i,error=0;   
	for (i=0x80;i>0;i/=2) {             //shift bit for masking 
		if (i & value) DATA=1;          //masking value with i , write to SENSI-BUS 
		else DATA=0;                         

		SCK=1;                          //clk for SENSI-BUS 
		_nop_();_nop_();_nop_();        //pulswith approx. 5 us      
		SCK=0; 
	}

	DATA=1;                           //release DATA-line 
	SCK=1;                            //clk #9 for ack  
	error=DATA;                       //check ack (DATA will be pulled down by SHT11) 
	SCK=0;         
	return error;                     //error=1 in case of no acknowledge 
} 

char s_read_byte (unsigned char ack) 
{  
	unsigned char i,val=0; 
	DATA=1;                           //release DATA-line 
	for (i=0x80;i>0;i/=2) {            //shift bit for masking 
		SCK=1;                          //clk for SENSI-BUS 
		if (DATA) val=(val | i);        //read bit   
		SCK=0;                   
	}

	DATA=!ack;                        //in case of "ack==1" pull down DATA-Line 
	SCK=1;                            //clk #9 for ack 
	_nop_();_nop_();_nop_();          //pulswith approx. 5 us  
	SCK=0;                       
	DATA=1;                           //release DATA-line 

	return val; 
} 

void s_transstart (void)
{
// generates a transmission start  
//       _____         ________ 
// DATA:      |_______| 
//           ___     ___ 
// SCK : ___|   |___|   |______ 

	DATA=1;
	SCK=0;                   //Initial state 
	_nop_(); 
	SCK=1; 
	_nop_(); 
	DATA=0; 
	_nop_(); 
	SCK=0;   
	_nop_();_nop_();_nop_(); 
	SCK=1; 
	_nop_(); 
	DATA=1;          
	_nop_(); 
	SCK=0;          
} 

// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart 
//       _____________________________________________________         ________ 
// DATA:                                                      |_______| 
//          _    _    _    _    _    _    _    _    _        ___     ___ 
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______ 
void s_connectionreset(void) 
{   
	unsigned char i;

	DATA=1;
	SCK=0;                    //Initial state 
	for(i=0;i<9;i++) {                  //9 SCK cycles 
		SCK=1; 
		SCK=0; 
	} 
	s_transstart();                   //transmission start 
} 
                     

char s_measure (unsigned char *p_value, unsigned char *p_checksum, unsigned char mode) 
{  
	unsigned error=0; 
	unsigned int i; 

	s_transstart();                   //transmission start 
	switch(mode) {                     //send command to sensor 
	case TEMP   : error+=s_write_byte(MEASURE_TEMP); break; 
	case HUMI   : error+=s_write_byte(MEASURE_HUMI); break; 
	default     : break;     
	}

	for (i=0;i<65535;i++) {
		if(DATA==0) break; //wait until sensor has finished the measurement 
	}

	if (DATA) {                // or timeout (~2 sec.) is reached 
		error+=1;
	}

	*(p_value)  =s_read_byte(ACK);    //read the first byte (MSB) 
	*(p_value+1)=s_read_byte(ACK);    //read the second byte (LSB) 
	*p_checksum =s_read_byte(noACK);  //read checksum 

	if (Crc8_check(mode,*(p_value), *(p_value+1))==0) {
    	error+=1;
	}

	return error; 
} 


void calc_sth11 (int *p_humidity ,int *p_temperature) 
{
	const long C1=-40000000;              // for 12 Bit
	const long C2=+405000;           // for 12 Bit
	const long C3=-28;        // for 12 Bit
	const long T1=+100000;             // for 14 Bit @ 5V
	const long T2=+800;           // for 14 Bit @ 5V    

	long rh=*p_humidity;             /* rh:      Humidity [Ticks] 12 Bit */
	long t=*p_temperature;           /* t:       Temperature [Ticks] 14 Bit    */
	long rh_lin;                     /* rh_lin:  Humidity linear    */
	long rh_true;                    /* rh_true: Temperature compensated humidity    */
	long t_C;                        /* t_C   :  Temperature [癈]    */

	t_C=t*1 - 3960;                  /*calc. temperature from ticks to [癈]    */

	rh_lin=C3*rh*rh + C2*rh + C1;     /*calc. humidity from ticks to [%RH]    */
	rh_true=(t_C-2500)*(T1+T2*rh)/100+rh_lin;   /*calc. temperature compensated humidity [%RH]    */

	t_C=t_C/10;
	rh_true=rh_true/MILLION;

	if(rh_true>1000)rh_true=1000;       /*cut if the value is outside of    */
	if(rh_true<1)rh_true=1;       /*the physical possible range    */

	*p_temperature=t_C;               /*return temperature [癈]    */
	*p_humidity=rh_true;              /*return humidity[%RH]    */
} 

void th10_main (void)
{ 
    static bit first_time_flag = 1;  // indicate the first time to caculate humidity/temperature
    unsigned char i; 

    s_connectionreset(); 

    error=0; 
    error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity 
    error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature 

    if (error!=0) {
        s_connectionreset();                 //in case of an error: connection reset
        if (sensor_error < 4)sensor_error++;
    } else { 
        humi_val.f=humi_val.i; 
        temp_val.f=temp_val.i;  
        calc_sth11(&humi_val.f,&temp_val.f);            //calculate humidity, temperature 
    } 

    humi_val.f=humi_val.f+humi_offset;
    temp_val.f=temp_val.f+temp_offset;
    if (first_time_flag) {   // 第一次采集温湿度值，直接置成实际值
        humi_buf = humi_val.f;
        temp_buf = temp_val.f;
        first_time_flag = 0;
    }

    if ((abs(temp_val.f - temp_buf) > 200)    // 温度跳变20度认为出错
      || (abs(humi_val.f - humi_buf) > 400)) {       // 湿度跳变40%认为出错
        error++;
        if(sensor_error < 4) {  //非正常次数最多为4次
            sensor_error++;
        }
        first_time_flag = 1;
        count_ad1=0;
        count_ad2=0;
    } else {
        if (sensor_error) {
            sensor_error--;
        }
    }

    if (!sensor_error) {  // 错误值不更新缓存,
        humi_val_buff[count_ad1++] = humi_val.f;  
    }

    if (!sensor_error) {   // 错误值不更新缓存
        temp_val_buff[count_ad2++] = temp_val.f;
    }

    if (count_ad1>7) {  // 采样值 次
        count_ad1=0;
        humi = humi_buf;
        for (i = 0; i < 8; i++) {
            humi += humi_val_buff[i];
        }
        humi /= 9;
        humi_buf = humi;
    } else {
        humi=humi_buf;
    }

    if (count_ad2>7) {
        count_ad2=0;
        temp = temp_buf;
        for (i = 0; i < 8; i++) {
            temp += temp_val_buff[i];
        }
        temp /= 9;
        temp_buf = temp;
    } else {
        temp=temp_buf;
    }

    if(humi>=1000) {
        humi=humi_buf=999;
    }

    if(humi<0) {
        humi=humi_buf=0;
    }

    if(temp<-400) {
        error+=1;
        if (sensor_error < 4) {
            sensor_error++;
        }
    }

    if(temp<0) {
        temp=0-temp;
    }

    if(temp>1200) {
        error+=1;
        if (sensor_error < 4) {
            sensor_error++;
        }
    }

    if (select_menu_flag!=1&&set_mode_flag!=1&&set_offset_flag==0) {
        a[11]=num2[temp/1000];
        a[10]=num2[(temp%=1000)/100];
        a[9]=(num2[(temp%=100)/10]+0x08);
        a[8]=num2[temp%10];

        a[7]=num1[humi/1000];
        a[6]=num1[(humi%=1000)/100];
        a[5]=num1[(humi%=100)/10]+1;
        a[4]=num1[humi%10];

        if(a[11]==0xd7) {
            a[11]=0;
            if(a[10]==0xd7) {
                a[10]=0;
            }
        }
        
        if (a[7]==0xbe) {
            a[7]=0;
            if (a[6]==0xbe) {
                a[6]=0;
            }
        }
        
        if (temp_buf<0) {
            a[11]=0x20;
        }

        if (error!=0) {
            a[11]=0x20;
            a[10]=0x20;
            a[9]=0x20;
            a[8]=0x20;

            a[7]=0x40;
            a[6]=0x40;
            a[5]=0x40;
            a[4]=0x40;

        }
    } else {
        a[11]=0;
        a[10]=0;
        a[9]=0;
        a[8]=0;

        a[7]=0;
        a[6]=0;
        a[5]=0;
        a[4]=0;
    }

    if (set_offset_flag&&set_temp_offset_flag==0) {

        if (humi_offset<0) {
            a[7]=0x40;
            humi_offset_buf=0-humi_offset;
        } else {
            a[7]=0;
            humi_offset_buf=humi_offset;
        }
        a[6]=num1[(humi_offset_buf%=1000)/100];
        a[5]=num1[(humi_offset_buf%=100)/10]+1;
        a[4]=num1[humi_offset_buf%10];

        a[11]=0;
        a[10]=0;
        a[9]=0;
        a[8]=0;
        
        if(a[6]==0xbe)    a[6]=0;
    }

    if (set_offset_flag&&set_temp_offset_flag==1) {
        a[7]=0;
        a[6]=0;
        a[5]=0;
        a[4]=0;

        if(temp_offset<0) {
            a[11]=0x20;
            temp_offset_buf=0-temp_offset;
        } else {
            a[11]=0;
            temp_offset_buf=temp_offset;
        }

        a[10]=num2[(temp_offset_buf%=1000)/100];
        a[9]=(num2[(temp_offset_buf%=100)/10]+0x08);
        a[8]=num2[temp_offset_buf%10];

        if (a[10]==0xd7) {
			a[10]=0;
		}
    }
}




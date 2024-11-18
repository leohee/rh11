
//	标题: CAT24C02驱动程序 , 跟AT24C02是不同

#ifndef __24C02_H__
#define __24C02_H__


void i2c_start( void )
{
	SDA = 1;
	_nop_();
 	_nop_();
	SCL = 1;
 	_nop_();
 	_nop_();
 	SDA = 0;
 	_nop_();
 	_nop_();
 	SCL = 0;
 	_nop_();
 	_nop_();
}


void i2c_stop( void )
{
	SDA = 0;
 	_nop_();
 	_nop_();
 	SCL = 1;
 	_nop_();
 	_nop_();
 	SDA = 1;
 	_nop_();
 	_nop_();
}


void i2c_ack( void )
{
	SDA = 0;
	_nop_();
	_nop_();
 	SCL = 1;
 	_nop_();
 	_nop_();
	SCL = 0;
 	_nop_();
 	_nop_();
 	SDA = 1;
 	_nop_();
 	_nop_();
}									 


void i2c_init( void )
{
	SCL = 0;
	_nop_();
	_nop_();
	i2c_stop();	
}

//----------------------------------------
//	功能:发送8位(主循环体调用)
//	入口:待发字节
//	出口:成功返回真失败返回假
//	说明:上层程序不单独调用本函数
//----------------------------------------
unsigned char i2c_send8bit(unsigned char dat)  
{
	unsigned char i;
	unsigned char sample;

	Dog();		
 	for( i = 0; i < 8; i++ )
  	{	
		SDA = ( dat & 0x80 );	
   		dat <<= 1;
   		_nop_();
		SCL = 1;
 		_nop_();
 		_nop_();
		SCL=0;
 		_nop_();
 		_nop_();
   	}
 	SDA = 1;
	SCL = 1;
 	_nop_();
 	_nop_();
	sample = SDA;
	SCL = 0;
	_nop_();
 	_nop_();

 	return(!sample);      			
}

//----------------------------------------
//	功能:接收8位(主循环体调用)
//	出口:数据
//	说明:上层程序不单独调用本函数
//----------------------------------------
unsigned char i2c_receive8bit( void ) 
{
	unsigned char i;
 	unsigned char dat = 0;
	unsigned char sample;

	Dog();		
	for( i = 0; i < 8; i++ )
  	{	
		dat <<= 1;
		SCL = 1;
	 	_nop_();
	 	_nop_();
		sample = SDA;
		SCL = 0;
 		_nop_();
 		_nop_();
   		if( sample )
   			dat++;
  	}
 	return( dat );
}

//----------------------------------------
//	功能:页面读出
//	入口:1.地址2.字节数3.数据指针
//	出口:成功返回真失败返回假
//	说明:地址应保证在同一页面下
//----------------------------------------
void i2c_readpage(unsigned char adr,unsigned char size,unsigned char *dat)  
{
	unsigned char i;

	Dog();
	i2c_init();
	i2c_start();
	i2c_send8bit( 0xa0 ); 
	i2c_send8bit(adr);
	Dog();
	i2c_start();
	i2c_send8bit( 0xa1 );
 	for( i = 0; i < ( size - 1 ); i++ )								
  	{
		Dog();
		*( dat + i ) = i2c_receive8bit();
		i2c_ack();  
   	}
	*( dat + i ) = i2c_receive8bit();
 	i2c_stop();
}

//----------------------------------------
//	功能:页面写入(包含延时)
//	入口:1.地址2.字节数3.数据指针
//	出口:成功返回真失败返回假
//	说明:地址应保证在同一页面下
//----------------------------------------
void i2c_writepage(unsigned char adr,unsigned char size,unsigned char *dat) 	
{	
	unsigned char i;
	unsigned long delay;

	Dog();
	i2c_init();				 						//写
	i2c_start();
	i2c_send8bit(0xa0);
	i2c_send8bit(adr);
	for( i = 0; i < size; i++ )
	{	
		Dog();
		i2c_send8bit(*(dat+i));
		_nop_();
	}
	i2c_stop();	
	for( delay = 0;delay < 4000; delay++ ) Dog();
}

//=============================================
//	Description: Write int to flash..
//	Para: address,data
//	return: none
//=============================================
void I2cWriteInt( unsigned int address, int value )
{
    unsigned char buf[6];

    if( value < 0 )
    {
        buf[0] = '-';
        value = 0 - value;
    }
    else
    {
        buf[0] = '+';
    }

	buf[1] = ( value / 10000 ) + '0';
    value %= 10000;
    buf[2] = ( value / 1000 ) + '0';
    value %= 1000;
    buf[3] = ( value / 100 ) + '0';
	value %= 100;
    buf[4] = ( value / 10 ) + '0';
    value %= 10;
    buf[5] = value + '0';

    i2c_writepage( address,6,buf );
}

//=============================================
//	Description: Read int from flash..
//	Para: address
//	return: none 0x8000 is wrong
//=============================================
int I2cReadInt( unsigned int address )
{
    unsigned char i;
    int value;
    unsigned char buf[6];

	i2c_readpage( address,6,buf );

  	for( i = 1; i < 6; i++ )
    {
        if( ( buf[i] > '9' ) || (  buf[i] < '0' ) )
  			return( 32767 );
    }

    value = buf[5] - '0';
    value += ( buf[4] - '0' ) * 10;
    value += ( buf[3] - '0' ) * 100;
	value += ( buf[2] - '0' ) * 1000;
    value += ( buf[1] - '0' ) * 10000;
    
    switch( buf[0] )
    {
    case '-':
        value = 0 - value;
        break;
    case '+':
        break;
    default:
        return( 32767 );
    }

    return( value );
}

#endif



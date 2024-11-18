#ifndef __MATH_H__
#define __MATH_H__

void Delay_us(unsigned char nValue)
{
	unsigned char i;
    
    for( i = 0; i < nValue; i++ )
    {
        WatchDog();
  		_NOP();
    }
}


void Delay_ms( unsigned int wLoops )
{
	unsigned int wTemp;

    while (wLoops--)
	{
		Dog();
	    wTemp = 570;	
        while (wTemp--);
    }
}


#define ShortDelay( )	Delay_us( 100 )


#endif


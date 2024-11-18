
//#define HUMI_ADJUST_EXT

sbit k1=P2^0;
sbit k2=P1^6;
sbit k3=P1^7;
sbit k4=P2^1;

#define menu_key		1
#define up_key 			2
#define down_key		3
#define enter_key		4

bit	menu_key_flag = 0,up_key_flag = 0,down_key_flag = 0,enter_key_flag = 0;
unsigned char newkey;

char scan_key (void)
{
	unsigned char  i;

	newkey = 0;
	if ( k1 == 0 ) {
		for ( i = 0; i <= 50; i++ );
		if (( k1 == 0 ) && ( menu_key_flag == 0 )) {
			newkey = menu_key;
			menu_key_flag = 1;
		} else {
			newkey = 0;
		}
	} else {
		menu_key_flag = 0;
	}

	if ( k2 == 0 ) {
		for ( i = 0; i <= 50; i++ );
		if (( k2 == 0 ) && ( up_key_flag == 1 )) {
			key_time_cont2++;
		} else {
			key_time_cont2=0;
		}

		if (( k2 == 0 ) && ( up_key_flag == 0 )) {
			newkey = up_key;
			up_key_flag = 1;
		} else {
			newkey = 0;
		}
	} else {
		up_key_flag = 0;
		key_time_cont2 = 0;
	}

	if (key_time_cont2 > 8){
		key_time_cont3++;
		if ( key_time_cont3 == 1 ) {
			newkey= up_key;
			key_time_cont3=0;
		}
		key_time_cont2 = 21;
	}

	if ( k3 == 0 ) {
		for ( i = 0; i <= 50; i++ );
		if (( k3 == 0 ) && ( down_key_flag == 1 )) {
			key_time_cont++;
		} else {
			key_time_cont = 0;
		}

		if (( k3 == 0 ) && ( down_key_flag == 0 )) {
			newkey = down_key;
			down_key_flag = 1;
		} else {
			newkey = 0;
		}
	} else {
		down_key_flag = 0;
		key_time_cont = 0;
	}

	if ( key_time_cont > 8 ) {
		key_time_cont3++;
		if (key_time_cont3==1) {
			key_time_cont3 = 0;
			newkey = down_key;
		}
		key_time_cont = 21;
	}

	if ( k4 == 0 ) {
		for ( i = 0; i <= 50; i++);
		if (k4==0&&enter_key_flag==0) {
			newkey= enter_key;
			enter_key_flag=1;
		} else {
			newkey=0;
		}
	} else {
		enter_key_flag=0;
		//newkey=0;
	}

	if (k4==0&&enter_key_flag==1&&set_offset_flag==0
	  &&select_menu_flag==0&&set_mode==0&&select_mode==0) {
		count_time3++;
	} else {
		count_time3=0;
	}

	if (count_time3==34) {
		count_time3=0;
		set_offset_flag=1;
	}

	if (newkey==0) {
		count_time++;
	} else {
		count_time=0;
		count_time2=0;
	}
	
	if (count_time==34) {
		count_time2++;
		count_time=0;
	}
	
	if (count_time2>1) {
		count_time=0;
		count_time2=0;
		select_menu_flag=0;
		set_mode_flag=0;
		select_mode=0;
		set_mode=0;
		set_offset_flag=0;
		set_temp_offset_flag=0;
	}

	return	newkey;
}

void key_service (char current_key)
{
	switch (current_key) {
	case 1 :
		if (select_menu_flag==0&&set_mode==0&&set_offset_flag==0) {
			select_menu_flag=1;
			select_mode=1;
			break;
		}

		if(select_menu_flag==1&&set_mode==0&&set_offset_flag==0) {
			select_menu_flag=0;
			select_mode=0;
			break;
		}

		if (set_mode!=0) {
			select_menu_flag=1;
			set_mode_flag=0;
			set_mode=0;
		}

		if (set_offset_flag==1) {
			set_offset_flag=0;
			set_temp_offset_flag=0;
		}
	break;

	case 2 :
		if (select_menu_flag==1) {
			select_mode++;
			if (select_mode==3) {
				select_mode=1;
			}
			break;
		}

		if (set_mode==1) {
			baud++;
			if(baud==4)
				baud=0;
			break;
		}

		if (set_mode==2) {
			address++;
			if(address==0)
				address=1;
		}

		if (set_offset_flag==1&&set_temp_offset_flag==0) {
			humi_offset+=1;
#ifndef HUMI_ADJUST_EXT
			if(humi_offset>100)
				humi_offset=-100;
#else
			if (humi_offset>500)
				humi_offset=-500;
#endif
		}

		if (set_offset_flag==1&&set_temp_offset_flag==1) {
			temp_offset+=1;
			if (temp_offset>100)
				temp_offset=-100;
		}
	break;

	case 3 :
		if (select_menu_flag==1) {
			if (select_mode==1)
				select_mode=2;
			else
				select_mode--;
			break;
		}

		if (set_mode==1) {
			if(baud==0)
				baud=3;
			else
				baud--;
			break;
		}

		if (set_mode==2) {
			if(address==1)
				address=255;
			else
				address--;
		}

		if (set_offset_flag==1&&set_temp_offset_flag==0) {
			humi_offset-=1;
#ifndef HUMI_ADJUST_EXT
			if(humi_offset<-100)
				humi_offset=100;
#else
			if(humi_offset<-500)
				humi_offset=500;	
#endif
		}

		if(set_offset_flag==1&&set_temp_offset_flag==1) {
			temp_offset-=1;
			if(temp_offset<-100)
				temp_offset=100;
		}
	break;

	case 4 :
		if (select_menu_flag==1) {
			select_menu_flag=0;
			set_mode=select_mode;
			set_mode_flag=1;

			baud = I2cReadInt( ADR_FLASH_BAUD );
			address = I2cReadInt( ADR_FLASH_ADDRESS );
			break;
		}

		if (set_mode==1) {
			flash_flag=0;
			I2cWriteInt( ADR_FLASH_BAUD, baud );
			if (baud==0) {
				TH1=BAUD2400;
				TL1=BAUD2400;
				PCON=0x00;
			}

			if(baud==1) {
				TH1=BAUD4800;
				TL1=BAUD4800;
				PCON=0x00;
			}

			if(baud==2) {
				TH1=BAUD9600;
				TL1=BAUD9600;
				PCON=0x00;
			}

			if (baud==3) {
				TH1=BAUD9600;
				TL1=BAUD9600;
				PCON=0x80;
			}
			break;
		}

		if (set_mode==2) {
			slave_addres=address;
			I2cWriteInt( ADR_FLASH_ADDRESS, address );
			flash_flag=0;
		}

		if (set_offset_flag&&set_temp_offset_flag==0) {
			set_temp_offset_flag=1;
			I2cWriteInt( ADR_FLASH_HUMIOFFSET, humi_offset );
			break;
		}

		if (set_offset_flag&&set_temp_offset_flag==1) {
			set_temp_offset_flag=0;
			I2cWriteInt(ADR_FLASH_TEMPOFFSET, temp_offset);
		}
	break;

	default :
		break;
	}
}


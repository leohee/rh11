
#define BIAS 0x52
#define SYSEN 0x02
#define LCDON 0x06
#define LCDOFF 0x04

#define dot 0x01

sbit cs=P1^5;
sbit rd=P1^4;
sbit wr=P1^3;
sbit dat=P1^2;

// data 的高cnt 位写入HT1621，高位在前
void SendBit_1621 (uchar datas, uchar cnt)
{
	uchar i;
	for (i = 0; i < cnt; i ++) {
		if ((datas&0x80)==0) dat = 0;
		else dat = 1;

		wr = 0;
		_nop_();
		wr = 1;
		datas <<= 1;
	}
}

// datas 的低cnt 位写入HT1621，低位在前
void SendDataBit_1621 (uchar datas, uchar cnt)
{
	uchar i = 0;

	for (i = 0; i < cnt; i ++) {
		if ((datas&0x01)==0) dat = 0;
		else dat = 1;

		wr = 0;
		_nop_();
		wr = 1;
		datas >>= 1;
	}
}
void SendCmd (uchar command)
{
	cs = 0;
	SendBit_1621(0x80,4); //写入标志码“100”和9 位command 命令，由于
	SendBit_1621(command,8); //没有使有到更改时钟输出等命令，为了编程方便
	cs = 1; //直接将command 的最高位写“0”
}

void WriteAll_1621 (uchar addr, uchar *p, uchar cnt)
{
	uchar i = 0;

	cs = 0;
	SendBit_1621(0xa0, 3); //写入标志码“101”
	SendBit_1621(addr, 6); //写入addr 的高6 位

	for(i = 0; i < cnt; i ++, p++) { //连续写入数据
		SendDataBit_1621(*p,8);
	}

	cs = 1;
}



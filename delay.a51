;////////////////////////////////////////////////
;//	标题:精确延时程序
;//	芯片:STC12C5410
;//	晶体:3.6864M
;//	版本:V2.0
;//	作者:Jack Chung
;//	日期:2004-04-20 
;////////////////////////////////////////////////
	?PR?_delay5ms?rh11rs18b20  SEGMENT CODE 
	PUBLIC	_delay5ms
;////////////////////////////////////////////////
	RSEG  ?PR?_delay5ms?rh11rs18b20
_delay5ms:
	PUSH	0X00			; 2
	PUSH	ACC				; 2
	PUSH	PSW				; 2
	MOV		A,R7			; 1
	JNZ		D50MS_0			; 2
	LJMP	RETURN50
D50MS_0:
	MOV     R0,#220			; 1
D50MS_2:
	MOV		0XE1,#0X3E		; dog 2
	NOP
	NOP
	NOP
	DJNZ    R0,D50MS_2		; 2
	DJNZ    R7,D50MS_0		; 2
RETURN50:
	POP		PSW				; 2
	POP		ACC				; 2
	POP		0X00			; 2
	RET
;////////////////////////////////////////////////
	?PR?_delay5us?rh11rs18b20  SEGMENT CODE 
	PUBLIC	_delay5us
;////////////////////////////////////////////////
	RSEG  ?PR?_delay5us?rh11rs18b20
_delay5us:
	PUSH	ACC
	PUSH	PSW
	MOV		A,R7
	JNZ		D5MS_0
	LJMP	RETURN5
D5MS_0:
	NOP
	NOP
	NOP
	DJNZ    R7,D5MS_0
RETURN5:
	POP		PSW
	POP		ACC
	RET
;////////////////////////////////////////////////
	END

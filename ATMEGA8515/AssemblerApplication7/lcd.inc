;Header file for anything LCD related. This comes prebuilt with the LCD initialization routine, as well as some special delays and different output sbrs.

lcd_start:			;This sbr will init the LCD in 8bit, 2 line mode, and will clear the display as well.
	push r16			;Pushing r16 to save its state in case it is important
	rcall delay40ms
	ldi r16, $38
	sts	$2000, r16		;Over here, our memory has been split for memory address $2000, where
						;$2000 to $20FF is the DR and $2100 to 21FF is IR
	rcall delay37us
	ldi r16, $38
	sts	$2000, r16

	rcall delay37us
	ldi r16, $0F
	sts	$2000, r16 ;DISPLAY ON

	rcall delay37us
	ldi r16, $01
	sts	$2000, r16 ;CLEAR DISPLAY

	rcall delay1_52ms
	ldi r16, $06
	sts	$2000, r16 ;ENTRY MODE SET

	rcall delay37us
	pop r16
	ret 

delay37us:			;since each clockcycle is taken into account to make a 37us delay,
	NOP				;I counted the clocks of rcall and ret, and padded with NOPs :D lazy i know
	NOP				;but it works perfectly!
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

	ret

;Below are some precise timing delay functions that are not only required for LCD init
;but could also come in use for timing

delay1_52ms:
	push r18
	clr r18
delay_loop1_52ms:
	rcall delay37us
	inc r18
	cpi r18, 41			;1520us/37us is roughly 41
	breq fini_1_52ms
	rjmp delay_loop1_52ms
fini_1_52ms:
	pop r18
	ret

delay40ms:
	push r18
	clr r18
delay_loop40ms:
	rcall delay1_52ms
	inc r18
	cpi r18, 26			;40ms/1.5ms is roughly 26
	breq fini_40ms
	rjmp delay_loop40ms
fini_40ms:
	pop r18
	ret

delay1ms:
	push r18
	clr r18
delay_loop1ms:
	rcall delay37us
	inc r18
	cpi r18, 27			;1000us/37us is roughly 27
	breq fini_1ms
	rjmp delay_loop1ms
fini_1ms:
	pop r18
	ret

;clcd_outs1 will output a message to the first LCD line while keeping the second intact
;The following message buffer templates must be followed for it to work in CSEG
;***Before calling it, use Z index for the message buffer***
;
;	buff1: .db "First LCD line", $A0
;
;clcd_outs2 will output a message to the second LCD line while keeping the first intact
;The following message buffer template must be followed for it to work in CSEG
;***Before calling it, use Z index for the message buffer***
;
;	buff2: .db "Scnd LCD line", $A0
;
;$A0 is the termination character
;
clcd_outs1:				
	push r17
	ldi r17, $80
	sts $2000, r17
	rcall delay1ms
clcd_outs_loop1:
	lpm r17, Z+
	sts $2100, r17
	rcall delay1ms
	cpi r17, $A0
	brne clcd_outs_loop1
	pop r17
	ret

clcd_outs2:
	push r17
	ldi r17, $C0
	sts $2000, r17
	rcall delay1ms
clcd_outs_loop2:
	lpm r17, Z+
	sts $2100, r17
	rcall delay1ms
	cpi r17, $A0
	brne clcd_outs_loop2
	pop r17
	ret

clcd_outs_manual:			;Must Push R17 with appropriate custom LCD control signals to use this funtion
	rcall delay1ms
clcd_outs_manual_loop:
	lpm r17, Z+
	sts $2100, r17
	rcall delay1ms
	cpi r17, $A0
	brne clcd_outs_manual_loop
	ret

clcd_outs_commanual:			;This function will send a control sig to the LCD				
	rcall delay1ms			;Must Push R17 & Load R17 with appropriate custom LCD control signals to use this funtion
clcd_outs_commanual_loop:
	sts $2000, r17
	rcall delay1ms
	ret

clcd_outs_xmanual:			;This function will output a single char 				
	rcall delay1ms			;Must Push R17 & Load R17 with appropriate custom LCD control signals to use this funtion
clcd_outs_xmanual_loop:
	sts $2100, r17
	rcall delay1ms
	ret

clearlcd:
	push r16
	ldi r16, $01
	sts	$2000, r16 ;CLEAR DISPLAY
	rcall delay1_52ms
	pop r16
	ret


delay_4s:
	push r18
	ldi R18, 10
second1:
	ldi r25, $ff
	ldi r24, $ff
second2:
	sbiw r25:r24, 1
	brne second2
	dec r18
	breq sec_fini
	rjmp second1
sec_fini:
	pop r18
	ret
	
lookup: .db $30, $31, $32, $33, $34, $35, $36, $37, $38, $39, $2A, 0

lcd_lookup:
	lds r17, IN1
	cpi r17, NUM0
	breq numb0
	cpi r17, NUM1
	breq numb1
	cpi r17, NUM2
	breq numb2
	cpi r17, NUM3
	breq numb3
	cpi r17, NUM4
	breq numb4
	cpi r17, NUM5
	breq numb5
	cpi r17, NUM6
	breq numb6
	cpi r17, NUM7
	breq numb7
	cpi r17, NUM8
	breq numb8
	cpi r17, NUM9
	breq numb9
	cpi r17, POUND
	breq numbp
	ldi r17, '?'
	ret

numb0:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	lpm r17, Z
	ret
numb1:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 1
	lpm r17, Z
	ret
numb2:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 2
	lpm r17, Z
	ret
numb3:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 3
	lpm r17, Z
	ret
numb4:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 4
	lpm r17, Z
	ret
numb5:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 5
	lpm r17, Z
	ret
numb6:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 6
	lpm r17, Z
	ret
numb7:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 7
	lpm r17, Z
	ret
numb8:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 8
	lpm r17, Z
	ret
numb9:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 9
	lpm r17, Z
numbp:
	ldi r31, high(lookup<<1)
	ldi r30, low(lookup<<1)
	adiw r31:r30, 10
	lpm r17, Z


.exit
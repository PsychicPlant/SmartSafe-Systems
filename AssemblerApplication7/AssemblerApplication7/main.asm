
.dseg

.equ CMD_BUFF_MAX = 4

.equ HASH = $F6
.equ S1 = $FB
.equ S2 = $FD
.equ S3 = $FE
.equ NUM1 = $BB
.equ NUM2 = $BD
.equ NUM3 = $BE
.equ NUM4 = $DB
.equ NUM5 = $DD
.equ NUM6 = $DE
.equ NUM7 = $EB
.equ NUM8 = $ED
.equ NUM9 = $EE
.equ POUND = $F3
.equ NUM0 = $F5

.org $90
FLAG0: .byte 1
IN1: .byte 1
IN2: .byte 1
CMD: .byte 1
TRIES: .byte 1

.org $105
key_buff: .byte 3

.org $110
tx_buff: .byte CMD_BUFF_MAX

.org $115
rx_buff: .byte CMD_BUFF_MAX

.cseg
reset:  rjmp init       ;used as an entry point for the reset
int_isr0: rjmp isr0

.org  $32

;------
;Initializing stack
;------

init:

	ldi r16, LOW(RAMEND)
	out SPL, r16
	ldi r16, HIGH(RAMEND)
	out SPH, r16

	rcall init_uart	
	rcall tx_buff_init
	rcall rx_buff_init

init_ports:			
	;--------------------------------
	;Activaring necessary pins for reading and writting to PORTB in order to SCAN. 
	;!!IMPORTANT!! This is dependend on the pin mapping of the keypad used
	;--------------------------------
	//ldi R18, $54	
	ldi R18, $07
	out DDRB, R18 
	ldi r18, $FF
	out PORTB, r18
	ldi R18, $FF		;Activating PORTE for full output.
	out DDRE, R18		;Will serve as switch for solenoid relay.
	ldi r18, $00
	out PORTE, r18
			
banner:
	ldi r31, HIGH(msg_welcome<<1)
	ldi r30, LOW(msg_welcome<<1)
	rcall putz

init_vars:
	ldi r16, $0
	sts FLAG0, r16
	ldi r16, 3
	sts TRIES, r16

init_bus:
	ldi r17, $40
	out GICR, r17		;Int0 mask
	ldi r17, $83
	out MCUCR, r17		;Interrupt 0
	clr r17
						;Setting External Interrupt flag

main:
	rcall lcd_start

	ldi r31, high(lcd_banner1<<1)
	ldi r30, low(lcd_banner1<<1)
	rcall clcd_outs1

	ldi r31, high(lcd_banner2<<1)
	ldi r30, low(lcd_banner2<<1)
	rcall clcd_outs2

	ldi r31, HIGH(msg_keyby<<1)
	ldi r30, LOW(msg_keyby<<1)
	rcall putz
	rcall delay_1s
	rcall delay_1s
	ldi r16, $0
	sts FLAG0, r16
	sei

	
main_check:
	//rcall flag_debug
	ldi r18, S3
	out PORTB, r18
	NOP
	in r16, PORTB		;Allows AVR to do scan on Column 3 by means of  for # button

	rcall receive_check
	;-------------------------------------
	;This below code will read the flag FLAG0 and decide what loop it should stick to. There are two loops: STANDBY and INPUT
	;INPUT mode FLAG0 = 1; STANDBY mode FLAG0 = 0;
	;In STANDBY mode, the AVR blocks all scanning from the keypad. This is the default mode for when an OPEN cmd is sent to the AVR.
	;In INPUT mode, the AVR scans the keypad constantly and will output any key press. This is the default mode for when a CLOSE cmd is sent to the AVR.
	;
	lds	r16, FLAG0		
	cpi r16, $01		
	breq keypad
	cpi r16, $02
	breq fingermode_reach
	;-------------------------------------
	
	cli
	ldi r31, high(lcd_standby1<<1)
	ldi r30, low(lcd_standby1<<1)
	rcall clcd_outs1

	ldi r31, high(lcd_standby2<<1)
	ldi r30, low(lcd_standby2<<1)
	rcall clcd_outs2
	sei

	//rcall fuck

	rjmp main_check

receive_check:
	sbis UCSRA, RXC
	ret
	rcall rx_receive
	ret

fuck:
	ldi r19, 'f'
	sts CMD, r19
	rcall txsendmode
	ret

fingermode_reach:
	rcall fingermode
	rjmp main_check

keypad:
	

	rcall keypad_s3
	//rcall key_debug
	//rcall flag_debug
	lds r16, IN1
	lds r17, IN2
	cp r17, r16
	brne main_check
	cpi r16, S3
	brne keypad_in_call

	rcall keypad_s2
	//rcall key_debug
	//rcall flag_debug
	lds r16, IN1
	lds r17, IN2
	cp r17, r16
	brne main_check
	cpi r16, S2
	brne keypad_in_call


	rcall keypad_s1
	//rcall key_debug
	//rcall flag_debug
	lds r16, IN1
	lds r17, IN2
	cp r17, r16
	brne main_check
	cpi r16, S1
	brne keypad_in_call

	rjmp main_check

rx_receive:
	ldi r31, HIGH(rx_buff)
	ldi r30, LOW(rx_buff)
	ldi r17, CMD_BUFF_MAX
	rcall gets
	rcall rxrcvinput
	ret

keypad_in:
	cli
	rcall txsendinput
	//-------------------
	rcall lcd_lookup
	rcall clcd_outs_xmanual
	//--------------------

	sei
	rcall delay_s
	ldi r31, HIGH(key_buff)
	ldi r30, LOW(key_buff)
    rcall hex2asc
	push r16
	mov r16, r17
	st Z+, r16
	pop r16
	st Z+, r16
	ldi r16, NUL
	st Z, r16



	ldi r31, HIGH(msg_buffer<<1)
	ldi r30, LOW(msg_buffer<<1)
	rcall putz
	ldi r31, HIGH(key_buff)
	ldi r30, LOW(key_buff)
	rcall puts
	rcall newl

return:
  	ret

keypad_in_call:
	rcall keypad_in
	rjmp main_check

keypad_s3:
	ldi r17, S3
	out PORTB, r17
	NOP
	in r16, PINB
	sts IN1, r16
	ldi r17, S3
	out PORTB, r17
	NOP
	in r17, PINB
	sts IN2, r17
	ret

keypad_s2:
	ldi r17, S2
	out PORTB, r17
	NOP
	in r16, PINB
	sts IN1, r16
	ldi r17, S2
	out PORTB, r17
	NOP
	in r17, PINB
	sts IN2, r17
	ret

keypad_s1:
	ldi r17, S1
	out PORTB, r17
	NOP
	in r16, PINB
	sts IN1, r16
	ldi r17, S1
	out PORTB, r17
	NOP
	in r17, PINB
	sts IN2, r17
	ret


key_debug:
	ldi r16, 'a'
	rcall putchar
	lds r16, IN1
	rcall keypad_in
	ldi r16, 'b'
	rcall putchar
	lds r16, IN2
	rcall keypad_in
	ret

flag_debug:
	ldi r16, 'F'
	rcall putchar
	lds r16, FLAG0
	rcall keypad_in
	ret

isr0:
	push r16
	lds	r16, FLAG0
	inc r16
	cpi r16, 2
	brsh isr_exit
	ldi r19, HASH
	sts CMD, r19
	rcall txsendmode
	sts FLAG0, r16
	pop r16
	//----------------------------

	rcall clearlcd
	ldi r31, high(lcd_input1<<1)
	ldi r30, low(lcd_input1<<1)
	rcall clcd_outs1
	ldi r17, $C0
	rcall clcd_outs_commanual
	//----------------------------
	ldi r31, HIGH(msg_keyin<<1)
	ldi r30, LOW(msg_keyin<<1)
	rcall putz
	rcall newl
	reti

isr_exit:
	ldi r31, HIGH(msg_keyexit<<1)
	ldi r30, LOW(msg_keyexit<<1)
	rcall putz
	rcall newl
	rcall delay110us
	ldi r19, 'c'
	sts CMD, r19
	rcall txsendmode
	clr r16
	sts FLAG0, r16
	pop r16
	reti

fingermode:
	rcall clearlcd
	ldi r31, high(lcd_inputok1<<1)
	ldi r30, low(lcd_inputok1<<1)
	rcall clcd_outs1
	rcall delay_1s
	ldi r31, high(lcd_inputok2<<1)
	ldi r30, low(lcd_inputok2<<1)
	rcall clcd_outs2
	rcall delay_1s
fingermode_loop:
	sbis UCSRA, RXC
	rjmp fingermode_loop
	rcall rx_receive
	ret


msg_buffer: .db "Current reading: ", NUL
msg_welcome: .db "SmartSafe Systems Version 9.0", LF, CR, "By Guilherme G. Ruas", LF, CR, "Smile for the camera!", LF, CR, NUL, 0
msg_keyby: .db "Stand By Mode...", CR, LF, NUL, 0
msg_keyexit: .db CR, LF, "Exiting keypad input mode", NUL
msg_keyin: .db "Entering keypad input mode", CR, LF, "Beginning input sequence from keypad...", NUL
msg_nok: .db "NOT OK!", CR, LF, NUL
msg_ok: .db "OK!", CR, LF, NUL
msg_badcmd: .db "ERROR: BAD SERIAL CMD: ", NUL
lcd_banner1: .db "Welcome to", $A0, 0
lcd_banner2: .db "SmartSafe Sys.", $A0, 0
lcd_standby1: .db "Stand By Mode", $A0
lcd_standby2: .db "Press # to start", $A0, 0
lcd_input1: .db "Input Mode:", $A0
lcd_reg1: .db "Registering User", $A0, 0
lcd_reg2: .db "Type combination", $A0, 0
lcd_regok1: .db "Registration", $A0, 0
lcd_regok2: .db "Successful!", $A0
lcd_inputok1: .db "Input OK!", $A0
lcd_inputok2: .db "Scan Finger ->", $A0, 0
lcd_inputnok1: .db "Comb Not Found", $A0, 0
lcd_inputnok2: .db "Tries Left: ", $A0, 0
lcd_report1: .db "Attempt Reported", $A0, 0
lcd_report2: .db "Photo taken...", $A0, 0


.include "txmode.inc"
.include "termio.inc"
.include "rxmode.inc"
.include "lcd.inc"
.exit

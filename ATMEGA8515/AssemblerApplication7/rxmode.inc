
rxrcvinput:
	ldi r27, HIGH(rx_buff)
	ldi r26, LOW(rx_buff)
	push r19
	push r20

rxrcvinput_1:
	ld r19, X+
	cpi r19, $F2			//Checks if the command stored in the buffer was encapsulated by the value $F2 as per the protocol used in SmartSafe.
	brne rxrcvinput_2		//Jumps to cmd invalid response
	ld r19, X+
	ld r20, X
	cpi r20, $F2
	brne rxrcvinput_2

	//The following statements could be likened to a switch case structure, where the value in r19 remains constant, and the program
	//decides were to send it next based on the character.

	cpi r19, 'o'			
	breq rcv_open
	cpi r19, 'c'
	breq rcv_close
	cpi r19, 'r'
	breq rcv_reg
	cpi r19, 'i'
	breq rcv_input
	cpi r19, 's'
	breq rcv_scan

	rjmp rcv_more

rxrcvinput_2:
	ldi r31, HIGH(msg_badcmd<<1)
	ldi r30, LOW(msg_badcmd<<1)
	rcall putz
	mov r16, r19
	rcall putchar
	rjmp rx_end

rcv_reg:
	//ldi r20, $01
	//sts FLAG0, r20
	cli
	rcall clearlcd
	ldi r31, high(lcd_reg1<<1)
	ldi r30, low(lcd_reg1<<1)
	rcall clcd_outs1
	rcall delay_1s
	ldi r31, high(lcd_reg2<<1)
	ldi r30, low(lcd_reg2<<1)
	rcall clcd_outs2
	rcall delay_1s

rcv_input:
	cli
	ldi r16, $00
	sts FLAG0, r16
	rcall int_isr0
	rjmp rx_end

rcv_open:
	//Placeholder instructions for when i fix the AVR current source problem.
//	ldi r20, $FF
//	out PORTE, r20
	ldi r20, $00
	sts FLAG0, r20
	ldi r19, 'o'
	sts CMD, r19
	rcall txsendmode
	
	sbi UCSRA, UDRE

	ldi r31, HIGH(msg_ok<<1)
	ldi r30, LOW(msg_ok<<1)
	rcall putz

	cli
	rcall delay_1s
	rcall delay_1s
	sei
	rjmp rcv_close

rcv_close:
	ldi r20, $00
	out PORTE, r20
	ldi r20, $00
	sts FLAG0, r20
	ldi r19, 'c'
	sts CMD, r19
	rcall txsendmode

	sbi UCSRA, UDRE

	ldi r31, HIGH(msg_ok<<1)
	ldi r30, LOW(msg_ok<<1)
	rcall putz

	rjmp rx_end

rcv_scan:
	ldi r31, HIGH(msg_ok<<1)
	ldi r30, LOW(msg_ok<<1)
	rcall putz
	rjmp rx_end

rcv_more:
	cpi r19, '1'
	breq reg_ok
	cpi r19, '2'
	breq input_ok
	cpi r19, '3'
	breq input_nok

	rjmp rxrcvinput_2

reg_ok:
	ldi r19, 3
	sts TRIES, r19
	rcall clearlcd
	ldi r31, high(lcd_regok1<<1)
	ldi r30, low(lcd_regok1<<1)
	rcall clcd_outs1
	rcall delay_1s
	ldi r31, high(lcd_regok2<<1)
	ldi r30, low(lcd_regok2<<1)
	rcall clcd_outs2
	rcall delay_1s
	rjmp rx_end

input_ok:
	ldi r19, 3
	sts TRIES, r19
	rcall clearlcd
	ldi r19, $02
	sts FLAG0, r19
	rjmp rx_end

input_nok:
	lds r19, TRIES
	dec r19
	breq input_report
	sts TRIES, r19
	mov r16, r19
	rcall clearlcd
	ldi r31, high(lcd_inputnok1<<1)
	ldi r30, low(lcd_inputnok1<<1)
	rcall clcd_outs1
	ldi r31, high(lcd_inputnok2<<1)
	ldi r30, low(lcd_inputnok2<<1)
	rcall clcd_outs2
	rcall hex2asc
	mov r17, r16
	rcall clcd_outs_xmanual
	rcall delay_1s
	rjmp rx_end

input_report:
	ldi r19, 3
	sts TRIES, r19
	rcall clearlcd
	ldi r31, high(lcd_report1<<1)
	ldi r30, low(lcd_report1<<1)
	rcall clcd_outs1
	rcall delay_1s
	ldi r31, high(lcd_report2<<1)
	ldi r30, low(lcd_report2<<1)
	rcall clcd_outs2
	rcall delay_1s
	rjmp rx_end

rx_end:
	pop r20
	pop r19
	ret

rx_buff_init:
	push r19
	push r20
	ldi r27, HIGH(rx_buff)
	ldi r26, LOW(rx_buff)
	ldi r19, $0
	ldi r20, 3

rx_buff_init_1:
	st X+, r19
	dec r20
	brne rx_buff_init_1
	pop r20
	pop r19
	ret
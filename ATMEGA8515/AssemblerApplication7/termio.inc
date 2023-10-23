;
; Filename: termio.inc
; 
; The following code supplied to the Fall 2018 243-513-DW students for educational/study
; purposes. The use of these routines, in whole or in part, without proper reference to 
; origin is a violation of ISEP regarding Academic Integrity and Plagerism.
;
;Description: Subroutine framework for use in the project (do not modify) 
;
;Author: Mr. Markou

.equ  UBRR  = 25   ;see p.138 (important)	
.equ  FRAME = $86      ;8N1 standard frame
.equ  TXE = $18        ;Transmit & receive enable     
.equ  LF = $0A		   ;ASCII line feed
.equ  CR = $0D		   ;ASCII carriage return
.equ  NUL = 0	       ;string terminating value
.equ  EOL = 0	       ;string terminating value
.equ  EOT = $04	       ;string terminating value
.equ  MAX = 3

;
; Standard USART init which logically belongs here
; 
init_uart:                 
	ldi R16, 0	       ;always zero (mostly)
	out UBRRH, R16    
	ldi R16, UBRR	 
	out UBRRL, R16     ;config. the rate of data tx 
	ldi R16, TXE      
	out UCSRB, R16     ;enable port tx (see p.158)
	ldi R16, FRAME     ;defined in calling     
	out UCSRC, R16     ;config. frame elements 
	ret
;
;*gets - asm workalike C routine which inputs from UART RxD
; Entry: Z reg must point to key_buff  
;        R17 must contain the size of the key_buff.
gets:
	dec   R17	       ;leave room for NUL w/o exceeding key_buff limit
gtx:					//This sbr is using getch instead of getche so it does not echo!
    rcall getch
	st	  Z+, R16
	cpi	  R16, CR      ;check for returm
	breq  gty
	dec	  R17
	brne  gtx
gty:
	ldi   R16, NUL     ;place end of string
	st    Z, R16	
	ret
;
;*getch - asm workalike C routine to receive char from UART
; Exit:  R16 contain rx char
getch:

	in    R16,UCSRA
	andi  R16, $80
	breq  getch		   ;poll status for key press
	in	  R16,UDR      ;get char from UART 
	ret

getout:
	ret

;
;*getche - like above but w/echo
; Exit:  R16 contain rx char
getche:
	rcall getch
	push  R16          ;save R16 on stack
	rcall putchar
	pop  R16           ;and retrieve it back
	ret
;
;*putchar - char tx UART routine 
; Entry: R16  char. to send
putchar:	
    out  UDR,R16	   ;txmt char. out the TxD 
putc1:		
    in   R16, UCSRA    ;poll status reg
	andi R16, $20      ;check for tx complete
	breq putc1
	ret	
;
;*puts - asm workalike routine to puts() in C
; Entry: Z index points to an NUL terminated string in cseg or dseg
putz:
	push r16
putz_loop:
	lpm   R16, Z+      ;get char from memory
	cpi   R16, NUL     
	breq  pxz		   ;this is the end, my only friend, the end
	rcall putchar      ;send it to uart
	rjmp  putz_loop
pxz: 
	pop r16
	ret

puts:
	push r16
puts_loop:
	ld   R16, Z+      ;get char from memory
	cpi   R16, NUL     
	breq  pxs		   ;this is the end, my only friend, the end
	rcall putchar      ;send it to uart
	rjmp  puts_loop
pxs: 
	pop r16
	ret

;
;*newl - issues a new line (CR&LF) which comes in handy 
; Entry: R16
newl:
	push r16
	ldi   R16, LF	    ;nothing new here
    rcall putchar
	ldi   R16, CR	 
    rcall putchar
	pop r16
	ret	

getch_ret:
	pop r16
	ret

delay110us:
	rcall delay37us
	rcall delay37us
	rcall delay37us
	ret


delay_s:			;actually 1.61s, but that's enough
	push r18
	ldi r18, 1		;loading clock counter

second1_s:			;this loop is responsible  for the 1 second delay
					;calculations will be explained in report
	ldi r25, $ff
	ldi r24, $ff

	second2_s:
		sbiw r25:r24, 1
		brne second2_s

	dec r18
	breq fini_s	
	rjmp second1_s

	fini_s:
	pop r18
	ret

delay_1s:			;actually 1.61s, but that's enough
	push r18
	ldi r18, 6		;loading clock counter

second1_1s:			;this loop is responsible  for the 1 second delay
					;calculations will be explained in report
	ldi r25, $ff
	ldi r24, $ff

	second2_1s:
		sbiw r25:r24, 1
		brne second2_1s

	dec r18
	breq fini_1s	
	rjmp second1_1s

	fini_1s:
	pop r18
	ret

;*hex2asc - converts an 8 bit hex value into a valid ASCII characters by
;*	    masking and shifting ASCII character into a valid binary form
; Entry: R16 should contain the hex number to convert
; Usage: R15,R17,R18 **save before calling rtn if used 
; Exit:  R17:R16 contain msd:lsd respectively
hex2asc:
     clr   R17
     mov   R15,R16      ;make copy of hex number
     lsr   R16          ;shift ms nyble to ls
     lsr   R16     
     lsr   R16
     lsr   R16
h2a:
     andi  R16,$0F     ;mask off upper nyble
     ldi   R18,$30   
     add   R16,R18     ;add $30 to adjust to ascii 0 - 9
     cpi   R16,$3A     ;check if valid bcd range
     brlo  h2b
     ldi   R18,7       ;adjust to hex char into 'A' - 'F'
     add   R16,R18
h2b:
     push  R16		   ;save msd ascii
     mov   R16,R15
     inc   R17
     cpi   R17,1
     breq  h2a
	 pop  R16           ;R17:R16 contain msd:lsd ascii numbers
     pop  R17
     ret
/*
 * txmode.inc
 *
 *  Created: 3/21/2023 11:57:51 AM
 *   Author: 1834747
 *	I'll be mostly using the R19 and R20 registers in this bit. 
 *	The goal of this code is to send out codes to the Raspi which it can parse
 *		and analyze. This is done because of the necessacity of sending both status
 *		updates and input over the TXR pins.
 */

 //The idea of these subroutines is to receive the input from PORTC and transmit it in a way
 //the program at the Raspi can understand.
 //The input has already been validated at this point, but the program on the other side will keep 
 //displaying any strings sent.
 //This will be halted by a special combination of characters which will tell the program to interpret
 //these inputs differently.

 //Receives char cmd from mem address CMD and sends out.
txsendmode:
	ldi r27, HIGH(tx_buff)
	ldi r26, LOW(tx_buff)
	push r19
	push r20
	lds r19, CMD
	rjmp txsendcmd

//Automatically sends out whatever the most recent input, stored in IN1, was.
txsendinput:
	ldi r27, HIGH(tx_buff)
	ldi r26, LOW(tx_buff)
	push r19
	push r20
	lds r19, IN1
	rjmp txsendcmd

txsendcmd:
	ldi r20, $F1
	st X+, r20
	st X+, r19
	st X+, r20
	ldi r20, NUL
	st X, r20

	ldi r31, HIGH(tx_buff)
	ldi r30, LOW(tx_buff)
	rcall puts

	pop r20
	pop r19
	ret


tx_buff_init:
	push r19
	push r20
	ldi r27, HIGH(tx_buff)
	ldi r26, LOW(tx_buff)
	ldi r19, $0
	ldi r20, 3

tx_buff_init_1:
	st X+, r19
	dec r20
	brne tx_buff_init_1
	pop r20
	pop r19
	ret
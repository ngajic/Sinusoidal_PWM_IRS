;-------------------------------------------------------------------------------
; Interrupt service routine for both Timer A1 and Timer A0
;-------------------------------------------------------------------------------
 			.cdecls C,LIST,"msp430.h"

			.ref	digit					
			.ref	frequency

			.sect	.const
segtab		.byte	0x7e					; digit 0-9 table
			.byte	0x30
			.byte	0x6d
			.byte	0x79
			.byte	0x33
			.byte	0x5b
			.byte	0x5f
			.byte	0x70
			.byte	0x7f
			.byte	0x7b

			.text

TA0ISR		push	R12						; push R12 to stack, cause it will be used
			mov.b 	frequency, R12			; move value of frequency into R12
			dec.b	digit					; decrement digit, make sure it is opposite from what was in here (either 0 or 1)
			jz		LED1					; if digit was equal 1, select Led display digit that represents ones, if not select the one representing tens
LED10		cmp		#10, R12				; compare value in R12 with 10
			jn		ZZERO					
			mov.b 	#1, R12
			jmp		SEL1
ZZERO		mov.b	#0, R12
SEL1		bis.b 	#BIT0, &P11OUT
			mov.b 	segtab(R12), &P6OUT
			bic.b 	#BIT1, &P11OUT
			mov.b 	#1, digit
			jmp		EXIT
LED1		cmp		#10, R12
			jn		SEL0
			sub.b 	#10, R12
SEL0		bis.b 	#BIT1, &P11OUT
			mov.b	segtab(R12), &P6OUT
			bic.b 	#BIT0, &P11OUT
EXIT		bic.b 	#CCIFG, &TA0CCTL0		; clears interrupt flag
			pop		R12						; returns original value to R12 before returning to main
			reti							; return from interrupt


TA1ISR		bic.b	#0x30, &TA1CTL			; halting the timer
			bit.b	#BIT4, &P2IN			; testing if S1 was pressed
			jz		S1						; jump here if S1 was pressed
			bit.b 	#BIT5, &P2IN			; testing if S2 was pressed
			jz		S2						; jump here if S2 was pressed
			jmp		TA1_EXIT				; if nothing, gtfo
S1			inc.b	frequency				; in here increment frequency by 1 Hz
			cmp		#17, frequency			; check if frequency has invalid value of 17 Hz
			jnz		TA1_EXIT				; if not, everything OK
			mov.b 	#1, frequency			; if does, change it to 1 Hz
			jmp		TA1_EXIT				; now do reti
S2			dec.b	frequency				; in here, decrement frequency by 1 Hz
			jnz		TA1_EXIT				; if frequency does not have invalid value of 0 Hz, go to exit
			mov.b	#16, frequency			; if it does, change it to 16 Hz
TA1_EXIT	bic.b	#CCIFG, &TA1CCTL0		; clear interrupt flag of this routine
			bis.b 	#TACLR, &TA1CTL			; clear timer counter
			reti							; return from interrupt

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------

			.sect	.int49					; Setup Timer A1 ISR
			.short	TA1ISR

			.sect	.int54					; Setup Timer A0 ISR
			.short	TA0ISR


			.end

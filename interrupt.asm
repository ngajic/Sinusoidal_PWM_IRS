;-------------------------------------------------------------------------------
; Interrupt service routine for both Timer A1
;-------------------------------------------------------------------------------
 			.cdecls C,LIST,"msp430.h"
				
			.ref	frequency

			.text						

TA1ISR			bic.b	#0x30, &TA1CTL			; halting the timer
			bit.b	#BIT4, &P2IN			; testing if S1 was pressed
			jz	S1				; jump here if S1 was pressed
			bit.b 	#BIT5, &P2IN			; testing if S2 was pressed
			jz	S2				; jump here if S2 was pressed
			jmp	TA1_EXIT			; if nothing, gtfo
S1			inc.b	frequency			; in here increment frequency by 1 Hz
			cmp	#17, frequency			; check if frequency has invalid value of 17 Hz
			jnz	TA1_EXIT			; if not, everything OK
			mov.b 	#1, frequency			; if does, change it to 1 Hz
			jmp	TA1_EXIT			; now do reti
S2			dec.b	frequency			; in here, decrement frequency by 1 Hz
			jnz	TA1_EXIT			; if frequency does not have invalid value of 0 Hz, go to exit
			mov.b	#16, frequency			; if it does, change it to 16 Hz
TA1_EXIT		bic.b	#CCIFG, &TA1CCTL0		; clear interrupt flag of this routine
			bis.b 	#TACLR, &TA1CTL			; clear timer counter
			reti					; return from interrupt

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------

			.sect	.int49				; Setup Timer A1 ISR
			.short	TA1ISR

			.end

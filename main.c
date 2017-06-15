/**
 * @file main.c
 * @brief Sinusoidal-wise PWM
 *
 * Here we have a main function which simulates changing of an LE diode brightness in sinusoidal manner.
 * Frequency of such sine can go from 1 Hz to 16 Hz, and it is changed with UP and DOWN buttons.
 * Currently used frequency is presented on multiplexed 7-segment display.
 * Project is done on TI MSP430F5438A board.
 *
 * @date 22.4.2017
 * @author Nikola Gajic (nikola.gajic93@yahoo.com)
 */

 /** Necessary include */
#include <msp430.h> 

/** Simple variable representing what digit is used on LED display, for multiplexing */
unsigned int digit = 0;

/** Variable used to store how many times sample has been used for PWM pulse width */
unsigned int cnt = 0;

/** Variable where used frequency is stored */
unsigned int frequency = 1;

/** Variable that will store appropriate sample of sine */
unsigned int sample = 0;

/** Array with 256 equidistant samples of sine generated in Matlab */
unsigned int sine_values[] = {
    127, 130, 133, 136, 139, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 181, 184,
   187, 190, 193, 195, 198, 200, 203, 205, 208, 210, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231,
   233, 235, 236, 238, 239, 241, 242, 243, 245, 246, 247, 248, 249, 250, 250, 251, 252, 252, 253, 253,
   253, 254, 254, 254, 254, 254, 254, 254, 253, 253, 252, 252, 251, 251, 250, 249, 248, 247, 246, 245,
   244, 243, 241, 240, 239, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 214, 211, 209,
   207, 204, 202, 199, 196, 194, 191, 188, 186, 183, 180, 177, 174, 171, 168, 166, 163, 159, 156, 153,
   150, 147, 144, 141, 138, 135, 132, 129, 125, 122, 119, 116, 113, 110, 107, 104, 101,  98,  95,  91,
    88,  86,  83,  80,  77,  74,  71,  68,  66,  63,  60,  58,  55,  52,  50,  47,  45,  43,  40,  38,
    36,  34,  32,  30,  28,  26,  24,  22,  20,  19,  17,  15,  14,  13,  11,  10,   9,   8,   7,   6,
     5,   4,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     3,   3,   4,   5,   6,   7,   8,   9,  11,  12,  13,  15,  16,  18,  19,  21,  23,  25,  27,  29,
    31,  33,  35,  37,  39,  41,  44,  46,  49,  51,  54,  56,  59,  61,  64,  67,  70,  73,  75,  78,
    81,  84,  87,  90,  93,  96,  99, 102, 105, 108, 111, 115, 118, 121, 124, 127
};

/**
 * @brief Main function
 *
 * Initialization of necessary parameters
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;					              // Shut down watchdog timer

    P2DIR &= 0xCF;                      		      // set P2.4 and P2.5 as in
    P2IES |= BIT4 | BIT5;               		      // set P2.4 and P2.5 irq as h->l transition
    P2IFG &= ~(BIT4 | BIT5);            		      // clear P2.4 and P2.5 IFG
    P2IE |= BIT4 | BIT5;                		      // enable P2.4 and P2.5 isr

    P11DIR |= 0x03;                     		      // setting P11.0 and P11.1 as out
    P6DIR |= ~BIT7;                     		      // setting P6 pins as out

    P4DIR |= BIT5;                      	      	// P4.3 set as out
    P4SEL |= BIT5;                      	      	// P4.3 use for TimerB0


    TA0CCR0 = 5242;                     		      // value representing period for 7-segment LED multiplexing
    TA0CCTL0 |= CCIE;                   		      // enables TA0CCR0 isr
    TA0CCTL0 &= ~CCIFG;                 		      // clears interrupt flag
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;    // use SMCLK clock source and reset it

    TA1CCR0 = 2000;                     		      // debounce period
    TA1CCTL0 |= CCIE;							                    // enables TA1CCR0 isr
    TA1CCTL0 &= ~CCIFG;							                  // clears interrupt flag
    TA1CTL = TASSEL__SMCLK | TACLR;				         // sets timer to use SMCLK clock source and clears it

    TB0CCTL5 = OUTMOD_7;                		      // reset/set outmode
    TB0CCR0 = 255;                      		      // period
    TB0CCR5 = sine_values[sample];      		      // initial pulse width value
    TB0CCTL0 |= CCIE;                   		      // enables TB0CCR0 isr
    TB0CCTL0 &= ~CCIFG;                 		      // clears interrupt flag
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    // use SMCLK clock source and configure timer for UP mode and reset it




    __enable_interrupt();						                 // enables interrupts

    while(1);
}

/**
 * @brief Interrupt service routine of Port 2
 *
 * Starts debouncing period after either button was pressed
 */
void __attribute__ ((interrupt(PORT2_VECTOR))) P2ISR (void)
{
    TA1CTL |= TASSEL__SMCLK | MC__UP | TACLR;   // start counting debounce period

    P2IFG &= ~(BIT4 + BIT5);           			      // clear button flags
    return;
}

/**
 * @brief Interrupt service routine for TB0CCR0
 *
 * Switches sine sample used for width of PWM output
 */
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) TB0CCR0ISR (void)
{
    if(++cnt == 16)                     		      // check if sample value was used for 16 times already
    {
        cnt = 0;                        		      // if it is true, reset counter
        sample = (sample + frequency) & 0xff;   // also take next sample value
        TB0CCR5 = sine_values[sample];          // and move it toward compare register
    }
    TB0CCTL0 &= ~CCIFG;                		      	// clear counter flag
    return;
}

void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TA0CCR0ISR (void)
{
    switch(digit)                               // switch to multiplexing digit
    {
    case 0:
        P11OUT |= BIT0;                         // deselect appropriate 7-segment LED digit
        P6OUT = segtab[frequency / 10];         // put
        P11OUT &= ~BIT1;                        // select appropriate 7-segment LED digit
        digit = 1;                              // change digit for next time
        break;
    case 1:
        P11OUT |= BIT1;                         // deselect appropriate 7-segment LED digit
        P6OUT = segtab[frequency % 10];         // put
        P11OUT &= ~BIT0;                        // select appropriate 7-segment LED digit
        digit = 0;                              // change digit for next time
        break;
    default: break;
    }
    TA0CCTL0 &= ~CCIFG;                         // clear fu ckin flag
    return;
}

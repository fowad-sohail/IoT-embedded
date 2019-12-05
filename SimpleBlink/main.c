#include <msp430.h>
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P4SEL |= BIT4 + BIT5;
    UCA1CTL1 |= UCSWRST;
    UCA1BR0 = 104;
    UCA1BR1 = 0;
    UCA1CTL1 |= UCSSEL_2;
    UCA1MCTL |= UCBRS_1;
    UCA1CTL1 &= ~UCSWRST;
    //UCA1IE   |= UCRXIE;
    serialPrint('c');
    __bis_SR_register(LPM0_bits + GIE);

}


//Displays letter on term
void serialPrint(char c)
{
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 'a';
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 'b';
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = 'c';
}


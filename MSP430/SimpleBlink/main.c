#include <msp430.h>

void serialPrint(char c);
void serialString(char *c, int size);
void delay_ms(unsigned int ms);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P3SEL |= BIT3 + BIT4; //4 = RX
    UCA0CTL1 |= UCSWRST;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0CTL1 |= UCSSEL_2;
    UCA0MCTL |= UCBRS_1;
    UCA0CTL1 &= ~UCSWRST;

    P2DIR |= BIT5;

    char name[4] = "1234";
    while(1)
    {
        serialString(name, 4);
        P2OUT |= BIT5;
        delay_ms(7000);
        P2OUT &= ~BIT5;
        delay_ms(7000);
    }
    __bis_SR_register(LPM0_bits + GIE);

}

//Displays letter on term
void serialPrint(char c)
{
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = c;
    delay_ms(50);
}

void serialString(char *c, int size)
{
        int i;
        for(i = 0; i<size; i++)
        {
            serialPrint(*(c+i));
        }
        serialPrint('\n');
}

void delay_ms(unsigned int ms)
{
    while (ms)
    {
        __delay_cycles(1000); //1000 for 1MHz and 16000 for 16MHz
        ms--;
    }
}

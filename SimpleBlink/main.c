#include <msp430.h>

void serialPrint(char c);
void serialString(char *c, int size);
void delay_ms(unsigned int ms);

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

    char name[4] = "1234";
    while(1)
    {
        serialString(name, 4);
        delay_ms(1000);
    }
    __bis_SR_register(LPM0_bits + GIE);

}

//Displays letter on term
void serialPrint(char c)
{
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = c;
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

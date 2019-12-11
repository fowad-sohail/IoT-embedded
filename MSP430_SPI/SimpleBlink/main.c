#include <msp430.h>
void serialPrint(char c);
void serialString(char *c, int size);
void delay_ms(unsigned int ms);
char* itoa(int value, char* result, int base);

char name[4] = "1234";
char* n = name;
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

    P1SEL &= ~BIT0;
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    REFCTL0 &= ~REFMSTR;
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12REF2_5V | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12MCTL0 = ADC12SREF_1 | ADC12INCH_0;

    // Set P6.0 to FUNCTION mode
    // This connects the physical pin P6.0/A0 to the ADC input A0
    P6SEL |= BIT0;
    // Enable the ADC.  This means we are done configuring it,
    // so we can start the conversion.
    ADC12CTL0 |= ADC12ENC;
    // Clear the start bit (as a precaution)
    ADC12CTL0 &= ~ADC12SC;

    int number = 1234;
    char name[4] = "1234";
    itoa(number, name, 10);
    while(1)
    {
        ADC12CTL0 |= ADC12SC; // Start conversion
        // The busy bit is automatically set to 0 when the conversion is done
        while(ADC12CTL1 & ADC12BUSY) {
            __no_operation(); // Could also just leave the loop empty
        }

        // Now that the conversion has completed, we can read the result
        // from the memory register
        unsigned int in_value = ADC12MEM0 & 0x0FFF; // Keep only the low 12 bits
        itoa(in_value, n, 10);
        serialString(n, 4);
        delay_ms(1000);
    }
    __bis_SR_register(LPM0_bits + GIE);
}
char* itoa(int value, char* result, int base)
{
        // check that the base if valid
        if (base < 2 || base > 36) { *result = '\0'; return result; }
        char* ptr = result, *ptr1 = result, tmp_char;
        int tmp_value;
        do {
                tmp_value = value;
                value /= base;
                *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
        } while ( value );
        // Apply negative sign
        if (tmp_value < 0) *ptr++ = '-';
        *ptr-- = '\0';
        while(ptr1 < ptr) {
                tmp_char = *ptr;
                *ptr--= *ptr1;
                *ptr1++ = tmp_char;
        }
        return result;
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


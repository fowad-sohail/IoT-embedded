#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include "BME280.h"
#include <string.h>

#define PERIOD 10000 //Samping period. 10000 count is approximately 1 second; maximum is 65535

//These are function prototypes. Its a thing in c/c++ that allows us to use a function above the code where we actually impliment it 
void SetTimer(void);
void SetVLO(void);
void SetPins(void);
void SetUART(void);
void SetSPI(void);
void serialPrint(char c);
void serialString(char *c, int size);
void delay_ms(unsigned int ms);
char* itoa(int value, char* result, int base);

char str[80];
volatile uint8_t i,count;
volatile int32_t CorT;
volatile uint32_t CorH, CorP;

//This initilizes UART comms 
//With a baud rate of 115200
//Other than that its the default settings
void init_UART()
{
     WDTCTL = WDTPW | WDTHOLD;
     P3SEL |= BIT3 + BIT4; //4 = RX
     UCA0CTL1 |= UCSWRST;
     UCA0BR0 = 8;
     UCA0BR1 = 0;
     UCA0CTL1 |= UCSSEL_2;
     UCA0MCTL |= UCBRS_6;
     UCA0CTL1 &= ~UCSWRST;
}

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    init_UART(); //Do the UART initilization
    SetPins(); //Set the pins for UART and SPI
    SetVLO(); //Changes clock freqency for some reason?
    SetTimer(); //Sets the low power mode timer 
    SetSPI(); //Initilizes SPI
    _BIS_SR(GIE); //Enable global interrupts

    //Get the trimming parameters from sensor for raw data conversion
    GetCompData();

    while(1)
    {
         TA0CCR0 = PERIOD; // Polling period
         LPM3;      //Wait in low power mode
         //Burst read on SPI to get 3 press data bytes, 3 temp bytes and 2 humidity bytes
         ReadTHsensor();
         //Apply cal factors to raw data
         CorT = CalcTemp(); //Corrected temperature
         CorH = CalcHumid(); //Corrected humidity
         CorP = CalcPress(); //Corrected pressure
		 
		 //To advoid floats the decimal points are just returns with the numbers
		 //We need to remove the decimal data
         unsigned int fixedTemp = CorT/100;
         unsigned int fixedHumid = CorH/1000;
         unsigned int fixedPress = CorP/100;

         char temp[3];
         char humid[3];
         char press[4];
		
		//The data format to transmit is:
		//[temp];[humidity];[pressure]
         itoa(fixedTemp, temp, 10); //Convert to ASCII
         serialString(temp, 3); //Transmit it
         serialPrint(';'); //Add the semicolon at the end so the ESP knows that is the end of the value

         itoa(fixedHumid, humid, 10); //Convert to ASCII
         serialString(humid, 3); //Transmit it
         serialPrint(';'); //Add the semicolon at the end so the ESP knows that is the end of the value

         itoa(fixedPress, press, 10); //Convert to ASCII
         serialString(press, 4); //Transmit it
         serialPrint(';'); //Add the semicolon at the end so the ESP knows that is the end of the value

         delay_ms(20000);
    }
}

#pragma vector=TIMER0_A0_VECTOR
 __interrupt void timerfoo (void)
 {
    LPM3_EXIT;
 }

 void SetPins(void)
  {
     /* Port 1
     P1.0 Red LED
     P1.5 TH Select.  Pull this CSB line low to enable BME280 communication
     CS_TH must also be defined in BME280.h
     */
     P1DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
     P1OUT &= ~BIT0; //LED off

     /* Port 2
     P2.1  Button on Launchpad
     */
     P2DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;

     /* Port 3 */
     P3SEL = BIT0 + BIT1 + BIT2 + BIT3 + BIT4; //SPI + UART lines
     P3DIR |= BIT5 + BIT6 + BIT7; //Unused lines

     /* Port 4
     P4.0--4.6 unused
     P4.7 Green LED
     */
     P4DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
     P4OUT &= ~BIT7;

     /* Port 5
     P5.0 Unused
     P5.1 Unused
     P5.2--P5.5 grounded or open as per spec sheet
     */
     P5DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;

     /* Port 6
     P6.0--6.7 unused
     */
     P6DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
  }

//Sets clock frequency
 void SetVLO(void)
    { //Default frequency ~ 10 kHz
    UCSCTL4 |= SELA_1;  //Set ACLK to VLO
    }

 void SetTimer(void)
     {
    TA0CCTL0 |= CCIE;  //Enable timer interrupt
    TA0CTL = TASSEL_1 | MC_1;  //Set Timer A to ACLK; MC_1 to count up to TA0CCR0.
     }

//Inilizes SPI communication
 void SetSPI(void)
   {
     // Configure the USCI module: 3-pin SPI
     UCB0CTL1 |= UCSSEL_2 + UCSWRST;  //Select SMCLK; should be in reset state at PUC but set anyway
     UCB0CTL0 |= UCMST + UCSYNC + UCMSB + UCCKPH; //Set as master, synchronous, MSB first, clock phase adjust
     //UCB0BR0 |= 0x02; //Divide SMCLK by 2 to clock at 500 kHz
     UCB0BR0 |= 0x01; //1 MHz SPI clock
     UCB0BR1 |= 0x00;
   }

//Converts interager to its equivilent ASCII charecter
 char* itoa(int value, char* result, int base)
 {
         // check that the base if valid
         if (base < 2 || base > 36) { *result = '\0'; return result; }
         char* ptr = result, *ptr1 = result, tmp_char;
         int tmp_value;
		 //While they are still numbers left in value
         do {
                 tmp_value = value;
                 value /= base;
                 *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)]; //Add 35 to the remander, select this value
         } while ( value ); 
         // Apply negative sign
         if (tmp_value < 0) *ptr++ = '-';
         *ptr-- = '\0';
		 //The result by default is backwards so this code rotates it
         while(ptr1 < ptr) {
                 tmp_char = *ptr;
                 *ptr--= *ptr1;
                 *ptr1++ = tmp_char;
         }
         return result;
 }
 
 //Prints one charecter to UART
 void serialPrint(char c)
 {
     while(!(UCA1IFG & UCTXIFG)); //Wait for TX Buffer to be free
     UCA0TXBUF = c; //Add it to the buffer
     delay_ms(50); //This code is unneccesary but is here anyway because it makes me happy
 }
 
 //Prints multiple chrecters to UART
 void serialString(char *c, int size)
 {
         int i;
         for(i = 0; i<size; i++) //For each charecter
         {
             serialPrint(*(c+i)); //Print that charecter
         }
 }
 
 //Simple and easy delay function that works
 void delay_ms(unsigned int ms)
 {
     while (ms)
     {
         __delay_cycles(1000); //1000 for 1MHz and 16000 for 16MHz
         ms--;
     }
 }

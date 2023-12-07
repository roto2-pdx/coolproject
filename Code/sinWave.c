/*
 * GuitarPedal.c
 *
 * Created: 12/01/2023
 * Author : Lynn
 * 
 * NOTE : ADC operates in single conversion, manual mode.
 */ 


#ifndef F_CPU
#define F_CPU 16000000UL    // 16MHz Clock Speed
#endif

 #include <avr/io.h>
// #include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

const uint8_t sinewave[] = // 256 values
{
0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf5,
0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,
0xda,0xd8,0xd5,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb3,
0xb0,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x98,0x95,0x92,0x8f,0x8c,0x89,0x86,0x83,
0x80,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,
0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,
0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,
0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,
0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c
};
int i = 0;

// Function definitions
void switch_adc(void);
void pin_setup();
void pwm_setup();
long map(long x, long in_min, long in_max, long out_min, long out_max);

int main(void) {
    cli();
    pin_setup();
    pwm_setup();
    adc_setup();
    sei(); // Enable global interrupts

    while (1) {}; // prevent program from ending
}

////////////////////////////////////////////////////////////////////////////
//                      Interrupt Service Routines                        //
////////////////////////////////////////////////////////////////////////////

// Timer1 interrupt
ISR(TIMER1_COMPA_vect) {
    OCR1A = sinewave[i];
    i++; 
}

////////////////////////////////////////////////////////////////////////////
//                          HELPER FUNCTIONS                              //
////////////////////////////////////////////////////////////////////////////

void pin_setup(void){
    // Pins PD6 PD7 as output with PWM
    DDRD |= ((1<<DDB6) | (1<<DDB7));
}

// Used to generate PWM signals
void pwm_setup(void) {
    // initial OCR1A value
    OCR1A = 0x80;
    PRR0 &= ~(1<<PRTIM1);               // Enable TIMER1 module 
 
    // TCNT1 = 0x00;
    // TCCR1A = (1<<COM1A1);               // Set output to low level.
    TCCR1B = (1<<WGM13) | (0<<WGM12);   // Phase correct, PWM waveform generation
    TCCR1A = (1<<WGM11) | (1<<WGM10);   // TOP = ICR1         

    TCCR1B = (1<<CS10);                 // CLK/1 = 16 MHz TIMER1
    TCCR1B = (1<<COM1B1);               //

    ICR1L = 0xFF;                        
    ICR1H = (0xFF >> 8);                // PWM frequency = TIMER1/(1*PRE*ICR1)
                                        // PWM Resolution = log2

    // TIMSK1 = (1<<ICIE1);               // Enable TIMER1 capture interrupt
    TIMSK1 = (1<<OCIE1A);               // Enable TIMER1 capture interrupt


    //Output compare OC1A 8 bit non inverted PWM
    TCCR1A=0x91;
}


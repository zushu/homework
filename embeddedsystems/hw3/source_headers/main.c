/**
 * GROUP 5 
 * ZUMRUD SHUKURLU 2174761
 * SHARIF AFANDI   2278877
 * NIHAD  ALIYEV   2278885
 **/

#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#include <xc.h>
#include "breakpoints.h"


short timer1_500ms_counter = 0; // up to 10
short timer1_5s_counter = 0; // up to 10
short portb_val = 0; // // to circumvent the mismatch condition in interrupt
char is_correct = 0;
//short current_ad_val = 0;
char counter_update_flag = 0;

// helper function declarations
void init_ports();
void init_external();
void init_timer0();
void init_timer1();
void timer1_setup();
void init_ad();
short ad_value();
void display_value(int value);
void check_guess();
void reset();

void __interrupt() isr();


void main(void) 
{
    init_ports();
    init_external();
    init_timer0();
    init_timer1();
    init_ad();
    init_complete();
    
    while(1) {}
    
    return;
}

// reset IO ports and the game state
void reset()
{
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    PORTH = 0;
    PORTJ = 0;
    is_correct = 0;
}
// initialize external interrupts
void init_external()
{
    RCONbits.IPEN = 0;      // disable priorities
    INTCON = 0;
    INTCONbits.RBIE = 1;    // port B [4-7] as interrupt source   
    INTCONbits.GIE = 1;     // use interrupts
      
}

// initialize IO ports
void init_ports()
{
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    //PORTH = 0;
    //PORTJ = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISJ = 0;
    TRISHbits.RH3 = 0;
    
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    LATH = 0;
    LATJ = 0;
    TRISB = 0b00010000;
    
    INTCON2 = 0; // INTEDG0 = 0: interrupt on falling edge
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = 1;
}

void init_timer0()
{
    
    RCONbits.IPEN = 0; // only high priority interrupts    
    T0CONbits.TMR0ON = 1; // timer is on // TODO: check, should i use it?
    T0CONbits.T08BIT = 0; // 16 bit timer
    T0CONbits.T0CS = 0; // system clock to be used
    T0CONbits.PSA = 0; // pre-scaler will be used 
    T0CONbits.T0PS2 = 0; // 010: 1:8 pre-scaler
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 0;
    INTCONbits.TMR0IE = 1; // last action: enable timer 
    // TODO: set GIE once in the main function after all initializations, not in each init function.
    INTCONbits.GIE = 1; // use interrupts
    // timer should start at 0d3036 or 0x0BDC.
    TMR0H = 0x0B;
    TMR0L = 0xDC;
}

void init_timer1()
{
    RCONbits.IPEN = 0; // only high priority interrupts 
    T1CONbits.RD16 = 1; // write to timer1 with one 16-bit op
    T1CONbits.T1RUN = 1; 
    T1CONbits.T1CKPS1 = 1; // 11: 1:8 pre-scaler
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.T1OSCEN = 1; // TODO: CHECK!! timer 1 oscillator enabled
    T1CONbits.TMR1CS = 0; // internal clock
    T1CONbits.TMR1ON = 1; // enable timer
    PIE1bits.TMR1IE = 1; // enable timer1 interrupt
    // should start at 0d3036 or 0x0bdc
    TMR1H = 0x0b;
    TMR1L = 0xdc;
    
}

// to be called at timer1 interrupt
void timer1_setup()
{   
    TMR1H = 0x0b;
    TMR1L = 0xdc;
    timer1_500ms_counter++; // 500ms is complete when it is 10
    if (timer1_500ms_counter == 10)
    {
        // do something
        hs_passed();
        timer1_500ms_counter = 0;
        timer1_5s_counter++;
        // overwrite timer1_5s_counter to 10 at correct guess
        // counter_update_flag ensures it is done only once
        // if timer1_5s_counter >= 10, do nothing
        if (is_correct && counter_update_flag && timer1_5s_counter < 10) 
        {
            timer1_5s_counter = 10;
            counter_update_flag = 0;
        }
    }  
    // first half-second after the game is over
    if (timer1_5s_counter == 10)
    {
        // game over is called regardless of the guess result
        game_over();
        // delete arrow
        PORTC = 0;
        PORTD = 0;
        PORTE = 0;
        LATC = 0;
        LATD = 0;
        LATE = 0;
        //latcde_update_complete();
        // display desired number
        display_value(special_number());
        latjh_update_complete();
    }
    // second half-second
    if (timer1_5s_counter == 11)
    {
        // dim out
        PORTHbits.RH3 = 1;
        LATHbits.LATH3 = 1;
        PORTJ = 0;
        LATJ  = 0;
        latjh_update_complete();
        
    }
    // third half-second
    if (timer1_5s_counter == 12)
    {
        // show the special number
        display_value(special_number());
        latjh_update_complete();
    }
    // fourth half-second
    if (timer1_5s_counter == 13)
    {
        // dim out
        PORTHbits.RH3 = 1;
        LATHbits.LATH3 = 1;
        PORTJ = 0; 
        LATJ  = 0;
        latjh_update_complete(); 
        timer1_5s_counter = 0; // restart timer1
        restart(); // label function
        reset(); // resets IO and 7-segment display ports
    }   
}

// ad conversion initialization settings
void init_ad()
{
    INTCONbits.PEIE = 1; // enable peripheral interrupts
    PIE1bits.ADIE = 1; // ad converter interrupt enable
    // channel AN12
    ADCON0bits.CHS3 = 1;
    ADCON0bits.CHS2 = 1;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    
    ADCON1bits.VCFG1 = 0; // (internal) V_DD
    ADCON1bits.VCFG0 = 0; // (internal) V_SS
    // analog from AN0 to AN12
    ADCON1bits.PCFG3 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG1 = 1;
    ADCON1bits.PCFG0 = 0;
    
    // TODO: adcon2 settings 8c
    ADCON2bits.ADFM = 1; // right justified
    ADCON2bits.ACQT2 = 0; // TODO: check
    ADCON2bits.ACQT1 = 0;
    ADCON2bits.ACQT0 = 1; // 2*T_AD // not sure
    ADCON2bits.ADCS2 = 1;
    ADCON2bits.ADCS1 = 0;
    ADCON2bits.ADCS0 = 0; //F_OSC / 4 // not sure
    
    ADCON0bits.ADON = 1;
}

// fit the value read at adc conversion to [0,9] interval
short ad_value()
{
    if (ADRES >= 0 && ADRES <= 102)
    {
        return 0;
    }
    if (ADRES > 102 && ADRES <= 204)
    {
        return 1;
    }
    if (ADRES > 204 && ADRES <= 306)
    {
        return 2; 
    }
    if (ADRES > 306 && ADRES <= 408)
    {
        return 3;
    }
    if (ADRES > 408 && ADRES <= 510)
    {
        return 4;
    }
    if (ADRES > 510 && ADRES <= 612)
    {
        return 5;
    }
    if (ADRES > 612 && ADRES <= 714)
    {
        return 6;
    }
    if (ADRES > 714 && ADRES <= 816)
    {
        return 7;
    }
    if (ADRES > 816 && ADRES <= 918)
    {
        return 8;
    }
    if (ADRES > 918 && ADRES <= 1023)
    {
        return 9;
    }
}

// display given value in the 7-segment display
void display_value(int value)
{
    PORTHbits.RH3 = 1;
    LATHbits.LATH3 = 1;
    
    // used to be in switch-case :( 
    if (value == 0)
    {
        PORTJ = 0b00111111;
        LATJ  = 0b00111111;   
    }
    else if (value == 1)
    {
            PORTJ = 0b00000110;
            LATJ  = 0b00000110;
    }
    else if (value == 2)
    {
            PORTJ = 0b01011011;
            LATJ  = 0b01011011;
    }
    else if (value == 3)
    {
            PORTJ = 0b01001111;
            LATJ  = 0b01001111;
    }
    else if (value == 4)
    {
            PORTJ = 0b01100110;
            LATJ  = 0b01100110;
    }
            
    else if (value == 5)
    {
            PORTJ = 0b01101101;
            LATJ  = 0b01101101;
    }
    else if (value == 6)
    {
            PORTJ = 0b01111101;
            LATJ  = 0b01111101;
    }
    else if (value == 7)
    {
            PORTJ = 0b00000111;
            LATJ  = 0b00000111;
    }
    else if (value == 8)
    {
            PORTJ = 0b01111111;
            LATJ  = 0b01111111;
    }
    else if (value == 9)
    {
            PORTJ = 0b01101111;
            LATJ  = 0b01101111;
    }
    else
    {
            PORTJ = 0x00; // turn off
            LATJ  = 0x00;
    }                      
}

// compare the submitted value with the special number
// show hint with arrows
void check_guess()
{
    if (ad_value() < special_number())
    {
        // up arrow
        PORTC = 0b00000010;
        PORTD = 0b00001111;
        PORTE = 0b00000010;
        LATC = 0b00000010;
        LATD = 0b00001111;
        LATE = 0b00000010;
        latcde_update_complete();
    }
    else if (ad_value() > special_number())
    {
        // down_arrow
        PORTC = 0b00000100;
        PORTD = 0b00001111;
        PORTE = 0b00000100;
        LATC = 0b00000100;
        LATD = 0b00001111;
        LATE = 0b00000100;
        latcde_update_complete();
    }
    else // equal
    {
        correct_guess();
        is_correct = 1; // correct guess flag is set
        counter_update_flag = 1; // timer1_5s_counter value will be changed to 10 regardless of the current value
        
    }
}

void __interrupt() isr()
{
    if (INTCONbits.RBIF == 1) // rb4 pressed
    {       
        portb_val = PORTB;
        INTCONbits.RBIF = 0; // reset interrupt
        portb_val = PORTB; // to circumvent the mismatch condition
        rb4_handled();
        check_guess();      
    }
    if (INTCONbits.TMR0IF == 1) // timer0 overflows
    {
        INTCONbits.TMR0IF = 0;
        TMR0H = 0x0B;
        TMR0L = 0xDC;
        ADCON0bits.GO = 1; // start ad conversion
    }
    if (PIR1bits.TMR1IF == 1) // timer1 overflows
    {
        PIR1bits.TMR1IF = 0;
        timer1_setup();
    }
    
    if (PIR1bits.ADIF == 1) // ad conversion over
    {
        PIR1bits.ADIF = 0;
        adc_value = ADRES;
        adc_complete();
        display_value(ad_value());
        latjh_update_complete();
    }
}


LIST    P=18F8722
#include <p18F8722.inc>
    
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

    ORG 0x00
    goto init
    
init:
    
    clrf LATA
    clrf LATB
    clrf LATC
    clrf LATD
    clrf LATE
    
    clrf PORTA
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    
    movlw h'10' ; b'00010000' ; SET RA4 AS DIGITAL INPUT
    movwf TRISA 
    
    movlw h'18' ; b'00011000' ; SET RE3 AND RE4 AS DIGITAL INPUT
    movwf TRISE
    
    movlw h'00' ; not sure
    movwf TRISB
    
    ;movlw h'00' ; not sure
    movwf TRISC
    
    ;movlw h'00' ; not sure
    movwf TRISD
    
main:
    call turnOnLeds
    goto main
    
turnOnLeds
    
    movlw h'F0' ; b'11110000'
    movwf LATB ; turn on RB[0-3]
    
    ;movlw h'F0' 
    movwf LATC ; turn on RC[0-3]
    
    movlw h'00'
    movwf LATD ; turn on RD[0-7]
    
    call delay ; 1 second delay   
    return

delay
    
    
    
    
    
    
    



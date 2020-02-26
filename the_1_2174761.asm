LIST    P=18F8722
#INCLUDE <p18f8722.inc>
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
    
; equ: define an assembly constant
    
iter1 equ 0x01
iter2 equ 0x02
iter3 equ 0x03
;iter4 equ 0x04

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
    call turnonleds
    goto main
    
turnonleds
    
    movlw h'0F' ; b'11110000'
    movwf LATB ; turn on RB[0-3]
    
    ;movlw h'F0' 
    movwf LATC ; turn on RC[0-3]
    
    movlw h'FF'
    movwf LATD ; turn on RD[0-7]
    
    call delay ; 1 second (?) delay   
    return

delay
    
    movlw d'40'
    movwf iter1
    outerloop
	movlw d'250'
	movwf iter2
	innerloop
	    movlw d'250'
	    movwf iter3
	    innerloop2
		decfsz iter3, F
		goto innerloop2
	    decfsz iter2, F
	    goto innerloop   
	decfsz iter1, F
	goto outerloop
    
    return
    
; RESUME FROM ABOVE, CALCULATION OF TIME DELAY WITH LOOPS
; CAN I USE 4 NESTED LOOPS? 
    
end
    
    
    
    
    
    
    
    



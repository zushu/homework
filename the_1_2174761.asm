LIST    P=18F8722
#INCLUDE <p18f8722.inc>
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
    
; equ: define an assembly constant
    
iter1 equ 0x01
iter2 equ 0x02
iter3 equ 0x03
;iter4 equ 0x04
buttoncounter equ 0x04
re3buttoncounter equ 0x05

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
    movwf TRISC
    movwf TRISD
    movwf buttoncounter
    
    
    
main:
    call turnonleds
    call buttoncountercheck
    goto main
    
turnonleds
    
    movlw h'0F'	    ; b'11110000'
    movwf LATB	    ; turn on RB[0-3] 
    movwf LATC	    ; turn on RC[0-3]
    
    movlw h'FF'
    movwf LATD	    ; turn on RD[0-7]
    
    call delay	    ; 1 second (?) delay  
    
    movlw h'00'	    ; turn off leds
    movwf LATB
    movwf LATC
    movwf LATD
    return

delay
    
    movlw h'01' ;movlw d'40'
    movwf iter1
    outerloop
	movlw h'01' ;movlw d'250' 
	movwf iter2
	innerloop
	    movlw h'01' ;movlw d'250'
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
    
buttoncountercheck:
    movlw h'01'
    cpfseq buttoncounter    ; compare buttoncounter with 1
    goto buttonpress1
    goto buttonpress2
    
buttonpress1:
    btfsc PORTA, 4	    ; check RA4 press
    goto buttonpress1
    goto buttonrelease1
    
buttonrelease1:
    btfss PORTA, 4	    ; check RA4 release
    goto buttonrelease1
    incf buttoncounter	    ; buttoncounter becomes 1
    goto buttonpress2
    
buttonpress2:
    btfsc PORTA, 4
    goto addition	    ; if RA4 is still released, do addition
    goto buttonrelease2
    
buttonrelease2:
    btfss PORTA, 4
    goto buttonrelease2
    incf buttoncounter	    ; buttoncounter becomes 2
    ;goto subtraction
    
buttonpress3:
    btfsc PORTA, 4
    goto subtraction	    ; if RA4 is pressed and released again, do subtraction
    goto buttonrelease3
    
buttonrelease3:
    btfss PORTA, 4
    goto buttonrelease3
    decf buttoncounter	    ; buttoncounter becomes 1
    goto buttoncountercheck
    
    
;portselection:
    
    
    
addition
;    goto portselection
    return
subtraction
;    goto portselection
    return
;re3buttonpress:    
;    btfsc PORTE, 3	    ; check RE3 press
;   goto re3buttonpress
;re3buttonrelease:
;    btfss PORTE, 3	    ; check RE3 release
;    goto re3buttonrelease
;    incf re3buttoncounter
    
; choose PORTB
    
    
       
    ; increment buttoncounter until 2 for RA4 then set back to 0
    ; check if it is 1, do addition, goto buttonpress
    ; if it is 2, do subtraction and set buttoncounter to 0, goto buttonpress
    
end
    
    
    
    
    
    
    
    



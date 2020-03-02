LIST    P=18F8722
#INCLUDE <p18f8722.inc>
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
    
; equ: define an assembly constant
    
iter1 equ 0x01
iter2 equ 0x02
iter3 equ 0x03
;iter4 equ 0x04
ra4buttoncounter equ 0x04
re3buttoncounter equ 0x05
portbvalue equ 0x06 ; re4buttoncounter for portb
portcvalue equ 0x07 ; re4buttoncounter for portc
portdvalue equ 0x08
opcode equ 0x09

    ORG 0x00
    ;goto init
    goto main

    
; CLEAR ALL PORTS, SET DIGITAL INPUT PINS, INITIALIZE VARIABLES WITH 0
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
    
    movlw h'00' 
    movwf TRISB ; not sure
    movwf TRISC
    movwf TRISD
    movwf ra4buttoncounter ; INITIALIZE VARS WITH 0
    movwf re3buttoncounter
    movwf portbvalue
    movwf portcvalue
    movwf portdvalue
    return
    
    
; MAIN LOOP   
main:
    call init
    call turnonleds
    call buttoncountercheck
    ;call portselectioncheck
    goto main
    
; TURN ON LEDS, DELAY FOR 1 SEC, TURN OFF LEDS, RETURN
    
turnonleds:    
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

; 1 SECOND TIME DELAY FUNCTION
delay:    
    movlw d'158' ;movlw h'01'
    movwf iter1
    outerloop
	movlw d'153' 
	movwf iter2
	innerloop
	    movlw d'137'
	    movwf iter3
	    innerloop2
		decfsz iter3, F
		goto innerloop2
	    decfsz iter2, F
	    goto innerloop   
	decfsz iter1, F
	goto outerloop
    
    return
    
; RA4 LOOP
    
ra4endcheck: ; re3press1
    btfsc PORTE, 3
    goto ra4countcheck
    incf re3buttoncounter
    goto re3release1
     
ra4countcheck:
    movlw h'01'
    cpfseq ra4buttoncounter
    goto ra4press1
    goto ra4press2
    
ra4press1:
    btfsc PORTA, 4
    goto ra4press1
ra4release1:
    btfss PORTA, 4
    goto ra4release1
    incf ra4buttoncounter
    goto ra4endcheck
    
ra4press2:
    btfsc PORTA, 4
    goto ra4endcheck
ra4release2:
    btfss PORTA, 4
    goto ra4release2
    incf ra4buttoncounter    
ra4press3:
    btfsc PORTA, 4
    goto ra4endcheck
ra4release3:
    btfss PORTA, 4
    goto ra4release3
    decf ra4buttoncounter
    goto ra4endcheck
 
  
; RE3 LOOP 
    
re3endcheck: ; re4press1
    btfsc PORTE, 4
    goto re3countcheck 
    ;goto re4release1 ; NOT IMPLEMENTED YET
    goto determineport
    
re3countcheck: ; old portselectioncheck
    ; check0
    movlw h'00'
    cpfsgt re3buttoncounter
    goto ra4endcheck
    ; check1
    movlw h'01'
    cpfsgt re3buttoncounter
    goto re3press2
    ; check2
    movlw h'02'
    cpfsgt re3buttoncounter
    goto re3press3
    ; check3
    movlw h'03'
    cpfsgt re3buttoncounter
    goto re3press4
    ; check4
    subwf re3buttoncounter
    goto re3countcheck

;re3press1:
;    goto ra4countcheck
re3release1:
    btfss PORTE, 3
    goto re3release1
    incf re3buttoncounter
    goto re3endcheck
    
re3press2:
    btfsc PORTE, 3
    goto re3endcheck
re3release2:   
    btfss PORTE, 3
    goto re3release2
    incf re3buttoncounter
    goto re3endcheck
    
re3press3:
    btfsc PORTE, 3
    goto re3endcheck
re3release3:
    btfss PORTE, 3
    goto re3release3
    incf re3buttoncounter
    goto re3endcheck
    
re3press4:
    btfsc PORTE, 3
    goto re3endcheck
re3release4:
    btfss PORTE, 3
    goto re3release4
    clrf re3buttoncounter
    incf re3buttoncounter
    goto re3endcheck
    
determineport:
    movlw h'00'
    cpfsgt re3buttoncounter
    goto ra4endcheck ; ra4endcheck is re3press1
    ; portbcheck
    movlw h'01'
    cpfsgt re3buttoncounter   
    goto portbre4release1
    ;goto portccheck
    ;portccheck:
    movlw h'02'
    cpfsgt re3buttoncounter
    goto portcre4release1 ; TODO: NOT IMPLEMENTED YET
    ;goto portdcheck
    ;portdcheck:
    movlw h'03'
    cpfsgt re3buttoncounter
    goto portdcalc ; TODO: NOT IMPLEMENTED YET
    subwf re3buttoncounter
    goto determineport
    
; RE4 LOOP FOR PORTB
    
portbre4endcheck:
    btfsc PORTE, 3
    goto portbre4countcheck 
    incf re3buttoncounter
    goto re3endcheck ; or re3countcheck ????
    
portbre4countcheck:
    movlw h'00'
    cpfsgt portbvalue
    goto re3endcheck ; re4press1
    
    movlw h'01'
    cpfsgt portbvalue
    goto portbre4press2
    
    movlw h'02'
    cpfsgt portbvalue
    goto portbre4press3
    
    movlw h'03'
    cpfsgt portbvalue
    goto portbre4press4
    
    movlw h'04'
    cpfsgt portbvalue
    goto portbre4press5
    
    clrf portbvalue
    goto portbre4countcheck
        
portbre4release1:
    btfss PORTE, 4
    goto portbre4release1
    incf portbvalue
    goto portbre4endcheck
    
portbre4press2:
    btfsc PORTE, 4
    goto portbre4endcheck
portbre4release2:
    btfss PORTE, 4
    goto portbre4release2
    incf portbvalue
    goto portbre4endcheck
    
portbre4press3:
    btfsc PORTE, 4
    goto portbre4endcheck
portbre4release3:
    btfss PORTE, 4
    goto portbre4release3
    incf portbvalue
    goto portbre4endcheck
    
portbre4press4:
    btfsc PORTE, 4
    goto portbre4endcheck
portbre4release4:
    btfss PORTE, 4
    goto portbre4release4
    incf portbvalue
    goto portbre4endcheck
    
portbre4press5:
    btfsc PORTE, 4
    goto portbre4endcheck
portbre4release5:
    btfss PORTE, 4
    goto portbre4release5
    clrf portbvalue
    goto portbre4endcheck
    
 
; RE4 LOOP FOR PORTC
    
portcre4endcheck:
    btfsc PORTE, 3
    goto portcre4countcheck ; NOT IMPLEMENTED YET
    incf re3buttoncounter
    goto re3endcheck ; re4press1
    
portcre4countcheck:
    movlw h'00'
    cpfsgt portcvalue
    goto re3endcheck ; re4press1
    
    movlw h'01'
    cpfsgt portcvalue
    goto portcre4press2
    
    movlw h'02'
    cpfsgt portcvalue
    goto portcre4press3
    
    movlw h'03'
    cpfsgt portcvalue
    goto portcre4press4
    
    movlw h'04'
    cpfsgt portcvalue
    goto portcre4press5
    
    clrf portcvalue
    goto portcre4countcheck
    
portcre4release1:
    btfss PORTE, 4
    goto portcre4release1
    incf portcvalue
    goto portcre4endcheck ; NOT IMPLEMENTED YET
    
portcre4press2:
    btfsc PORTE, 4
    goto portcre4endcheck
portcre4release2:
    btfss PORTE, 4
    goto portcre4release2
    incf portcvalue
    goto portcre4endcheck
    
portcre4press3:
    btfsc PORTE, 4
    goto portcre4endcheck
portcre4release3:
    btfss PORTE, 4
    goto portcre4release3
    incf portcvalue
    goto portcre4endcheck
    
portcre4press4:
    btfsc PORTE, 4
    goto portcre4endcheck
portcre4release4:
    btfss PORTE, 4
    goto portcre4release4
    incf portcvalue
    goto portcre4endcheck
    
portcre4press5:
    btfsc PORTE, 4
    goto portcre4endcheck
portcre4release5:
    btfss PORTE, 4
    goto portcre4release5
    clrf portcvalue
    goto portcre4endcheck
    

    

    
    
    
    
    

end
    
    
    
    
    
    
    
    



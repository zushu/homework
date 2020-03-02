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
;opcode equ 0x09

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
    ;movwf PORTA
    
    movlw h'18' ; b'00011000' ; SET RE3 AND RE4 AS DIGITAL INPUT
    movwf TRISE
    ;movwf PORTE
    
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
    buttonaction:
    call ra4countcheck
    ;call portselectioncheck
    goto buttonaction
    
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
    btfss PORTE, 3
    goto ra4countcheck
    ;incf re3buttoncounter
    goto re3release1
     
ra4countcheck:
    movlw h'00'
    cpfseq ra4buttoncounter
    goto ra4check1
    goto ra4press1
    ra4check1:
    movlw h'01'
    cpfseq ra4buttoncounter
    goto ra4check2
    goto ra4press2
    ra4check2:
    movlw h'02'
    cpfseq ra4buttoncounter
    goto ra4check3
    goto ra4press3
    ra4check3:
    movlw h'03'
    cpfseq ra4buttoncounter
    goto ra4check4
    clrf ra4buttoncounter
    incf ra4buttoncounter
    goto ra4countcheck
    ra4check4:
    decf ra4buttoncounter
    decf ra4buttoncounter
    goto ra4countcheck
    
ra4press1:
    btfss PORTA, 4
    goto ra4press1
ra4release1:
    btfsc PORTA, 4
    goto ra4release1
    incf ra4buttoncounter
    goto ra4endcheck
    
ra4press2:
    btfss PORTA, 4
    goto ra4endcheck
ra4release2:
    btfsc PORTA, 4
    goto ra4release2
    incf ra4buttoncounter
    goto ra4endcheck
ra4press3:
    btfss PORTA, 4
    goto ra4endcheck
ra4release3:
    btfsc PORTA, 4
    goto ra4release3
    clrf ra4buttoncounter
    incf ra4buttoncounter
    ;decf ra4buttoncounter
    goto ra4endcheck
 
  
; RE3 LOOP 
    
re3endcheck: ; re4press1
    btfss PORTE, 4
    goto re3countcheck 
    ;goto re4release1 ; NOT IMPLEMENTED YET
    goto determineport
    
re3countcheck: ; old portselectioncheck
    ; check0
    movlw h'00'
    cpfseq re3buttoncounter
    goto check1
    goto ra4endcheck
    check1:
    movlw h'01'
    cpfseq re3buttoncounter
    goto check2
    goto re3press2
    check2:
    movlw h'02'
    cpfseq re3buttoncounter
    goto check3
    goto re3press3
    check3:
    movlw h'03'
    cpfseq re3buttoncounter
    goto check4
    goto portdcalc ; NOT IMPLEMENTED YET
    ;goto re3press4
    check4:
    subwf re3buttoncounter
    goto re3countcheck

;re3press1:
;    goto ra4countcheck
re3release1:
    btfsc PORTE, 3
    goto re3release1
    incf re3buttoncounter
    goto re3endcheck
    
re3press2:
    btfss PORTE, 3
    goto re3endcheck
re3release2:   
    btfsc PORTE, 3
    goto re3release2
    incf re3buttoncounter
    goto re3endcheck
    
re3press3:
    btfss PORTE, 3
    goto re3endcheck
re3release3:
    btfsc PORTE, 3
    goto re3release3
    incf re3buttoncounter
    goto re3endcheck
    
re3press4:
    btfss PORTE, 3
    goto re3endcheck
re3release4:
    btfsc PORTE, 3
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
    goto portcre4release1 
    ;goto portdcheck
    ;portdcheck:
    movlw h'03'
    cpfsgt re3buttoncounter
    goto portdcalc ; TODO: NOT IMPLEMENTED YET
    subwf re3buttoncounter
    goto determineport
    
; RE4 LOOP FOR PORTB
    
portbre4endcheck:
    btfss PORTE, 3
    goto portbre4countcheck 
    ;incf re3buttoncounter
    goto re3endcheck ; or re3countcheck ????
    
portbre4countcheck:
    movlw h'00'
    cpfsgt portbvalue
    ;goto re3endcheck ; re4press1
    goto portbre4endcheck
    
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
    btfsc PORTE, 4
    goto portbre4release1
    incf portbvalue
    movlw h'01' ; TURN ON RB0
    movwf LATB
    goto portbre4endcheck
    
portbre4press2:
    btfss PORTE, 4
    goto portbre4endcheck
portbre4release2:
    btfsc PORTE, 4
    goto portbre4release2
    incf portbvalue
    movlw h'03' ; TURN ON RB0, RB1
    movwf LATB
    goto portbre4endcheck
    
portbre4press3:
    btfss PORTE, 4
    goto portbre4endcheck
portbre4release3:
    btfsc PORTE, 4
    goto portbre4release3
    incf portbvalue
    movlw h'07' ; TURN ON RB0, RB1, RB2
    movwf LATB
    goto portbre4endcheck
    
portbre4press4:
    btfss PORTE, 4
    goto portbre4endcheck
portbre4release4:
    btfsc PORTE, 4
    goto portbre4release4
    incf portbvalue
    movlw h'0F' ; TURN ON RB0, RB1, RB2, RB3
    movwf LATB
    goto portbre4endcheck
    
portbre4press5:
    btfss PORTE, 4
    goto portbre4endcheck
portbre4release5:
    btfsc PORTE, 4
    goto portbre4release5
    clrf portbvalue
    ;movlw h'00' ; TURN OFF ALL RB LEDS
    clrf LATB
    goto portbre4endcheck
    
 
; RE4 LOOP FOR PORTC
    
portcre4endcheck:
    btfss PORTE, 3
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
    btfsc PORTE, 4
    goto portcre4release1
    incf portcvalue
    movlw h'01' ; TURN ON RC0
    movwf LATC
    goto portcre4endcheck ; NOT IMPLEMENTED YET
    
portcre4press2:
    btfss PORTE, 4
    goto portcre4endcheck
portcre4release2:
    btfsc PORTE, 4
    goto portcre4release2
    incf portcvalue
    movlw h'03' ; TURN ON RC0, RC1
    movwf LATC
    goto portcre4endcheck
    
portcre4press3:
    btfss PORTE, 4
    goto portcre4endcheck
portcre4release3:
    btfsc PORTE, 4
    goto portcre4release3
    incf portcvalue
    movlw h'07' ; TURN ON RC0, RC1, RC2
    movwf LATC
    goto portcre4endcheck
    
portcre4press4:
    btfss PORTE, 4
    goto portcre4endcheck
portcre4release4:
    btfsc PORTE, 4
    goto portcre4release4
    incf portcvalue
    movlw h'0F' ; TURN ON RC0, RC1, RC2, RC3
    movwf LATC
    goto portcre4endcheck
    
portcre4press5:
    btfss PORTE, 4
    goto portcre4endcheck
portcre4release5:
    btfsc PORTE, 4
    goto portcre4release5
    clrf portcvalue
    clrf LATC ; TURN OFF ALL RC LEDS
    goto portcre4endcheck
    

portdcalc:   ; operation
    btfsc PORTE, 3
    goto portdcalc
    movlw h'00'
    cpfseq ra4buttoncounter
    goto addcheck
    goto ra4endcheck ; ra4press1 maybe ??
    addcheck:
    movlw h'01' ; addition
    cpfseq ra4buttoncounter
    goto subcheck
    goto addition 
    subcheck:
    movlw h'02' ; subtraction
    cpfseq ra4buttoncounter
    goto finalcheck
    goto subtraction 
    finalcheck:
    cpfsgt ra4buttoncounter
    goto portdcalc
    decf ra4buttoncounter
    decf ra4buttoncounter
    goto portdcalc
    
addition:
    movf portbvalue, 0
    addwf portcvalue, 0
    movwf portdvalue
    goto turnonportdleds 
    
subtraction:
    movf portbvalue, 0
    cpfsgt portcvalue
    goto subcfromb  
    goto subbfromc 
    
subcfromb:
    subfwb portcvalue, 0
    movwf portdvalue
    goto turnonportdleds
    
subbfromc:
    subwf portcvalue, 0
    movwf portdvalue
    goto turnonportdleds
    

turnonportdleds:
    goto ledstord7
    
finalcall:
    call delay
    clrf LATB
    clrf LATC
    clrf LATD
    goto main
    
ledstord7:    
    movlw h'08'
    cpfseq portdvalue
    goto ledstord6
    movlw h'FF'
    movwf LATD
       
ledstord6:
    movlw h'07'
    cpfseq portdvalue
    goto ledstord5
    movlw h'7F'
    movwf LATD

ledstord5:
    movlw h'06'
    cpfseq portdvalue
    goto ledstord4
    movlw h'3F'
    movwf LATD
    
ledstord4:
    movlw h'05'
    cpfseq portdvalue
    goto ledstord3
    movlw h'1F'
    movwf LATD
    
ledstord3:
    movlw h'04'
    cpfseq portdvalue
    goto ledstord2
    movlw h'0F'
    movwf LATD

ledstord2:
    movlw h'03'
    cpfseq portdvalue
    goto ledstord1
    movlw h'07'
    movwf LATD

ledstord1:
    movlw h'02'
    cpfseq portdvalue
    goto ledstord0
    movlw h'03'
    movwf LATD
    
ledstord0:
    movlw h'01'
    cpfseq portdvalue
    goto finalcall
    movlw h'01'
    movwf LATD
    goto finalcall

end
    
    
    
    
    
    
    
    



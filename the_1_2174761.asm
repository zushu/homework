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
portbvalue equ 0x06
portcvalue equ 0x07
portdvalue equ 0x08
opcode equ 0x09

    ORG 0x00
    ;goto init
    goto main
    
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
    movwf portbvalue
    movwf portcvalue
    movwf portdvalue
    return
    
    
    
main:
    call init
    call turnonleds
    call buttoncountercheck
    ;call portselectioncheck
    goto main
    
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
    
    
portselectioncheck:
    movlw h'00'
    cpfsgt re3buttoncounter
    goto re3press1
    goto check2
    
    check2:
    movlw h'01'
    cpfsgt re3buttoncounter
    goto re3press2
    goto check3
    
    check3:
    movlw h'02'
    cpfsgt re3buttoncounter
    goto re3press3
    goto check4
    
    check4:
    movlw h'03'
    cpfsgt re3buttoncounter
    goto re3press4
    goto check5
    
    check5:
    subwf re3buttoncounter
    goto portselectioncheck
    
re3press1:
    btfsc PORTE, 3
    goto re3press1
    goto re3release1
 
re3release1:
    btfss PORTE, 3
    goto re3release1
    incf re3buttoncounter
    goto re3press2
    
re3press2:
    btfsc PORTE, 3
    goto portbselection
    goto re3release2
    
re3release2:
    btfss PORTE, 3
    goto re3release2
    incf re3buttoncounter
    
re3press3:
    btfsc PORTE, 3
    goto portcselection
    goto re3release3
    
re3release3:
    btfss PORTE, 3
    goto re3release3
    incf re3buttoncounter
    goto re3press4
    
re3press4:
    btfsc PORTE, 3
    goto portdselection
    goto re3release4
    
re3release4:
    btfss PORTE, 3
    goto re3release4
    clrf re3buttoncounter
    incf re3buttoncounter
    goto portselectioncheck
    
portbselection:
    ;clrf LATB
    ;clrf PORTB
    btfsc PORTE, 3
    goto re4press1
    incf re3buttoncounter
    goto portselectioncheck
    
    
portcselection:
    ;clrf LATC
    ;clrf PORTC
    btfsc PORTE, 3
    goto re4portcpress1
    incf re3buttoncounter
    goto portselectioncheck
    ;goto re3press4
    
portdselection:
    ;clrf LATD
    ;clrf PORTD
    btfsc PORTE, 3
    goto operation
    incf re3buttoncounter
    goto portselectioncheck
    
; for PORTB
re4press1:
    btfsc PORTE, 4
    goto portselectioncheck ; ????
    goto re4release1
    
re4release1:
    btfss PORTE, 4
    goto re4release1
    movlw h'01'		    ; b'00000001'
    incf portbvalue	    ; 1 led RB0 - replace with latb value assignment to turn on the led
    movf portbvalue, 0
    movwf LATB		    ; turn on RB0 
    goto re4press2
    
re4press2:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4release2
    
re4release2:
    btfss PORTE, 4
    goto re4release2
    
    incf portbvalue	    ; 2 leds - RB1
    ;movf portbvalue, 0
    movlw h'03'
    movwf LATB		    ; turn on RB0, RB1
    goto re4press3
    
re4press3:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4release3
    
re4release3:
    btfss PORTE, 4
    goto re4release3
    incf portbvalue	    ; 3 leds - RB2
    ;movf portbvalue, 0
    movlw h'07'
    movwf LATB		    ; turn on RB0, RB1, RB2
    goto re4press4
    
re4press4:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4release4
    
re4release4:
    btfss PORTE, 4
    goto re4release4
    incf portbvalue	    ; 4 leds, RB3
    ;movf portbvalue, 0	    ; TODO change to set bit
    movlw h'0F'
    movwf LATB		    ; turn on RB0, RB1, RB2, RB3
    goto re4press5
    
re4press5:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4release5
    
re4release5:
    btfss PORTE, 4
    goto re4release5
    clrf portbvalue	    ; 5 presses, clear PORTB
    goto portbselection
 
    
; for PORTC
re4portcpress1:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4portcrelease1
    
re4portcrelease1:
    btfss PORTE, 4
    goto re4portcrelease1
    incf portcvalue ; RC0
    ;movf portcvalue, 0
    movlw h'01'
    movwf LATC		    ; turn on RC0
    goto re4portcpress2
    
re4portcpress2:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4portcrelease2
    
re4portcrelease2:
    btfss PORTE, 4
    goto re4portcrelease2
    incf portcvalue ; RC1
    ;movf portcvalue, 0
    movlw h'03'
    movwf LATC		    ; turn on RC0, RC1
    goto re4portcpress3
    
re4portcpress3:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4portcrelease3
    
re4portcrelease3:
    btfss PORTE, 4
    goto re4portcrelease3
    incf portcvalue ; RC2
    ;movf portcvalue, 0
    movlw h'07'
    movwf LATC		    ; turn on RC0, RC1, RC2
    goto re4portcpress4
    
re4portcpress4:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4portcrelease4
    
re4portcrelease4:
    btfss PORTE, 4
    goto re4portcrelease4
    incf portcvalue ; RC3
    ;movf portcvalue, 0
    movlw h'0F'
    movwf LATC		    ; turn on RC0, RC1, RC2, RC3
    goto re4portcpress5
    
re4portcpress5:
    btfsc PORTE, 4
    goto portselectioncheck
    goto re4portcrelease5
    
re4portcrelease5:
    btfss PORTE, 4
    goto re4portcrelease5
    clrf portcvalue
    goto portcselection
       
addition:
    movlw h'05'
    movwf opcode
    goto portselectioncheck
    ;movlw h'05'
    ;movwf opcode
    ;goto operation
    
subtraction:
    movlw h'0A'
    movwf opcode
    goto portselectioncheck
    ;movlw h'0A'
    ;movwf opcode
    ;goto operation
   
operation:
    movlw h'05'
    cpfseq opcode
    goto opsubtraction
    goto opaddition
    
opaddition:
    movf portbvalue, 0
    addwf portcvalue, 0	    ; d = 0, store result back in the WREG
    ;movwf LATD		    ; turn on RD pins
    ;call delay
    ;clrf LATD
    movwf portdvalue
    goto turnonportdleds
    
opsubtraction:
    movf portbvalue, 0	    ; movfw
    cpfsgt portcvalue	    ; skip the next instruction if portbvalue is greater than portcvalue
    goto subcfromb
    goto subbfromc
    
subcfromb:
    subfwb portcvalue, 0    ; portbvalue in WREG
    ;movwf LATD		    ; turn on RD pins
    movwf portdvalue
    goto turnonportdleds
    ;return
    
subbfromc:
    subwf portcvalue, 0	    ; portbvalue in WREG
    ;movwf LATD		    ; turn on RD pins
    movwf portdvalue
    goto turnonportdleds
    ;return
    
turnonportdleds:
    ;movf portdvalue, 0 ; TODO : you should use set bit
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
    
    
    
    
    
    
    
    



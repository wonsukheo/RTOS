; Wait Library
; Jason Losh

;-----------------------------------------------------------------------------
; Hardware Target
;-----------------------------------------------------------------------------

; Target Platform: EK-TM4C123GXL
; Target uC:       TM4C123GH6PM
; System Clock:    40 MHz

; Hardware configuration:
; 16 MHz external crystal oscillator

;-----------------------------------------------------------------------------
; Device includes, defines, and assembler directives
;-----------------------------------------------------------------------------

    .def waitMicrosecond

;-----------------------------------------------------------------------------
; Register values and large immediate values
;-----------------------------------------------------------------------------

.thumb
.text

;EVERY INSTRUCTION TAKES 1 CLOCK 

;BRANCH UNCONDITIONAL
;BRANCH TAKES 2 CYCLE BECAUSE OF FETCH INSTRUCTION FLUSHING

;BRANCH CONDITIONAL
;DECODER FACE CONDITIONAL STATEMENT. SO UNDECIDED UNTIL EXECUTE PREV INSTRUCTION
;BRANCH IF SUCK UP 3 CLOCK CYCLE. 

;READ 1CYLCE + SUB 1CYCLE, WRITE 1 CYCLE
; waitMicrosec(1) R0 = 1 at start ; when you call this function bl foo() ->branch 2 clock
; waitMicrosec(N) R0 = N, 40N + 3

waitMicrosecond:
WMS_LOOP0:   MOV  R1, #6          ; 1
WMS_LOOP1:   SUB  R1, R1, #1      ; 6
             CBZ  R1, WMS_DONE1   ; 5+1*3
             NOP                  ; 5
             NOP                  ; 5
             B    WMS_LOOP1       ; 5*2
WMS_DONE1:   SUB  R0, R0, #1      ; 1
             CBZ  R0, WMS_DONE0   ; 1
             NOP                  ; 1
             B    WMS_LOOP0       ; 1*2			; BRANCH 2 CLOCK
WMS_DONE0:   BX   LR              ; ---
                                  ; 41 clocks/us

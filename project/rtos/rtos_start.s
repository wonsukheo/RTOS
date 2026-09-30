;-----------------------------------------------------------------------------
; rtos_start.s
; Low-level assembly helpers for PSP and CONTROL register
;-----------------------------------------------------------------------------
        .text
        .global setPsp
        .global getPsp
        .global setAsp
        .global launchThread

; void launchThread(uint32_t stackTop, void (*thread)(void))
; R0 = stackTop, R1= function ptr
launchThread:
        MSR     PSP, R0
        MRS     R2, CONTROL
        ORR     R2, R2, #0x02
        MSR     CONTROL, R2
        ISB
        BX      R1

; By AAPCS, the first argument (uint32_t *pStack) is passed in R0
setPsp:
        MSR     PSP, R0
        BX      LR

; Returns the current PSP in R0
getPsp:
        MRS     R0, PSP
        BX      LR

; Switch thread stack to PSP by setting bit 1 (ASP) in CONTROL
setAsp:
        MRS     R0, CONTROL
        ORR     R0, R0, #0x02
        MSR     CONTROL, R0
        ISB
        BX      LR

        .end

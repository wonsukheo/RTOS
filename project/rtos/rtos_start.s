.syntax unified
.thumb
.text

.global setPsp
.global getPsp
.global setAsp

// by AAPCS first argument(stack_addr) stored in R0
setPsp:
    MSR PSP, R0
    BX LR

getPsp:
    MRS R0, PSP
    BX LR

setAsp:
    MRS R0, CONTROL
    ORR R0, R0, #0x02
    MSR CONTROL, R0
    ISB

    BX LR

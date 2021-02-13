/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */
 
.cpu cortex-m4
.syntax unified
.thumb
.text

.equ    SCB_VTOR,           0xE000ED08              /* Vector Table Offset Register */
.equ    NVIC_INT_CTRL,      0xE000ED04              /* interrupt control state register */
.equ    NVIC_SYSPRI2,       0xE000ED20              /* system priority register (2) */
.equ    NVIC_PENDSV_PRI,    0x00FF0000              /* PendSV priority value (lowest) */
.equ    NVIC_PENDSVSET,     0x10000000              /* value to trigger PendSV exception */

.global switch_interrupt_flag
.global interrupt_from_task
.global interrupt_to_task
.global kernel_running
.global interrupt_nest

/*
 * __addr_t disable_irq_save();
 */
.global disable_irq_save
.type disable_irq_save, %function
disable_irq_save:
    mrs     r0, PRIMASK
    cpsid   I
    bx      lr

/*
 * void enable_irq_save(__addr_t level);
 */
    .global enable_irq_save
    .type enable_irq_save, %function
enable_irq_save:
    msr     PRIMASK, r0
    bx      lr

    .global context_switch_interrupt
    .type context_switch_interrupt, %function
    .global context_switch
    .type context_switch, %function
context_switch_interrupt:
context_switch:
    /* set switch_interrupt_flag to 1 */
    LDR     R2, =switch_interrupt_flag
    LDR     R3, [R2]
    CMP     R3, #1
    BEQ     _reswitch
    MOV     R3, #1
    STR     R3, [R2]

    LDR     R2, =interrupt_from_task  
    STR     R0, [R2]

_reswitch:
    LDR     R2, =interrupt_to_task  
    STR     R1, [R2]

    LDR     R0, =NVIC_INT_CTRL         
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

/* R0 --> switch from task stack
 * R1 --> switch to task stack
 * psr, pc, LR, R12, R3, R2, R1, R0 are pushed into [from] stack
 */
    .global PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    /* disable interrupt to protect context switch */
    MRS     R2, PRIMASK
    CPSID   I

    /* get switch_interrupt_flag */
    LDR     R0, =switch_interrupt_flag
    LDR     R1, [R0]
    CBZ     R1, pendsv_exit         /* pendsv aLReady handled */

    /* clear switch_interrupt_flag to 0 */
    MOV     R1, #0
    STR     R1, [R0]

    LDR     R0, =interrupt_from_task
    LDR     R1, [R0]
    CBZ     R1, switch_to_task      /* skip register save at the first time */

    MRS     R1, PSP                 /* get from task stack pointer */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    TST     lr, #0x10           /* if(!EXC_RETURN[4]) */
    VSTMDBEQ r1!, {d8 - d15}    /* push FPU register s16~s31 */
#endif
    STMFD   R1!, {R4 - R11}         /* push R4 - R11 register */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    MOV     r4, #0x00           /* flag = 0 */
    TST     lr, #0x10           /* if(!EXC_RETURN[4]) */
    MOVEQ   r4, #0x01           /* flag = 1 */
    STMFD   r1!, {r4}           /* push flag */
#endif
    LDR     R0, [R0]
    STR     R1, [R0]                /* update from task stack pointer */

switch_to_task:
    LDR     R1, =interrupt_to_task
    LDR     R1, [R1]
    LDR     R1, [R1]                /* load task stack pointer */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    LDMFD   r1!, {r3}           /* pop flag */
#endif
    LDMFD   R1!, {R4 - R11}         /* pop R4 - R11 register */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    CMP     r3,  #0             /* if(flag_r3 != 0) */
    VLDMIANE  r1!, {d8 - d15}   /* pop FPU register s16~s31 */
#endif
    MSR     PSP, R1                 /* update stack pointer */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    ORR     lr, lr, #0x10       /* lr |=  (1 << 4), clean FPCA. */
    CMP     r3,  #0             /* if(flag_r3 != 0) */
    BICNE   lr, lr, #0x10       /* lr &= ~(1 << 4), set FPCA. */
#endif

pendsv_exit:
    /* restore interrupt */
    MSR     PRIMASK, R2

    ORR     LR, LR, #0x04
    BX      LR


/*
 * void context_switch_to(__addr_t to);
 * R0 --> to
 */
.global context_switch_to
.type context_switch_to, %function
context_switch_to:
    LDR     R1, =interrupt_to_task
    STR     R0, [R1]

#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    /* CLEAR CONTROL.FPCA */
    MRS     r2, CONTROL         /* read */
    BIC     r2, #0x04           /* modify */
    MSR     CONTROL, r2         /* write-back */
#endif

    /* set from task to 0 */
    LDR     R1, =interrupt_from_task
    MOV     R0, #0
    STR     R0, [R1]

    /* set interrupt flag to 1 */
    LDR     R1, =switch_interrupt_flag
    MOV     R0, #1
    STR     R0, [R1]

    /* set kernel running to 1 */
    LDR     R1, =kernel_running
    MOV     R0, #1
    STR     R0, [R1]

    /* set the PendSV exception priority */
    LDR     R0, =NVIC_SYSPRI2
    LDR     R1, =NVIC_PENDSV_PRI
    LDR.W   R2, [R0,#0]             /* read */
    ORR     R1, R1, R2              /* modify */
    STR     R1, [R0]                /* write-back */

    LDR     R0, =NVIC_INT_CTRL               /* trigger the PendSV exception (causes context switch) */
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    /* restore MSP */
    LDR     r0, =SCB_VTOR
    LDR     r0, [r0]
    LDR     r0, [r0]
    NOP
    MSR     msp, r0

    /* enable interrupts at processor level */
    CPSIE   F
    CPSIE   I

    /* never reach here! */
loop:
    b loop

.global HardFault_Handler
.type HardFault_Handler, %function
HardFault_Handler:
    ldr     r1, =interrupt_nest
    mov     r2, #1
    str     r2, [r1]
    /* get current context */
    MRS     r0, msp                 /* get fault context from handler. */
    TST     lr, #0x04               /* if(!EXC_RETURN[2]) */
    BEQ     _get_sp_done
    MRS     r0, psp                 /* get fault context from task. */
_get_sp_done:

    STMFD   r0!, {r4 - r11}         /* push r4 - r11 register */
#if defined (__VFP_FP__) && !defined(__SOFTFP__)
    STMFD   r0!, {lr}               /* push dummy for flag */
#endif
    STMFD   r0!, {lr}               /* push exec_return register */

    TST     lr, #0x04               /* if(!EXC_RETURN[2]) */
    BEQ     _update_msp
    MSR     psp, r0                 /* update stack pointer to PSP. */
    B       _update_done
_update_msp:
    MSR     msp, r0                 /* update stack pointer to MSP. */
_update_done:

    PUSH    {LR}
    BL      cpu_hard_fault
    POP     {LR}

    ORR     LR, LR, #0x04
    BX      LR

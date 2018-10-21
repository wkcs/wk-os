/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */
 
    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb
    .text

    .global switch_interrupt_flag
    .global interrupt_from_task
    .global interrupt_to_task

    .equ    SCB_VTOR, 0xE000ED08            /* Vector Table Offset Register */
    .equ    ICSR, 0xE000ED04                /* interrupt control state register */
    .equ    PENDSVSET_BIT, 0x10000000       /* value to trigger PendSV exception */
    
    .equ    SHPR3, 0xE000ED20               /* system priority register (3) */
    .equ    PENDSV_PRI_LOWEST, 0x00FF0000   /* PendSV priority value (lowest) */

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

    LDR     R0, =ICSR         
    LDR     R1, =PENDSVSET_BIT
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
    STMFD   R1!, {R4 - R11}         /* push R4 - R11 register */
    LDR     R0, [R0]
    STR     R1, [R0]                /* update from task stack pointer */

switch_to_task:
    LDR     R1, =interrupt_to_task
    LDR     R1, [R1]
    LDR     R1, [R1]                /* load task stack pointer */

    LDMFD   R1!, {R4 - R11}         /* pop R4 - R11 register */
    MSR     PSP, R1                 /* update stack pointer */

pendsv_exit:
    /* restore interrupt */
    MSR     PRIMASK, R2

    ORR     LR, LR, #0x04
    BX      LR

/*
 * void context_switch_to(__addr_t *to);
 * R0 --> to
 */
    .global context_switch_to
    .type context_switch_to, %function
context_switch_to:
    LDR     R1, =interrupt_to_task
    STR     R0, [R1]

    /* set from task to 0 */
    LDR     R1, =interrupt_from_task
    MOV     R0, #0
    STR     R0, [R1]

    /* set interrupt flag to 1 */
    LDR     R1, =switch_interrupt_flag
    MOV     R0, #1
    STR     R0, [R1]

    /* set the PendSV exception priority */
    LDR     R0, =SHPR3
    LDR     R1, =PENDSV_PRI_LOWEST
    LDR.W   R2, [R0,#0]             /* read */
    ORR     R1, R1, R2              /* modify */
    STR     R1, [R0]                /* write-back */

    LDR     R0, =ICSR               /* trigger the PendSV exception (causes context switch) */
    LDR     R1, =PENDSVSET_BIT
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


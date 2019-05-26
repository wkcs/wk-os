/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/sch.h>
#include <wk/task.h>
#include <wk/irq.h>
#include <wk/clk.h>
#include <cmd/cmd.h>

#include <board.h>

#if (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__))
#define USE_FPU   1
#else
#define USE_FPU   0
#endif

struct exception_stack_frame {
    addr_t r0;
    addr_t r1;
    addr_t r2;
    addr_t r3;
    addr_t r12;
    addr_t lr;
    addr_t pc;
    addr_t psr;
};

struct stack_frame {
#if USE_FPU
    uint32_t flag;
#endif
    addr_t r4;
    addr_t r5;
    addr_t r6;
    addr_t r7;
    addr_t r8;
    addr_t r9;
    addr_t r10;
    addr_t r11;

    struct exception_stack_frame exception_stack_frame;
};

struct cpu_dump_type {
    addr_t sp;
    struct stack_frame stack_frame;
};

static void reboot_to_bootloader(void)
{
    uint8_t argc = 1;
    char *argv[1];

    argv[0] = "bootloader";

    cmd_run("reboot", argc, argv);
}

addr_t *stack_init(void *task_entry, void *parameter, addr_t *stack_addr, void *task_exit)
{
    struct stack_frame *stack_frame;
    uint8_t *stk;
    uint8_t i;

    stk = (uint8_t *)stack_addr + sizeof(addr_t);
    stk -= sizeof(struct stack_frame);
    stack_frame = (struct stack_frame *)stk;

    for (i = 0; i < sizeof(struct stack_frame) / sizeof(addr_t); i++)
        ((addr_t *)stack_frame)[i] = 0x0badc0de;

    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter;     /* r0 : argument */
    stack_frame->exception_stack_frame.r1  = 0;                            /* r1 */
    stack_frame->exception_stack_frame.r2  = 0;                            /* r2 */
    stack_frame->exception_stack_frame.r3  = 0;                            /* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                            /* r12 */
    stack_frame->exception_stack_frame.lr  = (unsigned long)task_exit;     /* lr */
    stack_frame->exception_stack_frame.pc  = (unsigned long)task_entry;    /* entry point, pc */
    stack_frame->exception_stack_frame.psr = 0x01000000L; 

#if USE_FPU
    stack_frame->flag = 0;
#endif

    return (addr_t *)stk;
}

void NMI_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
    reboot_to_bootloader();
}
 
void cpu_hard_fault(struct cpu_dump_type *cpu_dump_type)
{
    pr_info("-------------------\r\n");
    pr_info("r0  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.r0);
    pr_info("r1  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.r1);
    pr_info("r2  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.r2);
    pr_info("r3  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.r3);
    pr_info("r4  = 0x%08x\r\n", cpu_dump_type->stack_frame.r4);
    pr_info("r5  = 0x%08x\r\n", cpu_dump_type->stack_frame.r5);
    pr_info("r6  = 0x%08x\r\n", cpu_dump_type->stack_frame.r6);
    pr_info("r7  = 0x%08x\r\n", cpu_dump_type->stack_frame.r7);
    pr_info("r8  = 0x%08x\r\n", cpu_dump_type->stack_frame.r8);
    pr_info("r9  = 0x%08x\r\n", cpu_dump_type->stack_frame.r9);
    pr_info("r10 = 0x%08x\r\n", cpu_dump_type->stack_frame.r10);
    pr_info("r11 = 0x%08x\r\n", cpu_dump_type->stack_frame.r11);
    pr_info("r12 = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.r12);
    pr_info("lr  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.lr);
    pr_info("pc  = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.pc);
    pr_info("psr = 0x%08x\r\n", cpu_dump_type->stack_frame.exception_stack_frame.psr);
    pr_info("-------------------\r\n");
    if (cpu_dump_type->sp & 1 << 2) {
        struct task_struct_t *task = get_current_task();
        pr_info("cpu hard fault on task(name = %s)\r\n", task->name);
    } else {
        pr_info("cpu hard fault on handler\r\n");
    }

    dump_all_task();
    reboot_to_bootloader();
}
 
void MemManage_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
    /* Go to infinite loop when Memory Manage exception occurs */
    reboot_to_bootloader();
}

 
void BusFault_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
    /* Go to infinite loop when Bus Fault exception occurs */
    reboot_to_bootloader();
}
 
void UsageFault_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
    /* Go to infinite loop when Usage Fault exception occurs */
    reboot_to_bootloader();
}
 
void SVC_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
}
 
void DebugMon_Handler(void)
{
    pr_fatal("%s entry\r\n", __func__);
}

extern uint32_t SystemCoreClock;

uint32_t sys_tick_num_by_us;
uint32_t sys_tick_num_by_beat;
__init void asm_cpu_init(void)
{
    sys_tick_num_by_us = SystemCoreClock / 1000000;
    sys_tick_num_by_beat = SystemCoreClock / TICK_PER_SECOND;
}

void cpu_delay_usec(uint32_t usec)
{
    register uint32_t ticks;
    register uint32_t told, tnow;
    register uint32_t tcnt = 0;
    register uint32_t reload = SysTick->LOAD;

    ticks = usec * sys_tick_num_by_us;
    tcnt = 0;

    told = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks)
                break;
        }
    }
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    wk_interrupt_enter();

    if (kernel_running)
        system_beat_processing();

    /* leave interrupt */
    wk_interrupt_leave();
}

void cpu_reboot(void)
{
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

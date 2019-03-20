/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/sch.h>
#include <wk/task.h>
#include <wk/irq.h>
#include <wk/clk.h>

#include <board.h>

struct cpu_reg {
    addr_t r4;
    addr_t r5;
    addr_t r6;
    addr_t r7;
    addr_t r8;
    addr_t r9;
    addr_t r10;
    addr_t r11;

    addr_t r0;
    addr_t r1;
    addr_t r2;
    addr_t r3;
    addr_t r12;
    addr_t lr;
    addr_t pc;
    addr_t psr;
};

struct cpu_dump_type {
    addr_t sp;
    struct cpu_reg cpu_reg;
};

addr_t *stack_init(void *task_entry, void *parameter, addr_t *stack_addr, void *task_exit)
{
    addr_t *stk;
    uint8_t i = 0;

    stk  = stack_addr;
    
    *stk = 0x01000000L;                                /* PSR */
    *(--stk) = (unsigned long)task_entry;              /* pc */
    *(--stk) = (unsigned long)task_exit;               /* lr */
    *(--stk) = 0;                                      /* r12 */
    *(--stk) = 0;                                      /* r3 */
    *(--stk) = 0;                                      /* r2 */
    *(--stk) = 0;                                      /* r1 */
    *(--stk) = (unsigned long)parameter;               /* r0 */
    
    /*r4 - r11*/
    for(; i < 8; i++)
        *(--stk) = 0x0badc0de;

    return stk;
}

void NMI_Handler(void)
{
}
 
void cpu_hard_fault(struct cpu_dump_type *cpu_dump_type)
{
    pr_info("-------------------\r\n");
    pr_info("r0  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r0);
    pr_info("r1  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r1);
    pr_info("r2  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r2);
    pr_info("r3  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r3);
    pr_info("r4  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r4);
    pr_info("r5  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r5);
    pr_info("r6  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r6);
    pr_info("r7  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r7);
    pr_info("r8  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r8);
    pr_info("r9  = 0x%08x\r\n", cpu_dump_type->cpu_reg.r9);
    pr_info("r10 = 0x%08x\r\n", cpu_dump_type->cpu_reg.r10);
    pr_info("r11 = 0x%08x\r\n", cpu_dump_type->cpu_reg.r11);
    pr_info("r12 = 0x%08x\r\n", cpu_dump_type->cpu_reg.r12);
    pr_info("lr  = 0x%08x\r\n", cpu_dump_type->cpu_reg.lr);
    pr_info("pc  = 0x%08x\r\n", cpu_dump_type->cpu_reg.pc);
    pr_info("psr = 0x%08x\r\n", cpu_dump_type->cpu_reg.psr);
    pr_info("-------------------\r\n");
    if (cpu_dump_type->sp & 1 << 2) {
        struct task_struct_t *task = get_current_task();
        pr_info("cpu hard fault on task(name = %s)\r\n", task->name);
    } else {
        pr_info("cpu hard fault on handler\r\n");
    }

    dump_all_task();

  while (1);
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
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
    register uint32_t tcnt=0;
	register uint32_t reload = SysTick->LOAD;

	ticks = usec * sys_tick_num_by_us;
	tcnt = 0;

    told = SysTick->VAL;
	while(1) {
		tnow = SysTick->VAL;	
		if (tnow != told) {	    
			if (tnow < told)
                tcnt += told - tnow;
			else 
                tcnt += reload - tnow + told;	    
			told = tnow;
			if (tcnt >= ticks) {
                break;
            }
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

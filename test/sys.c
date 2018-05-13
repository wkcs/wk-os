#include "sys.h"

//采用如下方法实现执行汇编指令WFI
void WFI_SET(void)
{
	__ASM volatile ("wfi");
}
//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__ASM volatile ("cpsid i" : : : "memory");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile ("cpsie i" : : : "memory");
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr)
{
	__ASM volatile ("MSR msp, %0\n" : : "r" (addr) : "sp");
}
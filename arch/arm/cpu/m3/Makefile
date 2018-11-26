##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

# 架构相关编译指令
#使用硬件fpu
#FP_FLAGS	= -mfloat-abi=hard -mfpu=fpv4-sp-d16
FP_FLAGS	= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3

DEBUG_DIR += $(TOP_DIR)/debug/arch/arm/cpu/m3

LDSCRIPT += $(TOP_DIR)/arch/arm/cpu/m3/wk_kernel.ld

C_SRCS += $(TOP_DIR)/arch/arm/cpu/m3/cpu_c.c
C_SRCS += $(TOP_DIR)/arch/arm/cpu/m3/asm_irq.c

ASM_SRCS += $(TOP_DIR)/arch/arm/cpu/m3/cpu_s.S
ASM_SRCS += $(TOP_DIR)/arch/arm/cpu/m3/start.S

INC_DIR += -I$(TOP_DIR)/debug/arch/arm/cpu/m3




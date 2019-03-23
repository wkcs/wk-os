##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

DEFS += -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER

DEBUG_DIR += $(TOP_DIR)/debug/arch/arm/board/zj_v3

include $(TOP_DIR)/arch/arm/board/zj_v3/drivers/Makefile

C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_dma.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_uart.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_clk.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_irq.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_gpio.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/board_config.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/board_init.c
C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/arch_console.c

#C_SRCS += $(TOP_DIR)/arch/arm/board/zj_v3/system_stm32f10x.c

ASM_SRCS += 

INC_DIR += -I $(TOP_DIR)/arch/arm/board/zj_v3/include




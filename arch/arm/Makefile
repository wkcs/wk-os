##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

DEBUG_DIR += $(TOP_DIR)/debug/arch/arm

include $(TOP_DIR)/arch/arm/boot/Makefile
include $(TOP_DIR)/arch/arm/cmsis/Makefile

C_SRCS += $(TOP_DIR)/arch/arm/cpu_c.c

ASM_SRCS += $(TOP_DIR)/arch/arm/cpu_s.S
ASM_SRCS += $(TOP_DIR)/arch/arm/startup_stm32f10x_hd.S

INC_DIR += -I $(TOP_DIR)/arch/arm/include




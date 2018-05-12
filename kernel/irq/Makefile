##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

FILE_DIR = $(TOP_DIR)/kernel/irq

TEMP_C_SCRS = irq.c

TEMP_ASM_SCRS =

OBJS += $(TEMP_C_SCRS:%.c:$(TOP_DIR)/debug/objs/%.o)
OBJS += $(TEMP_ASM_SCRS:%.s:$(TOP_DIR)/debug/objs/%.o)

C_SCRS += $(TEMP_C_SCRS:%:$(FILE_DIR)/%)
ASM_SCRS += $(TEMP_ASM_SCRS:%:$(FILE_DIR)/%)


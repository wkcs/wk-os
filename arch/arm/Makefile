##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

FILE_DIR = $(TOP_DIR)/arch/arm

TEMP_C_SCRS = cpu_c.c

TEMP_ASM_SCRS = cpu_s.S

OBJS += $(TEMP_C_SCRS:%.c:$(TOP_DIR)/debug/objs/%.o)
OBJS += $(TEMP_ASM_SCRS:%.s:$(TOP_DIR)/debug/objs/%.o)

C_SCRS += $(TEMP_C_SCRS:%:$(FILE_DIR)/%)
ASM_SCRS += $(TEMP_ASM_SCRS:%:$(FILE_DIR)/%)




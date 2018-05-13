##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

TARGET = wk_os

RM := rm -rf

#打开显示选项
ifneq ($(V),1)
Q		    := @
endif

#优化等级
ifeq ($(OPT),0)
OPTSRC = -O0
else ifeq ($(OPT),1)
OPTSRC = -O1
else ifeq ($(OPT),2)
OPTSRC = -O2
else ifeq ($(OPT),3)
OPTSRC = -O3
else ifeq ($(OPT),s)
OPTSRC = -Os
else 
OPTSRC = -Og
endif


#定义工具链
PREFIX		:= arm-none-eabi
CC		    := $(PREFIX)-gcc
CXX		    := $(PREFIX)-g++
LD		    := $(PREFIX)-gcc
AR		    := $(PREFIX)-ar
AS		    := $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
SIZE        := $(PREFIX)-size
GDB		    := $(PREFIX)-gdb

# 架构相关编译指令
#使用硬件fpu
#FP_FLAGS	= -mfloat-abi=hard -mfpu=fpv4-sp-d16
FP_FLAGS	= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3


CCFLAGS = $(ARCH_FLAGS)
CCFLAGS += $(FP_FLAGS)
CCFLAGS += $(OPTSRC)
CCFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections                  \
           -fdata-sections -ffreestanding -fno-move-loop-invariants
CCFLAGS += -Wall -Wextra  -g3
CCFLAGS += -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c

ASFLAGS = $(ARCH_FLAGS)
ASFLAGS += $(FP_FLAGS)
ASFLAGS += $(OPTSRC)
ASFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections                  \
           -fdata-sections -ffreestanding -fno-move-loop-invariants
ASFLAGS += -Wall -Wextra  -g3 -x assembler-with-cpp
ASFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c

LDFLAGS = $(ARCH_FLAGS)
LDFLAGS += $(FP_FLAGS)
LDFLAGS += -specs=nano.specs -specs=nosys.specs -static
LDFLAGS += -Wl,--start-group -lc -lm -Wl,--end-group -Wl,-cref,-u,Reset_Handler -Wl,-Map=$(TOP_DIR)/debug/$(TARGET).map   \
           -Wl,--defsym=malloc_getpagesize_P=0x80

#-Wl,--gc-sections去除无用的段
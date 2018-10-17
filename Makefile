##############################################
# Copyright (C) 2018 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

#读取当前工作目录
TOP_DIR = .

include config.mk

# 宏定义
DEFS = -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER

#设定包含文件目录
INC_DIR = -I $(TOP_DIR)/include

DEBUG_DIR = $(TOP_DIR)/debug

# 展开工作子目录
include $(TOP_DIR)/arch/Makefile
include $(TOP_DIR)/drivers/Makefile
include $(TOP_DIR)/fs/Makefile
include $(TOP_DIR)/init/Makefile
include $(TOP_DIR)/kernel/Makefile
include $(TOP_DIR)/lib/Makefile
include $(TOP_DIR)/mm/Makefile
include $(TOP_DIR)/net/Makefile
include $(TOP_DIR)/scripts/Makefile
include $(TOP_DIR)/test/Makefile

C_OBJS = $(C_SRCS:./%.c=$(TOP_DIR)/debug/%.o)
ASM_OBJS = $(ASM_SRCS:./%.S=$(TOP_DIR)/debug/%.o)
OBJS = $(C_OBJS)
OBJS += $(ASM_OBJS)
DEPS = $(OBJS:%.o=%.d)

#链接脚本
LDSCRIPT    = $(TOP_DIR)/stm32_flash.ld

# OpenOCD specific variables
OOCD		:= openocd
OOCD_INTERFACE	:= flossjtag
OOCD_TARGET	:= stm32f4x
OOCDFLAGS := -f openocd.cfg
           

CCFLAGS += $(INC_DIR)
CCFLAGS += $(DEFS)



ASFLAGS += $(INC_DIR)
ASFLAGS += $(DEFS)


LDFLAGS += -T $(LDSCRIPT)

SECONDARY_FLASH = $(TOP_DIR)/debug/$(TARGET).hex
SECONDARY_SIZE = $(TOP_DIR)/debug/$(TARGET).size
SECONDARY_BIN = $(TOP_DIR)/debug/$(TARGET).bin
SECONDARY_ELF = $(TOP_DIR)/debug/$(TARGET).elf
SECONDARY_LIST = $(TOP_DIR)/debug/$(TARGET).list


.PHONY: all clean size bin hex debug stlink-flash flash debug_file


all: $(TARGET).images debug_file
	@echo   building done

size: $(SECONDARY_ELF)
	@echo   SIZE    $(SECONDARY_ELF)
	$(Q)$(SIZE) --format=berkeley "$(SECONDARY_ELF)"

bin: $(SECONDARY_BIN)

hex: $(SECONDARY_FLASH)


$(TARGET).images: $(SECONDARY_BIN) $(SECONDARY_FLASH) $(SECONDARY_LIST)
	$(Q)echo   images generated


$(SECONDARY_ELF): debug_dir $(OBJS) $(LDSCRIPT)
	@echo   LD      $(SECONDARY_ELF)
	$(Q)$(CC) $(LDFLAGS) -o "$(SECONDARY_ELF)" $(C_OBJS) $(ASM_OBJS)
	@echo   SIZE    $(SECONDARY_ELF)\n\n
	$(Q)$(SIZE) --format=berkeley "$(SECONDARY_ELF)"



$(SECONDARY_FLASH): $(SECONDARY_ELF)
	@echo   OBJCOPY $(SECONDARY_FLASH)
	$(Q)$(OBJCOPY) $(SECONDARY_ELF)  $(SECONDARY_FLASH) -Oihex


$(SECONDARY_BIN): $(SECONDARY_ELF)
	@echo   OBJCOPY $(SECONDARY_BIN)
	$(Q)$(OBJCOPY) $(SECONDARY_ELF)  $(SECONDARY_BIN) -Obinary

$(SECONDARY_LIST): $(SECONDARY_ELF)
	@echo   OBJDUMP $(SECONDARY_LIST)
	$(Q)$(OBJDUMP) -S $(SECONDARY_ELF) > $(SECONDARY_LIST)


$(C_OBJS):$(TOP_DIR)/debug/%.o:$(TOP_DIR)/%.c 
	@echo   CC      $<
	$(Q)$(CC) $(CCFLAGS) -o $@ $<

$(ASM_OBJS):$(TOP_DIR)/debug/%.o:$(TOP_DIR)/%.S 
	@echo   AS      $<
	$(Q)$(CC) $(ASFLAGS) -o $@ $<

#libs:
#	$(MAKE) -C $(LIB_MAKE_DIR) $@

# 使用stlink驱动下载bin程序
stlink-flash: $(SECONDARY_BIN)
	@echo   ST-link FLASH  $<
	$(Q)$(STFLASH) write $(SECONDARY_BIN) 0x8000000

# 使用OpenOCD下载hex程序
flash: $(SECONDARY_FLASH)
	@echo   OPEN_OCD FLASH $<
	$(Q)$(OOCD) $(OOCDFLAGS) -c "program $(SECONDARY_FLASH) verify reset exit" 

debug_dir:
	@echo   Create debug directory
	$(Q)-mkdir $(DEBUG_DIR)

# 使用GDB 通过sdtin/out管道与OpenOCD连接 并在main函数处打断点后运行
debug: $(SECONDARY_ELF)
	@echo   GDB DEBUG $<
	$(Q)$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
	-iex 'monitor reset halt' -ex 'load' -ex 'break rc_test_loop' -ex 'c' $(SECONDARY_ELF)

-include $(DEPS)

clean:
	@echo   CLEAN
	$(Q)-$(RM) $(TOP_DIR)/debug
	
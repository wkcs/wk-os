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

# 宏定义
DEFS		= -D STM32F40_41xxx -D USE_STDPERIPH_DRIVER -D __VFP_FP__ -D USE_USB_OTG_FS 

#链接脚本
LDSCRIPT    = $(TOP_DIR)/stm32_flash.ld

# OpenOCD specific variables
OOCD		:= openocd
OOCD_INTERFACE	:= flossjtag
OOCD_TARGET	:= stm32f4x
OOCDFLAGS := -f openocd.cfg

#设定包含文件目录
INC_DIR += -I $(TOP_DIR)/include           

CCFLAGS += $(INC_DIR)
CCFLAGS += $(DEFS)



ASFLAGS += $(INC_DIR)
ASFLAGS += $(DEFS)


LDFLAGS += -T $(LDSCRIPT)

DEPS = $(OBJS:%.o=%.d)

SECONDARY_FLASH = $(TOP_DIR)/debug/$(TARGET).hex
SECONDARY_SIZE = $(TOP_DIR)/debug/$(TARGET).size
SECONDARY_BIN = $(TOP_DIR)/debug/$(TARGET).bin
SECONDARY_ELF = $(TOP_DIR)/debug/$(TARGET).elf
SECONDARY_LIST = $(TOP_DIR)/debug/$(TARGET).list


.PHONY: images clean size bin hex size list debug stlink-flash style-code flash debug_file


all: $(TARGET).images debug_file
	@echo   building done

size: $(SECONDARY_ELF)
	@echo   SIZE    $(SECONDARY_ELF)
	$(Q)$(SIZE) --format=berkeley "$(SECONDARY_ELF)"

bin: $(SECONDARY_BIN)

hex: $(SECONDARY_FLASH)


$(TARGET).images: $(SECONDARY_BIN) $(SECONDARY_FLASH) $(SECONDARY_LIST)
	$(Q)echo   images generated


$(SECONDARY_ELF): $(OBJS) $(LDSCRIPT) libs
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


$(C_OBJS):$(C_SRCS).c 
	@echo   CC      $<
	$(Q)$(CC) $(CCFLAGS) -o $@ $<

$(ASM_OBJS):$(C_SRCS).S 
	@echo   AS      $<
	$(Q)$(CC) $(ASFLAGS) -o $@ $<

libs:
	$(MAKE) -C $(LIB_MAKE_DIR) $@

# 使用stlink驱动下载bin程序
stlink-flash: $(SECONDARY_BIN)
	@echo   ST-link FLASH  $<
	$(Q)$(STFLASH) write $(SECONDARY_BIN) 0x8000000

# 使用OpenOCD下载hex程序
flash: $(SECONDARY_FLASH)
	@echo   OPEN_OCD FLASH $<
	$(Q)$(OOCD) $(OOCDFLAGS) -c "program $(SECONDARY_FLASH) verify reset exit" 

debug_file:
	mkdir $(TOP_DIR)/debug
	mkdir $(TOP_DIR)/debug/objs

# 使用GDB 通过sdtin/out管道与OpenOCD连接 并在main函数处打断点后运行
debug: $(SECONDARY_ELF)
	@echo   GDB DEBUG $<
	$(Q)$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
	-iex 'monitor reset halt' -ex 'load' -ex 'break rc_test_loop' -ex 'c' $(SECONDARY_ELF)

-include $(DEPS)

clean:
	@echo   CLEAN
	$(Q)-$(RM) $(TOP_DIR)/debug
	
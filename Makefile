##############################################
# Copyright (C) 2018-2019 胡启航<Hu Qihang>
#
# Author: wkcs
# 
# Email: hqh2030@gmail.com, huqihan@live.com
##############################################

#打开显示选项
ifneq ($(V),1)
Q := @
endif

ifneq ($(MV), 1)
N := --no-print-directory
endif

ifndef $(ARCH)
    ARCH := arm
endif

ifndef $(BOARD)
    BOARD := zj_v3
endif

dir-y = app
dir-y += arch
dir-y += drivers
dir-y += fs
dir-y += init
dir-y += kernel
dir-y += lib
dir-y += mm
dir-y += net
dir-y += test

dir-run := $(dir-y:%=%-run)
project-dir = $(shell pwd)
out-dir = debug
obj-dir-t = $(dir-y:%=$(project-dir)/%)
obj-dir := $(dir-y:%=$(project-dir)/$(out-dir)/%)

#链接脚本
LDSCRIPT :=

include $(project-dir)/scripts/Makefile.config
include $(project-dir)/arch/$(ARCH)/board/$(BOARD)/Makefile.config

OOCD		:= openocd
OOCD_INTERFACE	:= flossjtag

LDFLAGS += $(LDSCRIPT:%=-T%)

.PHONY: all flash debug clean

all:$(TARGET_LIST) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_ELF) size

size:$(TARGET_ELF)
	@echo "SIZE     $<"
	$(Q)$(SIZE) --format=berkeley $<
	@echo build done

$(TARGET_HEX): $(TARGET_ELF)
	@echo "OBJCOPY  $@"
	$(Q)$(OBJCOPY) $<  $@ -Oihex

$(TARGET_BIN): $(TARGET_ELF)
	@echo "OBJCOPY  $@"
	$(Q)$(OBJCOPY) $<  $@ -Obinary

$(TARGET_LIST): $(TARGET_ELF)
	@echo "OBJDUMP  $@"
	$(Q)$(OBJDUMP) -S $< > $@

$(TARGET_ELF):$(dir-run) $(LDSCRIPT)
	@echo "LD       $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(shell find $(project-dir)/$(out-dir) -name "*.o")

$(dir-run):%-run:% $(project-dir)/$(out-dir) $(obj-dir)
	$(Q)make $(N) -f $(project-dir)/scripts/Makefile.build dir=$(project-dir)/$< \
	out-dir=$(out-dir) Q=$(Q) N=$(N) ARCH=$(ARCH) BOARD=$(BOARD) project-dir=$(project-dir) obj-done

$(project-dir)/$(out-dir):
	$(Q)-mkdir $@

$(obj-dir):
	$(Q)-mkdir $@

# 使用OpenOCD下载hex程序
flash: $(TARGET_HEX)
	@echo "OPEN_OCD FLASH $<"
	$(Q)$(OOCD) $(OOCDFLAGS) -c "program $< verify reset exit" 

# 使用GDB 通过sdtin/out管道与OpenOCD连接 并在main函数处打断点后运行
debug: $(TARGET_ELF)
	@echo "GDB DEBUG $<"
	$(Q)$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
	-iex 'monitor reset halt' -ex 'load' -ex 'break wkos_start' -ex 'c' $<

clean:
	$(Q)-$(RM) $(project-dir)/$(out-dir)
	@echo "clean done"
	
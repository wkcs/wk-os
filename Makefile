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

ifndef $(CONFIG)
    CONFIG := zj_v3_config
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
#dir-y += test

dir-run := $(dir-y:%=%-run)
ifndef $(out-dir)
    out-dir := out
endif
obj-dir := $(dir-y:%=$(out-dir)/%)

obj-all := $(out-dir)/app/app.a
obj-all += $(out-dir)/arch/arch.a
obj-all += $(out-dir)/drivers/drivers.a
#obj-all += $(out-dir)/fs/fs.a
obj-all += $(out-dir)/init/init.a
obj-all += $(out-dir)/kernel/kernel.a
obj-all += $(out-dir)/lib/lib.a
obj-all += $(out-dir)/mm/mm.a
#obj-all += $(out-dir)/net/net.a
#obj-all += $(out-dir)/test/test.a

#链接脚本
LDSCRIPT :=

include scripts/Makefile.config
include arch/$(ARCH)/board/$(BOARD)/Makefile.config
-include .config

OOCD		:= openocd
OOCD_INTERFACE	:= flossjtag

LDFLAGS += $(LDSCRIPT:%=-T%)

.PHONY: all flash debug clean config size $(obj-all)

all:$(TARGET_LIST) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_ELF) size

size: $(TARGET_ELF)
	@echo "SIZE     $(<:$(out-dir)/%=%)"
	$(Q)$(SIZE) --format=berkeley $<
	@echo build done

$(TARGET_HEX): $(TARGET_ELF)
	@echo "OBJCOPY  $(@:$(out-dir)/%=%)"
	$(Q)$(OBJCOPY) $<  $@ -Oihex

$(TARGET_BIN): $(TARGET_ELF)
	@echo "OBJCOPY  $(@:$(out-dir)/%=%)"
	$(Q)$(OBJCOPY) $<  $@ -Obinary

$(TARGET_LIST): $(TARGET_ELF)
	@echo "OBJDUMP  $(@:$(out-dir)/%=%)"
	$(Q)$(OBJDUMP) -S $< > $@

$(TARGET_ELF):$(obj-all) $(LDSCRIPT)
	@echo "LD       $(@:$(out-dir)/%=%)"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(obj-all)

$(obj-all): $(obj-dir) .config
	$(Q)$(MAKE) $(N) -f scripts/Makefile.build dir=$(@:$(out-dir)/%/$(notdir $@)=%) \
		out-dir=$(out-dir) Q=$(Q) N=$(N) ARCH=$(ARCH) BOARD=$(BOARD) \
		TARGET=$(notdir $@) built-in.o

$(obj-dir):$(out-dir)
	$(Q)if [ ! -d $@ ]; then \
		mkdir $@; \
	fi

$(out-dir):
	$(Q)if [ ! -d $@ ]; then \
		mkdir $@; \
	fi

# 使用OpenOCD下载hex程序
flash: $(TARGET_HEX)
	@echo "OPEN_OCD FLASH $<"
	$(Q)$(OOCD) $(OOCDFLAGS) -c "program $< verify reset exit" 

# 使用GDB 通过sdtin/out管道与OpenOCD连接 并在main函数处打断点后运行
debug: $(TARGET_ELF)
	@echo "GDB DEBUG $<"
	$(Q)$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
	-iex 'monitor reset halt' -ex 'load' -ex 'break wkos_start' -ex 'c' $<

config:
	@echo "write to .config"
	$(Q)cat arch/$(ARCH)/config/$(CONFIG) > .config

clean:
	$(Q)-$(RM) $(out-dir)
	$(Q)-$(RM) .config
	@echo "clean done"

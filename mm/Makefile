FILE_DIR = $(TOP_DIR)/mm

TEMP_C_SCRS = mm_core.c
TEMP_C_SCRS += mm_init.c
TEMP_C_SCRS += mm.c

TEMP_ASM_SCRS =

OBJS += $(TEMP_C_SCRS:%.c:$(TOP_DIR)/debug/objs/%.o)
OBJS += $(TEMP_ASM_SCRS:%.s:$(TOP_DIR)/debug/objs/%.o)

C_SCRS += $(TEMP_C_SCRS:%:$(FILE_DIR)/%)
ASM_SCRS += $(TEMP_ASM_SCRS:%:$(FILE_DIR)/%)
# Definition#################################################################
# DEBUG
DEBUG_DIR 	:= Debug
$(shell mkdir -p $(DEBUG_DIR))
DEBUG 		= true
DEBUG_TOOL	= cmsis-dap

# TARGET_MCU
TARGET_MCU 	= stm32g0x
TARGET_CORE = cortex-m0
# Project name
PROJECT     :=stm32g070-ssd1306-lvgl

# Define compile tool
CC_PREFIX   := arm-none-eabi-
CXX         := $(CC_PREFIX)g++
CC          := $(CC_PREFIX)gcc
CP          := $(CC_PREFIX)objcopy
GDB         := $(CC_PREFIX)gdb
SIZE        := $(CC_PREFIX)size
AS          := $(CXX) -x assembler-with-cpp
HEX         := $(CP) -O ihex
BIN         := $(CP) -O binary -S

# Configure common parameters
SPECS 		+= -mthumb #define cortex-m instruction set
SPECS 		+= -mcpu=$(TARGET_CORE) #define MCU architecture
#SPECS 		+= -mfloat-abi=hard -mfpu=fpv4-sp-d16 #enable hardware floating-point support
SPECS       += -O0 #define optimization level
SPECS       += -fsingle-precision-constant  # this means that by default, all constants are single-float, not double type
SPECS       += -fno-common  # Inhibiting variables multiple definition
SPECS       += -ffunction-sections -fdata-sections  # Each function is divided into individual section
SPECS_C     += $(SPECS) -std=c99
SPECS_CXX   += $(SPECS) -std=c++11

# Define link script
LINK_SCRIPT := $(shell find . -name "*.ld")
ELF_FILES 	:= $(shell find . -name "*.elf")
MAP_FILES 	:= $(shell find . -name "*.map")
HEX_FILES 	:= $(shell find . -name "*.hex")
BIN_FILES 	:= $(shell find . -name "*.bin")

# Configure parameters for linking and debugging#################################
ifeq ($(DEBUG), true)
FLAGS	 	+= -ggdb3 -gdwarf-2 #enable debugging function , default debugging form  is dwarf-5 in .o files, -ggdb3 support debug of macro
FLAGS	 	+=  -fverbose-asm #-save-temps # generate debugging files 
FLAGS	   	+= -fomit-frame-pointer # 1.Save memory because it omit  the frame pointer of some functions ,it's decided by compiler .But it is not good for debugging for some machines.
									# 2. If not use this parameter , r7 is not available as general register.Because the register will store the stack frame . 
									# 3. So , it's necessary for cortex-m4 , if it is not added , program will jump into Infinite_Loop when debugging . Because r7 is used in context switching.
LIBS        += -specs=rdimon.specs -u _printf_float -lm -lc -lrdimon # Use Semihosting Debug When Compiling with GCC
																# and enable floating point support in printf. 
																# It is not enabled by default
else
LIBS 		+= -specs=nano.specs -lm # nano.specs is smaller than rdimon.specs . 
									# lm is math lib,
endif
FLAGS	 	+= -c -MD -MP   # generate .d files, keep track of dependencies between .h and .c files
FLAGS	   	+= -Wall  # enable all warnings
FLAGS_CXX   += $(FLAGS) $(SPECS_CXX)  
FLAGS_C   	+= $(FLAGS) $(SPECS_C)  

FLAGS_AS 	+= $(SPECS_C) $(FLAGS)  #enable debugging function , default debugging form  is dwarf-5 in .o files
FLAGS_LD 	+= $(SPECS_C) $(LIBS)   # link standard library
FLAGS_LD	+= -Xlinker --gc-section  # delete untapped code or data to save memory 
FLAGS_LD 	+= -T$(LINK_SCRIPT) -lc -lnosys # link .ld file
FLAGS_LD 	+= -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch # generate .map file , 
																# --cref , generate debugging message about cross-reference
																# --no-warn-mismatch,  disable the warning when  some symbols mismatch

# Define SRC, OBJ, DPT, INC files paths
# for c
C_INC_FILES           := $(shell find ./ -name "*.h" | xargs dirname)
C_INC_FILES           := $(addprefix -I, $(C_INC_FILES))
C_SRC_FILES           := $(shell find ./ -name "*.c")
C_PRE_COMPILE_FILES   := $(patsubst %.c,%.i,$(C_SRC_FILES))  # C pre-compile
# for cpp
CXX_SRC_FILES         := $(shell find ./ -name "*.cpp")  # Change from *.c to *.cpp for C++ source files
CXX_PRE_COMPILE_FILES := $(patsubst %.cpp,%.ii,$(CXX_SRC_FILES))  # Update for C++ pre-compile
# general setting
C_ASM_FILES1          := $(shell find ./ -name "*.s")
C_ASM_FILES2          := $(shell find ./ -name "*.S")
C_ASM_FILES_TEMP      += $(patsubst %.cpp,%.s,$(CXX_SRC_FILES))  # Update to handle C++ files
C_ASM_FILES_TEMP      += $(patsubst %.c,%.s,$(C_SRC_FILES))  # Update to handle C files
C_OBJ_FILES           := $(patsubst %.cpp,$(DEBUG_DIR)/%.o,$(CXX_SRC_FILES))  # Change to handle C++ objects
C_OBJ_FILES           += $(patsubst %.c,$(DEBUG_DIR)/%.o,$(C_SRC_FILES))
C_OBJ_FILES           += $(patsubst %.s,$(DEBUG_DIR)/%.o,$(C_ASM_FILES1))
C_OBJ_FILES           += $(patsubst %.S,$(DEBUG_DIR)/%.o,$(C_ASM_FILES2))
C_DPT_FILES           += $(patsubst %.cpp,$(DEBUG_DIR)/%.d,$(CXX_SRC_FILES))  # Change to handle C++ dependencies
C_DPT_FILES           += $(patsubst %.c,$(DEBUG_DIR)/%.d,$(C_SRC_FILES))  # Change to handle C dependencies

# Define debug parameters
TYPE_LOAD             := openocd_load
TYPE_BURN             := openocd_swd_flash
TYPE_DEBUG            := openocd_swd_debug
TYPE_ERASE            := openocd_swd_erase

################################################################################
.PHONY:all burn debug erase clean test

all:$(C_OBJ_FILES) $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin 
	$(SIZE) $(PROJECT).elf

$(DEBUG_DIR)/%.o:%.c
	@mkdir -p $(dir $@)
	$(CC) 	$(C_INC_FILES) $(FLAGS_C) 	$< 	-o $@
$(DEBUG_DIR)/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(CXX) 	$(C_INC_FILES) $(FLAGS_CXX) $< 	-o $@
$(DEBUG_DIR)/%.o:%.S
	@mkdir -p $(dir $@)
	$(AS) 	$(C_INC_FILES) $(FLAGS_AS) 	$< 	-o $@
$(DEBUG_DIR)/%.o:%.s
	@mkdir -p $(dir $@)
	$(AS) 	$(C_INC_FILES) $(FLAGS_AS) 	$< 	-o $@
%.elf:$(C_OBJ_FILES)
	$(CC) 	$(C_OBJ_FILES) $(FLAGS_LD) 		-o $@
%.hex:%.elf
	$(HEX) 	$< $@
%.bin:%.elf
	$(BIN) 	$< $@

################################################################################

load: $(TYPE_LOAD)
burn: $(TYPE_BURN)
debug: $(TYPE_DEBUG)
erase: $(TYPE_ERASE)

openocd_load:
	openocd -f /usr/share/openocd/scripts/interface/$(DEBUG_TOOL).cfg \
	-f /usr/share/openocd/scripts/target/$(TARGET_MCU).cfg \

openocd_swd_flash: $(PROJECT).bin
	$(GDB) \
	-ex 'target remote localhost:3333' \
	-ex 'monitor reset init' -ex 'monitor reset halt' \
	-ex 'monitor sleep 100' -ex 'monitor wait_halt 2' \
	-ex 'monitor flash write_image erase $(PROJECT).bin 0x08000000' \
	-ex 'monitor sleep 100' -ex 'monitor verify_image $(PROJECT).bin 0x08000000' \

openocd_swd_debug: $(PROJECT).bin  
	$(GDB) \
	-ex 'target remote localhost:3333' \
	-ex 'monitor reset init' -ex 'monitor reset halt' \
	-ex 'monitor arm semihosting enable' \
	-ex 'load' \
	-ex 'b main:35' \
	$(PROJECT).elf \

openocd_swd_erase:
	openocd -f /usr/share/openocd/scripts/interface/$(DEBUG_TOOL).cfg \
	-f /usr/share/openocd/scripts/target/$(TARGET_MCU).cfg \
	-c "init" -c "reset halt" -c "sleep 100" -c "stm32g4x mass_erase 0" \
	-c "sleep 100" -c "shutdown"

test:
#@echo $(C_SRC_FILES)
#@echo $(CXX_SRC_FILES)
#@echo $(C_INC_FILES)
	@echo $(C_OBJ_FILES)
#@echo $(C_DPT_FILES)
#@echo $(C_PRE_COMPILE_FILES)
#@echo $(CXX_PRE_COMPILE_FILES)
#@echo $(C_ASM_FILES_TEMP)
#@echo $(C_ASM_FILES)

clean:
	-find . -name "*.d" -delete
	-find . -name "*.o" -delete
	-find . -name "*.ii" -delete
	-find . -name "*.elf" -delete
	-find . -name "*.map" -delete
	-find . -name "*.hex" -delete
	-find . -name "*.bin" -delete
	-rm -r $(DEBUG_DIR)
#-rm $(C_ASM_FILES_TEMP)

# End ############################################################################3

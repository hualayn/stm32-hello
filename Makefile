###############################################################################
# STM32F103C8T6 "Blue Pill" - hello world (PC13 LED blink)
#
# Toolchain : arm-none-eabi-gcc 15.x   (on PATH)
# Build     : mingw32-make            -> build/stm32_hello.{elf,bin,hex}
# Flash     : mingw32-make flash      -> OpenOCD + ST-Link V2
#
# The HAL/CMSIS driver sources are NOT copied into this project; they are
# referenced from the sibling STM32CubeF1 checkout (G:\my_git\STM32CubeF1).
# Override with:  mingw32-make CUBE_DIR=/path/to/STM32CubeF1
#
# NOTE: All .o files are kept flat in build/ (vpath resolves the sources)
# so that every recipe line is mingw-make friendly (mingw make executes
# recipe lines containing '/' or '\' directly, bypassing the shell).
###############################################################################

TARGET   = stm32_hello
CUBE_DIR ?= ../STM32CubeF1

HAL_SRC  = $(CUBE_DIR)/Drivers/STM32F1xx_HAL_Driver/Src

PREFIX   = arm-none-eabi-
CC       = $(PREFIX)gcc
OBJCOPY  = $(PREFIX)objcopy
OBJDUMP  = $(PREFIX)objdump
SIZE     = $(PREFIX)size

CPU      = -mcpu=cortex-m3
MCU      = $(CPU) -mthumb -mabi=aapcs

DEFS     = -DUSE_HAL_DRIVER -DSTM32F103xB

INCLUDES = -ICore/Inc \
           -I$(CUBE_DIR)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
           -I$(CUBE_DIR)/Drivers/CMSIS/Include \
           -I$(CUBE_DIR)/Drivers/STM32F1xx_HAL_Driver/Inc \
           -I$(CUBE_DIR)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy

C_SOURCES = \
  Core/Src/main.c \
  Core/Src/stm32f1xx_it.c \
  Core/Src/system_stm32f1xx.c \
  $(HAL_SRC)/stm32f1xx_hal.c \
  $(HAL_SRC)/stm32f1xx_hal_cortex.c \
  $(HAL_SRC)/stm32f1xx_hal_gpio.c \
  $(HAL_SRC)/stm32f1xx_hal_gpio_ex.c \
  $(HAL_SRC)/stm32f1xx_hal_rcc.c \
  $(HAL_SRC)/stm32f1xx_hal_rcc_ex.c

ASM_SOURCES = Core/Src/startup_stm32f103xb.s

# Sources found via vpath below
vpath %.c Core/Src $(HAL_SRC)
vpath %.s Core/Src

LDSCRIPT  = STM32F103C8Tx_FLASH.ld
BUILD_DIR = build
OUTPUT    = $(BUILD_DIR)/$(TARGET)

# Flat object list (all basenames are unique)
OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)))
DEP = $(OBJ:.o=.d)

CFLAGS   = $(MCU) $(DEFS) $(INCLUDES) -Wall -Wextra -fdata-sections -ffunction-sections -Os -std=c11
AFLAGS   = $(MCU) -Wall
LDFLAGS  = $(MCU) -T$(LDSCRIPT) --specs=nano.specs \
           -Wl,--gc-sections -Wl,-Map=$(OUTPUT).map -Wl,--print-memory-usage \
           -Wl,--no-warn-rwx-segments

.PHONY: all clean flash program

all: $(OUTPUT).elf $(OUTPUT).bin $(OUTPUT).hex

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(CC) $(AFLAGS) -x assembler-with-cpp -c $< -o $@

$(OUTPUT).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(OUTPUT).bin: $(OUTPUT).elf
	$(OBJCOPY) -O binary $< $@

$(OUTPUT).hex: $(OUTPUT).elf
	$(OBJCOPY) -O ihex $< $@

# Flash via OpenOCD + ST-Link V2 (blue pill: stm32f1x target, 64K @ 0x08000000)
flash: $(OUTPUT).bin
	openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
		-c "program $(OUTPUT).bin 0x08000000 verify reset exit"

program: flash

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

-include $(DEP)

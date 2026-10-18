cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------------------
# STM32F103C8T6 (Blue Pill) hello world: 72 MHz HSE clock + PC13 LED blink
#
# Cross-compiles with any GCC-based arm-none-eabi toolchain on Windows
# and Linux.
#
#   configure: cmake -B build -S . -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
#   build:     cmake --build build
#   flash:     openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
#                -c "program build/stm32_hello.bin 0x08000000 verify reset exit"
#
# Self-contained: the CMSIS + HAL driver trees are vendored in this project
# under Drivers/. CUBE_DIR points at that directory and can be overridden to
# use an external checkout:
#   -DCUBE_DIR=/path/to/STM32CubeF1/Drivers
# ---------------------------------------------------------------------------

if(NOT DEFINED CUBE_DIR)
  set(CUBE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Drivers")
endif()

set(TARGET_NAME stm32_hello)
project(${TARGET_NAME} C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- Sources ---------------------------------------------------------------
set(HAL_SRC "${CUBE_DIR}/STM32F1xx_HAL_Driver/Src")

add_executable(${TARGET_NAME}
  Core/Src/main.c
  Core/Src/oled_i2c.c
  Core/Src/font6x8.c
  Core/Src/face.c
  Core/Src/stm32f1xx_it.c
  Core/Src/system_stm32f1xx.c
  Core/Src/startup_stm32f103xb.s
  ${HAL_SRC}/stm32f1xx_hal.c
  ${HAL_SRC}/stm32f1xx_hal_cortex.c
  ${HAL_SRC}/stm32f1xx_hal_gpio.c
  ${HAL_SRC}/stm32f1xx_hal_gpio_ex.c
  ${HAL_SRC}/stm32f1xx_hal_rcc.c
  ${HAL_SRC}/stm32f1xx_hal_rcc_ex.c
)

target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
  ${CUBE_DIR}/CMSIS/Device/ST/STM32F1xx/Include
  ${CUBE_DIR}/CMSIS/Include
  ${CUBE_DIR}/STM32F1xx_HAL_Driver/Inc
  ${CUBE_DIR}/STM32F1xx_HAL_Driver/Inc/Legacy
)

target_compile_definitions(${TARGET_NAME} PRIVATE
  USE_HAL_DRIVER
  STM32F103xB
)

# ---- Cortex-M3 build flags -------------------------------------------------
set(CPU_FLAGS -mcpu=cortex-m3 -mthumb)

target_compile_options(${TARGET_NAME} PRIVATE
  ${CPU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections -Os
)

# --no-warn-rwx-segments only exists in binutils >= 2.41; probe so that
# older Linux toolchains (e.g. Debian 12, binutils 2.40) still link.
execute_process(COMMAND ${CMAKE_SIZE} --version
                OUTPUT_VARIABLE _size_ver
                OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCH "[0-9]+\\.[0-9]+" BINUTILS_VERSION "${_size_ver}")
if(NOT BINUTILS_VERSION VERSION_LESS 2.41)
  list(APPEND CPU_FLAGS -Wl,--no-warn-rwx-segments)
endif()

set(LDSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld")

target_link_options(${TARGET_NAME} PRIVATE
  ${CPU_FLAGS}
  -T${LDSCRIPT}
  --specs=nano.specs
  -Wl,--gc-sections
  -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.map
  -Wl,--print-memory-usage
)

# .elf output name
set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".elf")

# .bin / .hex / size report after link
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}>
          ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_NAME}>
          ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.hex
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
  COMMENT "Generating ${TARGET_NAME}.bin / .hex"
)

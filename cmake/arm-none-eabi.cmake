# CMake toolchain file for cross-compiling to STM32F103C8T6 (Cortex-M3)
# with a GCC-based arm-none-eabi toolchain. Works on Windows and Linux.
#
# The toolchain is located by its prefix on PATH. For non-PATH installs:
#   cmake -B build -DCMAKE_C_COMPILER=/path/to/arm-none-eabi-gcc \
#         -DCMAKE_ASM_COMPILER=/path/to/arm-none-eabi-gcc ...
# (setting the compilers explicitly bypasses the prefix logic below.)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Bare-metal target: CMake's compiler/ABI self-tests cannot link a runnable
# test program (no newlib startup symbols like _exit/_sbrk), so run them as
# compile-only (static library) tests.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLCHAIN_PREFIX "arm-none-eabi-"
    CACHE STRING "Prefix of the ARM GCC toolchain")

if(NOT CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
endif()
if(NOT CMAKE_ASM_COMPILER)
  set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc")
endif()

set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy" CACHE FILEPATH "")
set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}objdump" CACHE FILEPATH "")
set(CMAKE_SIZE    "${TOOLCHAIN_PREFIX}size"    CACHE FILEPATH "")

# Host programs (objcopy, size, ...) are found on PATH;
# libraries/includes for the target come from the toolchain itself.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

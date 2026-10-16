# STM32F103C8T6 Hello World（Blue Pill 点灯）

CMake 交叉编译的 STM32F103C8T6 "Blue Pill" 工程（不依赖 CubeMX 图形界面，
**自包含**：所需 CMSIS + HAL 驱动已内置，克隆本仓库即可构建）。
功能：系统时钟 72 MHz + 板载 LED（PC13）500 ms 闪烁。Windows / Linux 均可构建。

## 工程结构

```
stm32-hello/
├── CMakeLists.txt                  # 构建脚本
├── cmake/
│   └── arm-none-eabi.cmake         # 交叉编译 toolchain（GCC 系，Win/Linux 通用）
├── STM32F103C8Tx_FLASH.ld          # 链接脚本（64K Flash / 20K RAM）
├── Drivers/                        # 内置驱动（取自 STM32CubeF1，无需另克隆）
│   ├── CMSIS/Include/              #   CMSIS 核心头文件
│   ├── CMSIS/Device/ST/STM32F1xx/  #   设备头文件 + gcc 启动/链接模板
│   └── STM32F1xx_HAL_Driver/       #   STM32F1 HAL（Inc + Src）
├── Core/
│   ├── Inc/                        # main.h / hal_conf / it.h
│   └── Src/                        # main.c / it.c / startup / system
├── .vscode/                        # VS Code：IntelliSense 配置 + 构建/烧录/调试任务
└── build/                          # 构建产物（生成物，勿提交）
```

如需改用外部的 STM32CubeF1 仓库（不推荐，仅为了省磁盘）：
`-DCUBE_DIR=/path/to/STM32CubeF1/Drivers`

## 依赖

| 工具 | Windows | Linux |
| --- | --- | --- |
| CMake ≥ 3.15 | ✅ | `sudo apt install cmake` |
| Ninja | ✅ | `sudo apt install ninja-build` |
| arm-none-eabi-gcc | Arm GNU Toolchain（PATH） | `sudo apt install gcc-arm-none-eabi`（或 STM32CubeCLT） |
| openocd（仅烧录/调试） | ✅（PATH） | `sudo apt install openocd` |

## 构建（Windows / Linux 相同命令）

```bash
cmake -B build -S . -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
cmake --build build
```

产物：`build/stm32_hello.elf / .bin / .hex / .map`
（参考值：FLASH 2872 B / 4.38%，RAM 1584 B / 7.73%）

- toolchain 不在 PATH 时，追加：
  `-DCMAKE_C_COMPILER=/path/to/arm-none-eabi-gcc -DCMAKE_ASM_COMPILER=/path/to/arm-none-eabi-gcc -DCMAKE_OBJCOPY=/path/to/arm-none-eabi-objcopy`
- CMake 找不到 ninja 时（Windows 个别环境），追加：`-DCMAKE_MAKE_PROGRAM=/path/to/ninja(.exe)`
- 清理：`cmake --build build --target clean`（或删掉 build 目录重来）

## 烧录

### ST-Link（推荐，本工程验证通过）

```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
    -c "program build/stm32_hello.bin 0x08000000 verify reset exit"
```

VS Code：`Ctrl+Shift+P` → Run Task → `flash`（自动先构建）；
或直接 F5 调试（自动构建 → 开 OpenOCD → arm-none-eabi-gdb 接管）。
DAP-Link 用户把 `interface/stlink.cfg` 换成 `interface/stlink-dap.cfg`。

### USB 串口 Bootloader（无 ST-Link 时）

1. BOOT0 = 1、BOOT1 = 0，复位（进入系统存储器 Bootloader）。
2. USB-TTL 线（115200）+ `st-flash` 或 STM32CubeProgrammer 下载
   `build/stm32_hello.bin` 到 `0x08000000`。
3. BOOT0 拨回 0，复位运行。

## 时钟配置

```
8 MHz 外部晶振 (HSE) → PLL ×9 → 72 MHz SYSCLK
  HCLK  = 72 MHz (APB2)
  PCLK1 = 36 MHz (APB1)
  PCLK2 = 72 MHz
  Flash 等待周期 = 2 WS
```

## 修改 LED 引脚 / 闪烁周期

- 引脚：`Core/Inc/main.h` 的 `LED_Pin` / `LED_GPIO_Port`
  （Blue Pill 板载 LED 接 PC13、低电平点亮；你的板子若接在其他引脚，
  改这两行，必要时同步改 `main.c` 的 `MX_GPIO_Init`）。
- 周期：`main.h` 的 `BLINK_PERIOD_MS`（默认 500 ms）。

## 备注

- `Drivers/` 里的文件来自 `G:\my_git\STM32CubeF1`（ST 官方仓库），
  只复制了构建所需的部分（CMSIS 核心 + F1 设备头文件 + F1 HAL + gcc 模板）。
  升级 HAL 时从新仓库同步 `Drivers/STM32F1xx_HAL_Driver` 与
  `Drivers/CMSIS/Device/ST/STM32F1xx` 即可。
- 链接脚本派生自 ST 模板 `STM32F103XB_FLASH.ld`，模板 FLASH 长度误写 128K，
  本文件已改为 C8T6 实际的 64K。
- 本工程使用旧版 F1 HAL（`RCC_OscInitTypeDef` 在 `stm32f1xx_hal_rcc_ex.h`，
  字段 `SYSCLKSource` / `HSEPredivValue`），`main.c` 已按该版本编写；
  升级 HAL 到 v1.8+ 时改回 `SYSCLK` / `HSEValue` 写法。
- toolchain 里的 `CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY` 是裸机工程必需：
  CMake 自检程序无法链接运行（缺 `_exit`/`_sbrk`），改为只编译不链接。
- 若 HSE 晶振不起振（个别板子虚焊），可把 `main.c` 时钟源改为 HSI
  （8 MHz 内部振荡器，不经过 PLL 即 8 MHz）。

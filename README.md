# STM32F103C8T6 Hello World（Blue Pill 点灯）

手工初始化的 STM32F103C8T6 "Blue Pill" 工程（不依赖 CubeMX 图形界面），
功能：系统时钟 72 MHz + 板载 LED（PC13）以 500 ms 周期闪烁。

## 工程结构

```
stm32-hello/
├── Core/
│   ├── Inc/
│   │   ├── main.h                  # LED 引脚定义（PC13，低电平点亮）
│   │   ├── stm32f1xx_hal_conf.h    # HAL 模块配置（RCC/GPIO/CORTEX）
│   │   └── stm32f1xx_it.h          # 中断处理函数声明
│   └── Src/
│       ├── main.c                  # 主程序：时钟配置 + LED 闪烁
│       ├── stm32f1xx_it.c          # 中断服务函数（SysTick 等）
│       ├── startup_stm32f103xb.s   # 启动文件（取自 STM32CubeF1 仓库）
│       └── system_stm32f1xx.c      # SystemInit（取自 STM32CubeF1 仓库）
├── STM32F103C8Tx_FLASH.ld          # 链接脚本（64K Flash / 20K RAM）
├── Makefile                        # mingw32-make 构建脚本
└── build/                          # 编译产物（生成物，勿提交）
```

HAL / CMSIS 驱动源码**不复制**进本工程，而是引用同级目录的
`G:\my_git\STM32CubeF1` 仓库（可用 `CUBE_DIR=...` 覆盖）：

```
mingw32-make CUBE_DIR=D:/other/path/to/STM32CubeF1
```

## 依赖（本机已就绪）

| 工具 | 版本 | 位置 |
| --- | --- | --- |
| arm-none-eabi-gcc | 15.3 (Arm GNU Toolchain 15.3.Rel2) | `E:\arm-gnu-toolchain-15.3...`（已在 PATH） |
| make | 4.4.1（mingw32-make） | `E:\msys64\ucrt64\bin`（已在 PATH） |
| openocd | 0.12.0 (xPack) | `E:\xpack-openocd-0.12.0-7`（已在 PATH） |
| STM32CubeF1 仓库 | — | `G:\my_git\STM32CubeF1` |

## 构建

```powershell
mingw32-make          # 生成 build\stm32_hello.{elf,bin,hex,map}
mingw32-make clean    # 清理
```

构建结果（参考值）：

```
FLASH: 2876 B / 64 KB (4.39%)
RAM:   1584 B / 20 KB (7.73%)
```

## 烧录

### 方式一：ST-Link V2（推荐，本工程自带）

```powershell
mingw32-make flash
```

等价于：

```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg ^
    -c "program build/stm32_hello.bin 0x08000000 verify reset exit"
```

如果 ST-Link 是 USB 转 SWD 转接线或板载调试口，可能需要先确认
`interface` 配置（常见为 `interface/stlink-v2.cfg`；
DAP-Link 用 `interface/stlink-dap.cfg` 或 `interface/daplink.cfg`）。

### 方式二：USB 串口 Bootloader（无 ST-Link 时）

1. BOOT0 拨到 1，BOOT1 保持 0，复位板子（进入系统存储器 Bootloader）。
2. 用 USB-TTL 线连接（115200 波特率），用 `st-flash` 或
   STM32CubeProgrammer 下载 `build\stm32_hello.bin` 到 `0x08000000`。
3. BOOT0 拨回 0，复位，程序开始运行。

## 时钟配置

```
8 MHz 外部晶振 (HSE) → PLL ×9 → 72 MHz SYSCLK
  HCLK  = 72 MHz (APB2)
  PCLK1 = 36 MHz (APB1)
  PCLK2 = 72 MHz
  Flash 等待周期 = 2 WS
```

## 修改 LED 引脚 / 闪烁周期

- 引脚：`Core/Inc/main.h` 中的 `LED_Pin` / `LED_GPIO_Port`
  （Blue Pill 板载 LED 接 PC13、低电平点亮；若你的板子 LED 在其他引脚，
  改这两行即可，必要时同步修改 `main.c` 中 `MX_GPIO_Init` 的 GPIO 口）。
- 周期：`main.h` 中的 `BLINK_PERIOD_MS`（默认 500 ms）。

## 备注

- 链接脚本 `STM32F103C8Tx_FLASH.ld` 派生自 ST 模板
  `STM32F103XB_FLASH.ld`，但模板中 FLASH 长度误写为 128K，
  本文件已改为 C8T6 实际的 **64K**。
- 本工程使用旧版 F1 HAL（`RCC_OscInitTypeDef` 位于
  `stm32f1xx_hal_rcc_ex.h`，字段为 `SYSCLKSource` / `HSEPredivValue`），
  `main.c` 的时钟初始化代码已按该版本编写；如果将来升级 HAL 到
  v1.8+，需改回 `SYSCLK` / `HSEValue` 写法。
- 若 HSE 晶振不起振（部分板子晶振虚焊），可把
  `main.c` 的时钟源改为 HSI（8 MHz 内部振荡器，无需 PLL 即 8 MHz）。

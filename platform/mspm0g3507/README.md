# MSPM0G3507 移植层

TI MSPM0G3507（Cortex-M0+）的 ezUI 硬件适配层，使用 CCS Theia + SysConfig + DriverLib。

## 与共享核心的关系

```
core/                  平台无关：菜单框架 + 两个 HAL 接口
  Inc/display_hal.h    ← 本目录实现它
  Inc/system_hal.h     ← 本目录实现它
platform/mspm0g3507/   本平台实现
```

菜单逻辑（`core/Src/menu.c`）**一行未改**，只替换了 HAL 实现文件。

## 文件说明

| 文件 | 作用 |
|---|---|
| `Src/display_hal_g3507.c` | 实现 `display_hal.h`，桥接到 `oled.c` |
| `Src/system_hal_g3507.c`  | 实现 `system_hal.h`（延时、串口、按键） |
| `Src/oled.c` + `Inc/oled.h` | SSD1306/CH1116 驱动，**含 MSPM0 的 `DL_GPIO_*` 宏，故属平台层** |
| `Inc/oledfont.h` | 字库 |
| `Src/sys.c` + `Inc/sys.h` | 类型别名与位带操作 |
| `Src/main.c` | 应用入口 |
| `ui.syscfg` | SysConfig 配置（引脚、外设） |
| `.ccsproject` / `.cproject` / `.project` | CCS 工程文件 |
| `targetConfigs/MSPM0G3507.ccxml` | 调试目标配置 |

## 编译

用 CCS Theia 打开本目录，或导入为 CCS 工程。依赖：
- MSPM0 SDK 2.10.00.04
- TI Clang 编译器
- SysConfig

## 移植要点

从 STM32 版迁移到本平台时，只需：
1. 实现 `display_hal.h` 的 13 个函数
2. 实现 `system_hal.h` 的延时 / 串口 / 按键接口
3. `core/` 下的菜单代码不用动

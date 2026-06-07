<p align="center">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  <img src="https://img.shields.io/badge/platform-STM32-green.svg" alt="Platform">
  <img src="https://img.shields.io/badge/display-OLED-orange.svg" alt="Display">
  <img src="https://img.shields.io/badge/ROM-11.6KB-red.svg" alt="ROM">
  <img src="https://img.shields.io/badge/RAM-3.2KB-yellow.svg" alt="RAM">
</p>

<h1 align="center">🎮 ezUI</h1>
<h3 align="center">嵌入式单片机上，最好用的菜单框架</h3>

<p align="center">
  <b>零动态内存 · 零学习成本 · 一个 .c 文件搞定全部菜单</b>
</p>

---

## 🧬 核心特性

### 🔌 多 MCU 可移植 — HAL 抽象，换芯不换码

ezUI 把硬件操作完全隔离到 `display_hal.h` 和 `system_hal.h` 两个接口里，菜单逻辑**一行不改**就能迁移。

| 当前平台 | 说明 |
|---------|------|
| STM32F1 / F4 / G0 / H7 | 已实现 `display_hal_stm32.c` + `system_hal_stm32.c` |

> 💡 移植到其他 MCU 只需实现两个文件：`display_hal_xxx.c` + `system_hal_xxx.c`，详见下方 [🔌 移植到其他 MCU](#-移植到其他-mcu)。

### 🖥️ 多驱动芯片可移植 — 显示与逻辑解耦

显示驱动和菜单框架完全分离。换一款 OLED 驱动芯片，**只改驱动层，不动菜单代码**。

| 当前驱动 | 分辨率 | 接口 |
|---------|--------|------|
| CH1116 | 128×64 | I²C |

> 💡 所有显示操作统一走 `display_hal.h` 接口。换 SSD1306 / SH1106 / TFT 等屏，只需重写一个 `.c` 文件。

### ⚡ 非阻塞实时架构 — 主循环永不卡顿

| 特性 | 实现方式 |
|------|---------|
| **非阻塞 IO** | 串口没数据立刻返回，不忙等 |
| **非阻塞命令** | 画一帧就回来，不占主循环 |
| **脏标记刷新** | 画面没变就不刷屏，零 CPU 浪费 |
| **无 RTOS 依赖** | 纯裸机运行，不占额外栈空间 |
| **RTOS 兼容** | 也可跑在 FreeRTOS / RT-Thread 线程里 |

ezUI **不需要 RTOS**，但**不排斥 RTOS**——你可以把它扔进一个 task，主循环照样流畅。

### 📦 极致轻量 — 嵌入式就该这么省

```
ROM  11.6 KB  （含 HAL 驱动 + 字体 + 菜单引擎）
RAM   3.2 KB  （含帧缓冲 + 环形缓冲 + 菜单树）
堆分配      0  （完全静态分配，无碎片风险）
```

### 🧩 分层架构一览

```
         ┌─────────────────────────┐
         │    menuData.c           │  ← 你只改这个文件
         │    (菜单 & 命令)         │
         ├─────────────────────────┤
         │    menu.c               │  ← 菜单引擎（通用，不改）
         ├───────────┬─────────────┤
         │ display   │  system     │  ← HAL 接口（不改）
         │ _hal.h    │  _hal.h     │
         ├───────────┼─────────────┤
         │ display   │  system     │  ← 平台实现（换芯片只换这两个）
         │ _hal_xxx.c│  _hal_xxx.c │
         └───────────┴─────────────┘
```

---

## ✨ 介绍

ezUI 是为 STM32 + OLED 量身打造的菜单框架。  

你只需要在 **一个文件** 里定义菜单项和对应的函数，烧录，打开串口助手，**W/S/D/A** 就能玩了。

不需要 RTOS，不需要配复杂的结构体，不需要管动态内存。  
**就是一个 .h、几个 .c、改一个文件、编译、下载、收工。**

```c
MenuItem_t my_items[] = {
    MENU_CMD("闪烁 LED",  cmd_blink)
    MENU_CMD("读取电压",  cmd_adc)
    MENU_BACK
};
```

三行数组，一个菜单。

---

## 🚀 三分钟上手

### 第 1 步 — 在 `menuData.h` 声明你的命令函数

```c
void cmd_blink(void);
void cmd_adc(void);
void cmd_about(void);
```

### 第 2 步 — 在 `menuData.c` 写命令和菜单

```c
#include "menu.h"
#include "menuData.h"
#include "display_hal.h"

/* ── 命令函数：按 D 执行 ── */

void cmd_blink(void) {
    Display_NewFrame();
    Display_PrintString(0, 0,
        "LED 闪烁中！", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
    /* 在这里翻转你的 GPIO！ */
}

void cmd_adc(void) {
    Display_NewFrame();
    Display_PrintString(0, 0,
        "当前电压: 3.28V", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 24,
        "按 A 返回", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

/* ── 定义菜单 ── */

MenuItem_t my_items[] = {
    MENU_CMD("闪烁 LED", cmd_blink)
    MENU_CMD("读取电压", cmd_adc)
    MENU_BACK
};

Menu_t my_menu = {
    "我的应用",                  /* 屏幕顶部的标题 */
    my_items,                    /* 菜单项数组 */
    ITEM_COUNT(my_items),        /* 自动算数量 */
    NULL                         /* parent 不用管，Menu_Init 自动修 */
};
```

### 第 3 步 — 在 `main.c` 初始化

```c
int main(void) {
    HAL_Init();  SystemClock_Config();
    MX_GPIO_Init();  MX_I2C1_Init();  MX_USART3_UART_Init();

    Display_Init();            /* 点亮 OLED */
    Sys_UART_Init();           /* 开启串口接收 */
    Menu_Init(&my_menu);       /* 启动菜单 */

    while (1) {
        Menu_HandleInput();    /* 处理 W/S/D/A */
        Menu_Render();         /* 刷新屏幕（没变化就不刷） */
    }
}
```

**编译 → 烧录 → 打开串口助手（115200 8N1）→ 发送 W/S/D/A。**  
三分钟，不能再多了。

---

## 📂 文件结构

```
mycore/                    ← 这就是整个库
├── inc/
│   ├── menu.h              ← 核心类型和宏（不用改）
│   ├── menuData.h          ← 声明你的命令函数 ✏️
│   ├── display_hal.h       ← 显示抽象层（不用改）
│   └── system_hal.h        ← 系统抽象层（不用改）
├── src/
│   ├── menu.c              ← 菜单引擎（不用改）
│   ├── menuData.c          ← ⭐ 你的文件——写菜单和命令
│   ├── display_hal_stm32.c ← STM32 OLED 驱动（不用改）
│   └── system_hal_stm32.c  ← STM32 串口驱动（不用改）
└── README.md
```

**你只需要改两个文件：**
- `menuData.h` —— 每个命令函数加一行声明
- `menuData.c` —— 写你的命令逻辑和菜单定义

其他文件编译即可，一行都不用动。

---

## ⌨️ 按键说明

| 按键 | 功能 |
|---|---|
| `W` / `w` | 光标**上移**（超出窗口自动上滚） |
| `S` / `s` | 光标**下移**（超出窗口自动下滚） |
| `D` / `d` | **确认**：进入子菜单 / 执行命令 / 触发返回 |
| `A` / `a` | **返回**：回到上一级菜单 / 退出命令模式 |

不分大小写，串口 115200 8N1，即开即用。

---

## 🧩 定义菜单

### 四个宏，记牢就行

| 宏 | 作用 | 参数 |
|---|---|---|
| `MENU_CMD(name, func)` | 菜单项，按 D 执行函数 | `name` = 显示文字，`func` = 函数名 |
| `MENU_SUB(name, sub)` | 菜单项，按 D 进入子菜单 | `name` = 显示文字，`sub` = 子菜单指针 |
| `MENU_BACK` | 返回按钮（自动指向父菜单） | 无 |
| `ITEM_COUNT(arr)` | 编译期计算数组元素个数 | `arr` = 数组名 |

> ⚠️ 这些宏**自带逗号**，数组末尾**不要再加逗号**。

### 做个两级菜单

```c
/* ── 子菜单 ── */
MenuItem_t settings_items[] = {
    MENU_CMD("设置时间", cmd_set_time)
    MENU_CMD("校准",     cmd_cal)
    MENU_BACK
};

Menu_t settings_menu = {
    .title      = "设置",
    .items      = settings_items,
    .item_count = ITEM_COUNT(settings_items),
    .parent     = NULL,          /* ← 填 NULL 就行，Menu_Init 自动修 */
};

/* ── 根菜单 ── */
MenuItem_t main_items[] = {
    MENU_SUB("设置", &settings_menu)
    MENU_CMD("关于",  cmd_about)
};

Menu_t main_menu = {
    .title      = "主菜单",
    .items      = main_items,
    .item_count = ITEM_COUNT(main_items),
    .parent     = NULL,
};
```

两个数组，两个结构体，完事。

---

## 🎯 写命令函数

命令就是普通的 C 函数。按 **D** 的时候，ezUI 调你的函数，你往屏幕上画一帧，然后返回。

> ezUI 采用**非阻塞命令模型**：你的函数画完就回来，主循环继续跑，不会卡住。

### 一次性命令

```c
void cmd_reboot(void) {
    Display_NewFrame();
    Display_PrintString(0, 0,
        "正在重启...", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
    Sys_Delay(500);
    Sys_Reboot();         /* 软件复位 */
}
```

### 带按键处理的命令

想让用户在命令里按别的键？用 `Menu_SetCmdKeyHandler()`：

```c
static uint8_t my_key_handler(char ch) {
    if (ch == 'x' || ch == 'X') {
        /* 做点什么酷的事情 */
        return 1;   /* ← 返回 1 告诉框架要刷新画面 */
    }
    return 0;       /* ← 返回 0 保持当前画面 */
}

void cmd_my_feature(void) {
    Menu_SetCmdKeyHandler(my_key_handler);

    Display_NewFrame();
    Display_PrintString(0, 0,
        "按 X 发射！", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 16,
        "按 A 退出", 16, 16, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}
```

> 按 **A** 会自动退出命令模式，你不需要在处理器里管它。

---

## 🎨 交互式命令

有些命令需要实时刷新——比如动画、调参数、滚动文字。  
ezUI 给了你两个钩子：

| 函数 | 作用 |
|---|---|
| `Menu_SetCmdKeyHandler(handler)` | 命令模式下，每次按键都调用这个函数 |
| `Menu_SetCmdDrawHandler(drawer)` | `Menu_Render()` 需要刷新时，调这个函数画画面 |

### 举个栗子：彩蛋

```c
static int idx = 0;
static const char *frames[] = { "(•_•)", "( •_•)>⌐■-■", "(⌐■_■)" };

static void egg_draw(void) {
    Display_NewFrame();
    Display_PrintString(0, 0,  "~ 彩蛋 ~",
                         16, 16, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 24, frames[idx],
                         16, 16, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

static uint8_t egg_key(char ch) {
    if (ch == 'w' || ch == 'W') { idx = (idx + 1) % 3; return 1; }
    if (ch == 's' || ch == 'S') { idx = (idx + 2) % 3; return 1; }
    return 0;
}

void cmd_egg(void) {
    Menu_SetCmdKeyHandler(egg_key);
    Menu_SetCmdDrawHandler(egg_draw);
    egg_draw();   /* 显示第一帧 */
}
```

**W/S** 切换表情，**A** 退出。`egg_key()` 返回 1 时，`Menu_Render()` 自动调 `egg_draw()` 刷新。

---

## 🔙 子菜单与返回

### `MENU_BACK` 做了什么

`MENU_BACK` 展开成：

```c
{ "返回", MENU_ITEM_BACK, .content.submenu = NULL }
```

你调用 `Menu_Init()` 之后，框架会**自动遍历整棵菜单树**，把每个 `MENU_BACK` 的 `submenu` 指针指向它的父菜单。

### A 键的约定

在任何菜单里按 **A** → 回到上一层。  
在命令模式里按 **A** → 退出命令，回到菜单。

**你永远不需要手动设置 parent 或者 Back 指针。**

---

## 🔄 主循环与初始化

### 初始化顺序

```c
Display_Init();            /* 1. OLED 上电 */
Sys_UART_Init();           /* 2. 开启串口中断接收 */
Menu_Init(&main_menu);     /* 3. 修复菜单指针，显示第一屏 */
```

### 主循环

```c
while (1) {
    Menu_HandleInput();    /* 看看串口有没有数据，有就处理一个按键 */
    Menu_Render();         /* 画面变了就重绘，没变就跳过 */
}
```

### 为什么要这么写

| 特性 | 对你有啥好处 |
|---|---|
| **脏标记刷新** | `Menu_Render()` 没变化就啥也不干。CPU 时间省下来了。 |
| **非阻塞输入** | 没按键？`Menu_HandleInput()` 立刻返回。 |
| **非阻塞命令** | 你的命令画一帧就回来。主循环永远在跑。 |
| **别放延时** | 不需要在主循环里放 `HAL_Delay()`。调两个函数就够了。 |

---

## 🖥️ 显示 API

所有显示操作都通过 `display_hal.h` 抽象接口：

```c
/* 生命周期 */
void Display_Init(void);
void Display_On(void);
void Display_Off(void);

/* 帧缓冲 */
void Display_NewFrame(void);         /* 清空画布 */
void Display_ShowFrame(void);        /* 刷到 OLED */

/* 基本图形 */
void Display_SetPixel(x, y, color);
void Display_DrawLine(x1, y1, x2, y2, color);
void Display_DrawRectangle(x, y, w, h, color);
void Display_DrawFilledRectangle(x, y, w, h, color);
void Display_DrawCircle(x, y, r, color);
void Display_DrawFilledCircle(x, y, r, color);
void Display_DrawImage(x, y, img, color);

/* 文字（每个字 font_w × font_h 像素） */
void Display_PrintString(x, y, str, font_w, font_h, color);
```

---

## ⚙️ 系统 API

```c
uint32_t Sys_GetTick(void);          /* 毫秒计数器 */
void     Sys_Delay(uint32_t ms);     /* 阻塞延时 */
void     Sys_Reboot(void);           /* 软件复位 */

void     Sys_UART_Init(void);        /* 开启串口中断接收 */
uint8_t  Sys_UART_Available(void);   /* 有没有数据 */
char     Sys_UART_ReadChar(void);    /* 读一个字符 */
void     Sys_UART_RxProc(uint8_t ch); /* 中断里调这个函数 */
```

### 串口中断怎么接

在 `stm32f1xx_it.c` 或者 HAL 回调里：

```c
#include "system_hal.h"

void USART3_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE)) {
        uint8_t ch = (uint8_t)(huart3.Instance->DR & 0xFF);
        Sys_UART_RxProc(ch);          /* 扔进环形缓冲区 */
    }
}
```

或者用 HAL 回调：

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        Sys_UART_RxProc(rx_byte);
        HAL_UART_Receive_IT(&huart3, &rx_byte, 1);  /* 重新开启接收 */
    }
}
```

---

## 🤖 For Vibe Coders（写给 AI 与人类协作开发者）

> 你正让 AI 帮你写 ezUI 菜单代码？以下这些坑，AI 最容易踩。  
> 把你看到的这一整节丢给你的 AI 助手，能少炸一半编译错误。

---

### 🖥️ 屏幕显示行数——最常翻车的地方

**硬件：128×64 像素 OLED**

| 区域 | Y 坐标 | 内容 | 高度 |
|------|--------|------|------|
| 标题行 | `y=0` | 菜单标题（固定占用） | 16px |
| 菜单项 1 | `y=20` | `▶ 闪烁 LED` | 16px |
| 菜单项 2 | `y=40` | `设置` | 16px |
| 底部 | `y=60` | ❌ **不要在这里画东西，只有 4px 空间** | |

**关键数字：**
```
字体大小          16×16 像素
行间距            20 像素（16px 字体 + 4px 间隙）
菜单可见项数       2 项（标题占一行，只剩 2 行给菜单）
命令画面最大行数   4 行（y = 0, 16, 32, 48）
每行最大字符数     ≈8 个中文字 / ≈16 个 ASCII 字符
```

### ⚠️ AI 最容易犯的 10 个错误

#### 1. `Display_PrintString` 的 y 坐标算错

```c
// ❌ AI 常犯：y = 60 已经超出显示区
Display_PrintString(0, 60, "你好", 16, 16, DISPLAY_COLOR_NORMAL);

// ✅ 正确：y 最大到 48（第 4 行）
Display_PrintString(0, 48, "你好", 16, 16, DISPLAY_COLOR_NORMAL);
```

**公式：** 第 N 行（从 0 开始）→ `y = N * 16`，最大 N = 3（y = 48）

#### 2. 忘记成对调用 `Display_NewFrame` / `Display_ShowFrame`

```c
// ❌ AI 常犯：只调了一个
void cmd_blink(void) {
    Display_NewFrame();
    Display_PrintString(0, 0, "OK", 16, 16, DISPLAY_COLOR_NORMAL);
    // 忘了 Display_ShowFrame() —— 屏幕不更新！
}

// ✅ 正确：成对使用
void cmd_blink(void) {
    Display_NewFrame();        // ① 清空画布
    Display_PrintString(...);  // ② 画内容
    Display_ShowFrame();       // ③ 刷到屏幕（这步不能少）
}
```

#### 3. 在命令函数里放阻塞循环或 `HAL_Delay`

```c
// ❌ AI 常犯：阻塞主循环
void cmd_long_operation(void) {
    for (int i = 0; i < 100; i++) {
        Display_NewFrame();
        Display_PrintString(..., "处理中 %d%%", ..., i);
        Display_ShowFrame();
        HAL_Delay(50);   // ← 卡死！整个系统停转
    }
}

// ✅ 正确：用 draw handler 实现动画
static uint8_t progress = 0;

static void progress_draw(void) {
    Display_NewFrame();
    Display_PrintString(0, 0, "处理中...", 16, 16, DISPLAY_COLOR_NORMAL);
    // sprintf 展示进度
    Display_ShowFrame();
}

static uint8_t progress_key(char ch) {
    if (ch == 'd' || ch == 'D') { progress++; return 1; }  // 推进进度
    return 0;
}

void cmd_long_operation(void) {
    progress = 0;
    Menu_SetCmdKeyHandler(progress_key);      // ← 按键回调
    Menu_SetCmdDrawHandler(progress_draw);    // ← 绘制回调
    progress_draw();                          // 显示第一帧
}
// 现在主循环可以正常跑，按 D 推进进度，按 A 退出
```

#### 4. `MENU_CMD` / `MENU_SUB` 宏自带逗号，不加额外逗号

```c
// ❌ AI 常犯：手贱加了逗号
MenuItem_t items[] = {
    MENU_CMD("闪烁", cmd_blink),   // ← 编译错误：多余的逗号！
    MENU_CMD("关于", cmd_about),   // ← 编译错误！
    MENU_BACK,                     // ← 编译错误！
};

// ✅ 正确：宏内部已经有逗号了
MenuItem_t items[] = {
    MENU_CMD("闪烁", cmd_blink)
    MENU_CMD("关于", cmd_about)
    MENU_BACK
};
```

#### 5. 忘了在 `menuData.h` 声明函数

每个命令函数需要改动 **2 个文件**：
- `menuData.h` 加声明
- `menuData.c` 加实现和菜单项

#### 6. 字符串超长导致显示截断

```c
// ❌ 超长文字
Display_PrintString(0, 0, "这是一个非常长的设置说明文字", 16, 16, DISPLAY_COLOR_NORMAL);

// ✅ 分行显示
Display_PrintString(0, 0,  "这是一个", 16, 16, DISPLAY_COLOR_NORMAL);
Display_PrintString(0, 16, "设置说明", 16, 16, DISPLAY_COLOR_NORMAL);
```

**每行容量参考：**
| 字体 | 单行最大字符数 |
|------|---------------|
| 16×16（中文） | 8 个汉字 |
| 16×16（英文） | 16 个字符 |

#### 7. `MENU_BACK` 是菜单项类型，不是函数

- `MENU_BACK` = 菜单里的一项，选中后按 D 回到父菜单
- `A` 键 = 在任何菜单按 A 返回上一级
- `A` 键 = 在命令模式按 A 退出命令

#### 8. 在主循环之外调 `Menu_HandleInput` / `Menu_Render`

```c
// ✅ 正确：主循环里调一次就够了
while (1) {
    Menu_HandleInput();   // 一次调用处理一个按键
    Menu_Render();        // 脏了就刷屏，不脏就不刷
}
```

#### 9. `sprintf` 缓冲区溢出

```c
// ❌ 缓冲区太小
char buf[8];
sprintf(buf, "电压: %.2fV", voltage);   // ← 缓冲区溢出！

// ✅ 给够空间
char buf[24];
sprintf(buf, "电压: %.2fV", voltage);
```

#### 10. 根菜单 parent 别动

```c
// ✅ 正确：根菜单 parent = NULL
Menu_t main_menu = {
    .title      = "主菜单",
    .items      = main_items,
    .item_count = ITEM_COUNT(main_items),
    .parent     = NULL,          // ← 别改它
};
```

---

### 🧪 给你的 AI 提示词模板（直接复制用）

```
你正在编写基于 ezUI 框架的 STM32 嵌入式菜单代码。
必须遵守以下规则：
1. OLED 128×64，字体 16×16，每屏最多 4 行文字（y=0,16,32,48）
2. 菜单区只有 2 个可见项，标题占一行
3. Display_NewFrame() 和 Display_ShowFrame() 必须成对使用
4. 命令函数不得包含阻塞循环或 HAL_Delay
5. MENU_CMD/MENU_SUB 宏自带逗号，数组末尾不要加逗号
6. 每个命令函数需要同时在 menuData.h（声明）和 menuData.c（实现）添加
7. 缓冲区 sprintf 给至少 24 字节
8. 根菜单的 parent 必须为 NULL
```

---

## 🔌 移植到其他 MCU

ezUI 把硬件和逻辑分得清清楚楚。换个芯片或屏幕？简单。

### 1. 写 `display_hal_xxx.c`

```c
void Display_Init(void) {
    /* 你的 OLED 初始化序列 */
}

void Display_NewFrame(void) {
    memset(my_buffer, 0, sizeof(my_buffer));
}

void Display_ShowFrame(void) {
    /* I2C 或 SPI：把 my_buffer 发给 OLED */
}

void Display_SetPixel(uint8_t x, uint8_t y, DisplayColor_t color) {
    /* 在 my_buffer 里设一个像素 */
}
```

### 2. 写 `system_hal_xxx.c`

```c
void Sys_UART_Init(void) {
    /* 开启串口中断接收 */
}

void Sys_UART_RxProc(uint8_t ch) {
    /* 写入环形缓冲区 */
}

uint8_t Sys_UART_Available(void) {
    /* 缓冲区有数据就返回 1 */
}

char Sys_UART_ReadChar(void) {
    /* 从缓冲区读一个字符 */
}
```

### 3. 加到工程里

| 文件 | 必须加吗？ |
|---|---|
| `src/menu.c` | ✅ 必须 |
| `src/menuData.c` | ✅ 必须 |
| `src/display_hal_xxx.c` | ✅ 必须（你移植的） |
| `src/system_hal_xxx.c` | ✅ 必须（你移植的） |

头文件路径加：`inc/`

---

## 🧠 设计理念

| 原则 | 为什么 |
|---|---|
| **零 malloc** | 所有结构体静态分配。没有堆，没有碎片。 |
| **脏标记刷新** | 画面没变就不刷屏。省电省时间。 |
| **非阻塞 IO** | 没数据就立刻返回，不耽误事。 |
| **非阻塞命令** | 画一帧就回来，主循环永远在跑。 |
| **自动修复指针** | `Menu_Init()` 帮你遍历整棵树，parent 和 Back 自动接好。 |
| **编译期算数量** | `ITEM_COUNT()` 零运行时开销。 |
| **HAL 抽象** | 换芯片只改两个文件，菜单逻辑不动。 |
| **极小体积** | ROM ~11.6KB，RAM ~3.2KB，含 HAL 库。 |

---

## 📄 License

ezUI 框架代码以 MIT License 发布 — 详见 [LICENSE](LICENSE) 文件。

## 🙏 第三方代码

本项目使用了以下开源代码，向其作者致谢：

| 文件 | 来源 | 作者 | 协议 |
|------|------|------|------|
| `oled.c` / `oled.h` | 波特律动 OLED 驱动 (CH1116) | [keysking](https://space.bilibili.com/51106824) (博哥在学习) | MIT |
| `font.c` / `font.h` | 波特律动字体库 | [keysking](https://space.bilibili.com/51106824) (博哥在学习) | MIT |

> **取模工具**: 波特律动 LED 取模助手 → [https://led.baud-dance.com](https://led.baud-dance.com)

以上文件在原作者的 MIT License 下使用，原始版权声明保留在各文件头部。

---

<p align="center">
  为嵌入式开发者而生。<br>
  <b>ezUI —— 零折腾，零意外，菜单就该这么简单。</b>
</p>

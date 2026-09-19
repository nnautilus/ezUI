#ifndef __SYS_H
#define __SYS_H

#include "ti_msp_dl_config.h"

/* 数据类型定义 */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

/* 位操作宏定义 */
#define BIT(n)                  ((unsigned int)1 << (n))

/* GPIO位操作 */
#define PAout(n)                BIT(n)   /* 输出高电平位值 */

/* 系统时钟频率定义（来自配置） */
#define SYSTEM_CLOCK_FREQ       CPUCLK_FREQ   /* 32MHz */

/* 延时函数声明 */
void delay_us(u32 us);
void delay_ms(u32 ms);

#endif

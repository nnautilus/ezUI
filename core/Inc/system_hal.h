#ifndef __SYSTEM_HAL_H__
#define __SYSTEM_HAL_H__

#include <stdint.h>

/* 获取系统滴答计数值（毫秒） */
uint32_t Sys_GetTick(void);

/* 毫秒延时 */
void Sys_Delay(uint32_t ms);

/* 系统复位 */
void Sys_Reboot(void);

/* 串口初始化（需底层实现调用 HAL_UART_Receive_IT 启动中断接收） */
void Sys_UART_Init(void);

/* 环形缓冲区中是否有可读字符 */
uint8_t Sys_UART_Available(void);

/* 从环形缓冲区读取一个字符 */
char Sys_UART_ReadChar(void);

/* 底层串口接收中断处理（每收到一个字符由中断回调调用） */
void Sys_UART_RxProc(uint8_t ch);

#endif /* __SYSTEM_HAL_H__ */
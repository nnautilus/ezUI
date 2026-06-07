/* system_hal_stm32.c — STM32 系统适配层 */
#include "system_hal.h"
#include "stm32f1xx.h"

/* 串口环形缓冲区 */
#define RING_BUF_SIZE 64

static uint8_t ring_buf[RING_BUF_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

void Sys_UART_Init(void)
{
    /* 启动 USART3 中断接收（单字节） */
    extern UART_HandleTypeDef huart3;
    static uint8_t rx_byte;
    HAL_UART_Receive_IT(&huart3, &rx_byte, 1);
}

void Sys_UART_RxProc(uint8_t ch)
{
    uint8_t next = (head + 1) & (RING_BUF_SIZE - 1);
    if (next != tail) {
        ring_buf[head] = ch;
        head = next;
    }
}

uint8_t Sys_UART_Available(void)
{
    return (uint8_t)((head - tail) & (RING_BUF_SIZE - 1));
}

char Sys_UART_ReadChar(void)
{
    if (head == tail) return 0;
    char ch = (char)ring_buf[tail];
    tail = (tail + 1) & (RING_BUF_SIZE - 1);
    return ch;
}

uint32_t Sys_GetTick(void)
{
    return HAL_GetTick();
}

void Sys_Delay(uint32_t ms)
{
    HAL_Delay(ms);
}

void Sys_Reboot(void)
{
    HAL_NVIC_SystemReset();
}
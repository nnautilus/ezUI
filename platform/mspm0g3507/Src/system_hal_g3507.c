/* system_hal_g3507.c — MSPM0G3507 系统适配层 */
#include "system_hal.h"
#include "ti_msp_dl_config.h"
#include "sys.h"

/* 串口环形缓冲区 */
#define RING_BUF_SIZE 64

static uint8_t ring_buf[RING_BUF_SIZE];
static volatile uint8_t ring_head = 0;
static volatile uint8_t ring_tail = 0;

void Sys_UART_Init(void)
{
    /* 启动 USART3 中断接收（单字节） */
    DL_UART_Main_enableInterrupt(UART_1_INST, DL_UART_MAIN_INTERRUPT_RX);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
}

void Sys_UART_RxProc(uint8_t ch)
{
    uint8_t next = (ring_head + 1) & (RING_BUF_SIZE - 1);
    if (next != ring_tail) {
        ring_buf[ring_head] = ch;
        ring_head = next;
    }
}

uint8_t Sys_UART_Available(void)
{
    return (uint8_t)((ring_head - ring_tail) & (RING_BUF_SIZE - 1));
}

char Sys_UART_ReadChar(void)
{
    if (ring_head == ring_tail) return 0;
    char ch = (char)ring_buf[ring_tail];
    ring_tail = (ring_tail + 1) & (RING_BUF_SIZE - 1);
    return ch;
}

uint32_t Sys_GetTick(void)
{
    return 0;
}

void Sys_Delay(uint32_t ms)
{
    delay_ms((u32)ms);
}

void Sys_Reboot(void)
{
    NVIC_SystemReset();
}

/* UART1 中断服务函数 */
void UART1_IRQHandler(void)
{
    while (!DL_UART_isRXFIFOEmpty(UART_1_INST)) {
        uint8_t ch = (uint8_t)DL_UART_Main_receiveData(UART_1_INST);
        Sys_UART_RxProc(ch);
    }
}

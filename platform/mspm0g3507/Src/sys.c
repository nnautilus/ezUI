#include "sys.h"

/**
 * @brief  微秒级延时 (基于硬件定时器周期计数)
 * @param  us: 延时微秒数
 * @retval 无
 * @note   使用 DriverLib 的 delay_cycles 实现，精度高
 *         系统时钟 32MHz: 1us = 32 cycles
 */
void delay_us(u32 us)
{
    delay_cycles(us * (CPUCLK_FREQ / 1000000));
}

/**
 * @brief  毫秒级延时 (通过delay_us实现)
 * @param  ms: 延时毫秒数
 * @retval 无
 */
void delay_ms(u32 ms)
{
    u32 i;

    for(i = 0; i < ms; i++)
    {
        delay_us(1000);
    }
}

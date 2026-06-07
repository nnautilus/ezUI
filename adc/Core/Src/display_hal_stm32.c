/* display_hal_stm32.c — STM32 显示适配层 */
#include "display_hal.h"
#include "oled.h"
#include "font.h"

void Display_Init(void)
{
    OLED_Init();
}

void Display_On(void)
{
    OLED_DisPlay_On();
}

void Display_Off(void)
{
    OLED_DisPlay_Off();
}

void Display_NewFrame(void)
{
    OLED_NewFrame();
}

void Display_ShowFrame(void)
{
    OLED_ShowFrame();
}

void Display_SetPixel(uint8_t x, uint8_t y, DisplayColor_t color)
{
    OLED_SetPixel(x, y, (OLED_ColorMode)color);
}

void Display_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColor_t color)
{
    OLED_DrawLine(x1, y1, x2, y2, (OLED_ColorMode)color);
}

void Display_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color)
{
    OLED_DrawRectangle(x, y, w, h, (OLED_ColorMode)color);
}

void Display_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color)
{
    OLED_DrawFilledRectangle(x, y, w, h, (OLED_ColorMode)color);
}

void Display_DrawCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color)
{
    OLED_DrawCircle(x, y, r, (OLED_ColorMode)color);
}

void Display_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color)
{
    OLED_DrawFilledCircle(x, y, r, (OLED_ColorMode)color);
}

void Display_DrawImage(uint8_t x, uint8_t y, const void *img, DisplayColor_t color)
{
    OLED_DrawImage(x, y, (const Image *)img, (OLED_ColorMode)color);
}

void Display_PrintString(uint8_t x, uint8_t y, const char *str,
                         uint8_t font_w, uint8_t font_h, DisplayColor_t color)
{
    /* font16x16 是 oled.h 中已有的字体，这里用外部变量 */
    extern const Font font16x16;
    OLED_PrintString(x, y, (char *)str, &font16x16, (OLED_ColorMode)color);
}
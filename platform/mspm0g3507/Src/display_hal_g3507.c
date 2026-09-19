/* display_hal_g3507.c — MSPM0G3507 显示适配层 */
#include "display_hal.h"
#include "oled.h"
#include "string.h"

/* 外部 GRAM 缓冲区，直接清零避免重复刷新 */
extern u8 OLED_GRAM[144][8];

/* color: DISPLAY_COLOR_NORMAL=0 黑底白字, DISPLAY_COLOR_REVERSED=1 白底黑字
 * OLED 驱动 mode: 1=正常显示(白点), 0=反色显示(清除点)
 * 因此 OLED mode = 1 - color */
static inline u8 oled_mode_from_color(DisplayColor_t color)
{
    return (u8)(1 - (int)color);
}

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
    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
}

void Display_ShowFrame(void)
{
    u8 i, n;

    /* Set horizontal addressing mode and full screen address window */
    I2C_Start();
    Send_Byte(0x78);
    I2C_WaitAck();
    Send_Byte(0x00);
    I2C_WaitAck();
    Send_Byte(0x20); I2C_WaitAck();
    Send_Byte(0x00); I2C_WaitAck();
    Send_Byte(0x21); I2C_WaitAck();
    Send_Byte(0x00); I2C_WaitAck();
    Send_Byte(0x7F); I2C_WaitAck();
    Send_Byte(0x22); I2C_WaitAck();
    Send_Byte(0x00); I2C_WaitAck();
    Send_Byte(0x07); I2C_WaitAck();
    I2C_Stop();

    /* Send the entire 1024-byte GRAM in one I2C data transaction */
    I2C_Start();
    Send_Byte(0x78);
    I2C_WaitAck();
    Send_Byte(0x40);
    I2C_WaitAck();
    for (i = 0; i < 8; i++) {
        for (n = 0; n < 128; n++) {
            Send_Byte(OLED_GRAM[n][i]);
            I2C_WaitAck();
        }
    }
    I2C_Stop();
}

void Display_SetPixel(uint8_t x, uint8_t y, DisplayColor_t color)
{
    if (x >= 128 || y >= 64) return;
    OLED_DrawPoint((u8)x, (u8)y, oled_mode_from_color(color));
}

void Display_PrintString(uint8_t x, uint8_t y, const char *str,
                         uint8_t font_w, uint8_t font_h, DisplayColor_t color)
{
    /* font16x16 是 oled.h 中已有的字体，这里用外部变量 */
    (void)font_w;
    (void)font_h;
    OLED_ShowString((u8)x, (u8)y, (u8 *)str, 16, oled_mode_from_color(color));
}

void Display_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColor_t color)
{
    OLED_DrawLine((u8)x1, (u8)y1, (u8)x2, (u8)y2, oled_mode_from_color(color));
}

void Display_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color)
{
    u8 x1 = x;
    u8 y1 = y;
    u8 x2 = (uint8_t)(x + w - 1);
    u8 y2 = (uint8_t)(y + h - 1);
    u8 mode = oled_mode_from_color(color);

    if (w == 0 || h == 0) return;

    OLED_DrawLine(x1, y1, x2, y1, mode);
    OLED_DrawLine(x1, y2, x2, y2, mode);
    OLED_DrawLine(x1, y1, x1, y2, mode);
    OLED_DrawLine(x2, y1, x2, y2, mode);
}

void Display_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color)
{
    u8 mode = oled_mode_from_color(color);
    uint8_t i, j;

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            OLED_DrawPoint((u8)(x + i), (u8)(y + j), mode);
        }
    }
}

void Display_DrawCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color)
{
    /* OLED_DrawCircle 固定画白点，这里用中点圆算法按颜色绘制 */
    int16_t xc = (int16_t)x;
    int16_t yc = (int16_t)y;
    int16_t radius = (int16_t)r;
    int16_t a = 0, b = radius;
    int16_t d = 3 - 2 * radius;
    u8 mode = oled_mode_from_color(color);

    while (a <= b) {
        OLED_DrawPoint((u8)(xc + a), (u8)(yc - b), mode);
        OLED_DrawPoint((u8)(xc - a), (u8)(yc - b), mode);
        OLED_DrawPoint((u8)(xc - a), (u8)(yc + b), mode);
        OLED_DrawPoint((u8)(xc + a), (u8)(yc + b), mode);
        OLED_DrawPoint((u8)(xc + b), (u8)(yc + a), mode);
        OLED_DrawPoint((u8)(xc + b), (u8)(yc - a), mode);
        OLED_DrawPoint((u8)(xc - b), (u8)(yc - a), mode);
        OLED_DrawPoint((u8)(xc - b), (u8)(yc + a), mode);

        a++;
        if (d < 0) {
            d = d + 4 * a + 6;
        } else {
            d = d + 4 * (a - b) + 10;
            b--;
        }
    }
}

void Display_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color)
{
    int16_t xc = (int16_t)x;
    int16_t yc = (int16_t)y;
    int16_t radius = (int16_t)r;
    int16_t a = 0, b = radius;
    int16_t d = 3 - 2 * radius;
    u8 mode = oled_mode_from_color(color);
    int16_t i;

    while (a <= b) {
        for (i = xc - a; i <= xc + a; i++) {
            OLED_DrawPoint((u8)i, (u8)(yc + b), mode);
            OLED_DrawPoint((u8)i, (u8)(yc - b), mode);
        }
        for (i = xc - b; i <= xc + b; i++) {
            OLED_DrawPoint((u8)i, (u8)(yc + a), mode);
            OLED_DrawPoint((u8)i, (u8)(yc - a), mode);
        }

        a++;
        if (d < 0) {
            d = d + 4 * a + 6;
        } else {
            d = d + 4 * (a - b) + 10;
            b--;
        }
    }
}

void Display_DrawImage(uint8_t x, uint8_t y, const void *img, DisplayColor_t color)
{
    /* 当前菜单未使用图片，保留接口占位 */
    (void)x;
    (void)y;
    (void)img;
    (void)color;
}

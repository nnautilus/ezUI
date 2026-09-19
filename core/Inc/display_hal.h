#ifndef __DISPLAY_HAL_H__
#define __DISPLAY_HAL_H__

#include <stdint.h>

typedef enum {
    DISPLAY_COLOR_NORMAL = 0,   /* 正常模式 黑底白字 */
    DISPLAY_COLOR_REVERSED      /* 反色模式 白底黑字 */
} DisplayColor_t;

/* 初始化显示硬件 */
void Display_Init(void);

/* 打开显示 */
void Display_On(void);

/* 关闭显示 */
void Display_Off(void);

/* 开始绘制新帧（清空缓冲区） */
void Display_NewFrame(void);

/* 将缓冲区刷新到屏幕 */
void Display_ShowFrame(void);

/* 在 (x, y) 处画一个像素点 */
void Display_SetPixel(uint8_t x, uint8_t y, DisplayColor_t color);

/* 从 (x1, y1) 到 (x2, y2) 画直线 */
void Display_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColor_t color);

/* 画矩形边框 */
void Display_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color);

/* 画实心矩形 */
void Display_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, DisplayColor_t color);

/* 画圆形边框 */
void Display_DrawCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color);

/* 画实心圆 */
void Display_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, DisplayColor_t color);

/* 在 (x, y) 处显示图像 */
void Display_DrawImage(uint8_t x, uint8_t y, const void *img, DisplayColor_t color);

/* 在 (x, y) 处显示字符串 */
void Display_PrintString(uint8_t x, uint8_t y, const char *str,
                         uint8_t font_w, uint8_t font_h, DisplayColor_t color);

#endif /* __DISPLAY_HAL_H__ */
#include "menu.h"
#include "menuData.h"
#include "display_hal.h"
#include "system_hal.h"
#include "stdio.h"

#define FONT_W 16
#define FONT_H 16

/* ======================== 命令函数 ======================== */

void cmd_set_time(void){
    Display_NewFrame();
    Display_PrintString(0, 0, "Set Time Mode", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 16, "Use encoder...", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 48, "Press A to back", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

void cmd_reboot(void){
    Display_NewFrame();
    Display_PrintString(0, 0, "Rebooting...", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
    Sys_Delay(500);
    Sys_Reboot();
}

void cmd_ADC(void){
    Display_NewFrame();
    Display_PrintString(0, 0, "ADC Mode", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 16, "Not implemented", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 48, "Press A to back", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

/* ======================== About 彩蛋（保留 ddddd 进入） ======================== */

static int easter_egg_idx = 0;
static int easter_egg_active = 0;
static int d_cnt = 0;

static const char* kaomoji[] = {
    "(^-^)",
    "(>_<)",
    "(T_T)",
    "(^_^)",
    "(-_-)",
    "(O_O)",
    "(+_+)",
    "(@_@)",
};
#define KAOMOJI_N  (sizeof(kaomoji)/sizeof(kaomoji[0]))

static void easter_egg_draw(void);

static uint8_t about_key_handler(char ch)
{
    if (easter_egg_active) {
        /* 彩蛋互动模式 */
        if (ch == 'w' || ch == 'W') {
            easter_egg_idx--;
            if (easter_egg_idx < 0) easter_egg_idx = KAOMOJI_N - 1;
            return 1;
        }
        if (ch == 's' || ch == 'S') {
            easter_egg_idx++;
            if (easter_egg_idx >= (int)KAOMOJI_N) easter_egg_idx = 0;
            return 1;
        }
        return 0;
    }

    /* 主屏模式：连续 5 个 d 进入彩蛋 */
    if (ch == 'd' || ch == 'D') {
        d_cnt++;
        if (d_cnt >= 5) {
            easter_egg_active = 1;
            easter_egg_idx = 0;
            d_cnt = 0;
            Menu_SetCmdDrawHandler(easter_egg_draw);
            return 1;
        }
        return 0;
    }
    d_cnt = 0;
    return 0;
}

void cmd_about(void){
    d_cnt = 0;
    easter_egg_active = 0;
    easter_egg_idx = 0;

    Menu_SetCmdKeyHandler(about_key_handler);

    Display_NewFrame();
    Display_PrintString(0, 0, "ezUI V2.0", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 32, "June 2026", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 48, "MIT License", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

static void easter_egg_draw(void)
{
    Display_NewFrame();
    Display_PrintString(0, 0,  "~ Easter Egg ~", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 24, kaomoji[easter_egg_idx], FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_PrintString(0, 48, "W/S next A back", FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    Display_ShowFrame();
}

/* ======================== 菜单定义（对齐 README demo 结构） ======================== */

MenuItem_t settings_items[] = {
    MENU_CMD("Set Time", cmd_set_time)
    MENU_CMD("ADC",      cmd_ADC)
    MENU_CMD("Reboot",   cmd_reboot)
    MENU_BACK
};

Menu_t settings_menu = {
    .title      = "Settings",
    .items      = settings_items,
    .item_count = ITEM_COUNT(settings_items),
    .parent     = NULL,
};

MenuItem_t main_menu_items[] = {
    MENU_SUB("Settings", &settings_menu)
    MENU_CMD("About",    cmd_about)
};

Menu_t main_menu = {
    .title      = "Dashboard",
    .items      = main_menu_items,
    .item_count = ITEM_COUNT(main_menu_items),
    .parent     = NULL,
};
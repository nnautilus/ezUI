#include "menu.h"
#include "menuData.h"
#include "stdio.h"
#include "adc.h"
#include "key.h"

void cmd_set_time(void);
void cmd_ADC(void);
void cmd_reboot(void);
void cmd_about(void);

extern Menu_t main_menu;

Menu_t settings_menu = {
    .title = "Settings",
    .items = (const MenuItem_t[]){
        {"Set time", MENU_ITEM_COMMAND, .content.command = cmd_set_time},
        {"ADC", MENU_ITEM_COMMAND, .content.command = cmd_ADC},
        {"Reboot", MENU_ITEM_COMMAND, .content.command = cmd_reboot},
        {"Back", MENU_ITEM_SUBMENU, .content.submenu = &main_menu}
    }, 
    .item_count = 4,
    .parent = &main_menu
};

Menu_t main_menu = {
    .title = "Menu",
    .items = (const MenuItem_t[]){
        {"Settings", MENU_ITEM_SUBMENU, .content.submenu = &settings_menu},
        {"About", MENU_ITEM_COMMAND, .content.command = cmd_about}
    },
    .item_count = 2,
    .parent = NULL
};

void cmd_set_time(void){
    OLED_NewFrame();
    OLED_PrintString(0, 0, "Set Time Mode", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 16, "Use encoder...", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    HAL_Delay(1000);
    Menu_Render();
}
void cmd_ADC(void){
    OLED_NewFrame();
    OLED_PrintString(0, 0, "ADC starting", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    HAL_Delay(500);
    while(1){
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        float adc = HAL_ADC_GetValue(&hadc1);
        char message[10];
        sprintf(message, "adc:%f", adc);
        OLED_NewFrame();
        OLED_PrintString(0, 0, message, &font16x16, OLED_COLOR_NORMAL);
        OLED_ShowFrame();
        if (getkey() == KEY_2) break;
    }
    Menu_Render();
}
void cmd_reboot(void){
    OLED_NewFrame();
    OLED_PrintString(0, 0, "Rebooting...", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    HAL_Delay(500);
    HAL_NVIC_SystemReset();
    Menu_Render();
}
void cmd_about(void){
    OLED_NewFrame();
    OLED_PrintString(0, 0, "ezUI V1.2", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 32, "25.10.22", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 48, "Powered by Naut", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    while (1){
        if (getkey() == KEY_2) break;
    }
    Menu_Render();
}

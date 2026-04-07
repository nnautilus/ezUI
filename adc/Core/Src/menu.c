#include "menu.h"
#include "menuData.h"
#include "stm32f1xx.h"
#include "key.h"
#include "oled.h"
#include "string.h"
#include "stdio.h"

static const Menu_t* current_menu = NULL;
static uint8_t selected_index = 0;

void Menu_Init(const Menu_t* root){
    current_menu = root;
    selected_index = 0;
}

void Menu_Render(void){
    OLED_NewFrame();
    OLED_PrintString(0, 0, (char*)current_menu->title, &font16x16, OLED_COLOR_NORMAL);
    for (int i = 0; i < current_menu->item_count; i++){
        char line[16] = {0};
        sprintf(line, "%s %s", (i == selected_index)? ">" : " ", current_menu->items[i].name);
        OLED_PrintString(0, 16 * i, (char*)line, &font16x16, OLED_COLOR_NORMAL);
    }
    OLED_ShowFrame();
}

void Menu_HandleInput(void){
    int key = getkey();
    switch (key)
    {
    case KEY_1:
        if (selected_index >= current_menu->item_count - 1) 
            selected_index = 0;
        else 
            selected_index++;
        Menu_Render();
        HAL_Delay(150);
        break;
    case KEY_2:{
        const MenuItem_t* item = &current_menu->items[selected_index];
        if (item->Type == MENU_ITEM_SUBMENU){
            current_menu = item->content.submenu;
            selected_index = 0;
            Menu_Render();
        } else if (item->Type == MENU_ITEM_COMMAND){
            item->content.command();
            Menu_Render();
        }
        HAL_Delay(150);
        }
        break;
    default:
        break;
    }
}

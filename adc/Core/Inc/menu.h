#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include <oled.h>

typedef void (*MenuFunc_t)(void);

typedef enum{
    MENU_ITEM_SUBMENU,
    MENU_ITEM_COMMAND
} MenuItemType_t;

struct Menu;

typedef struct{
    const char* name;
    MenuItemType_t Type;
    union {
        const struct Menu* submenu;
        MenuFunc_t command;
    } content; 
} MenuItem_t;

typedef struct Menu{
    const char* title;
    const MenuItem_t* items;
    uint8_t item_count;
    const struct Menu* parent;
} Menu_t;

void Menu_Init(const Menu_t* root);
void Menu_Render(void);
void Menu_HandleInput(void);

#endif

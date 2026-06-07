#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include "display_hal.h"

typedef void (*MenuFunc_t)(void);

/* ======================== 内部类型（不直接使用） ======================== */
typedef enum {
    MENU_ITEM_SUBMENU,
    MENU_ITEM_COMMAND,
    MENU_ITEM_BACK
} MenuItemType_t;

struct Menu;

typedef struct {
    const char* name;
    MenuItemType_t Type;
    union {
        struct Menu* submenu;
        MenuFunc_t command;
    } content;
} MenuItem_t;

typedef struct Menu {
    const char* title;
    MenuItem_t* items;
    uint8_t item_count;
    struct Menu* parent;
} Menu_t;

/* ======================== API ======================== */
void Menu_Init(Menu_t* root);
void Menu_Render(void);
void Menu_HandleInput(void);

/* 命令模式控制（给命令函数使用） */
void Menu_SetCmdKeyHandler(uint8_t (*handler)(char ch));
void Menu_SetCmdDrawHandler(void (*drawer)(void));
void Menu_ExitCommand(void);
void Menu_RequestRedraw(void);

/* ======================== 辅助宏 ======================== */
#define ITEM_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

/* 菜单项（各展开为一条初始化条目） */
#define MENU_SUB(name_, sub)    { (name_), MENU_ITEM_SUBMENU,  .content.submenu = (sub) },
#define MENU_CMD(name_, func)   { (name_), MENU_ITEM_COMMAND,  .content.command = (func) },
#define MENU_BACK               { "Back",  MENU_ITEM_BACK,     .content.submenu = NULL },

#endif /* __MENU_H */
#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>

/* 菜单项类型 */
typedef enum {
    MENU_ITEM_COMMAND,   /* 执行命令函数 */
    MENU_ITEM_SUBMENU,   /* 进入子菜单 */
    MENU_ITEM_BACK       /* 返回上一级菜单 */
} MenuItemType_t;

/* 前向声明菜单结构体 */
typedef struct Menu_s Menu_t;

/* 菜单项 */
typedef struct MenuItem {
    char *name;
    MenuItemType_t Type;
    union {
        void (*command)(void);   /* MENU_ITEM_COMMAND 时调用 */
        Menu_t *submenu;         /* MENU_ITEM_SUBMENU 时指向子菜单 */
    } content;
} MenuItem_t;

/* 菜单 */
struct Menu_s {
    char *title;
    MenuItem_t *items;
    int item_count;
    Menu_t *parent;
};

/* 菜单项宏，用于数组初始化（末尾不要再加逗号） */
#define MENU_CMD(name, func)    { name, MENU_ITEM_COMMAND, .content.command = func },
#define MENU_SUB(name, sub)     { name, MENU_ITEM_SUBMENU, .content.submenu = sub },
#define MENU_BACK               { "Back", MENU_ITEM_BACK, .content.submenu = NULL },

/* 计算数组元素个数 */
#define ITEM_COUNT(arr)         (sizeof(arr) / sizeof((arr)[0]))

/* 初始化菜单系统，自动修复 parent 与 Back 指针 */
void Menu_Init(Menu_t* root);

/* 设置命令模式下的按键处理回调 */
void Menu_SetCmdKeyHandler(uint8_t (*handler)(char ch));

/* 设置命令模式下的绘制回调 */
void Menu_SetCmdDrawHandler(void (*drawer)(void));

/* 退出命令模式，返回菜单 */
void Menu_ExitCommand(void);

/* 请求重绘当前界面 */
void Menu_RequestRedraw(void);

/* 渲染当前菜单或命令界面（仅在画面变化时刷新） */
void Menu_Render(void);

/* 处理一个串口输入字符 */
void Menu_HandleInput(void);

#endif /* __MENU_H__ */

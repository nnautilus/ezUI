#include "menu.h"
#include "display_hal.h"
#include "system_hal.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

static Menu_t* current_menu = NULL;
static int8_t last_selected_index = -1;
static uint8_t selected_index = 0;
static int16_t scroll_offset = 0;        /* 当前滚动窗口起始下标 */

#define FONT_W      16
#define FONT_H      16
#define ROW_PITCH   20                   /* 行距（16px 字体 + 4px 间隙） */
#define MAX_VISIBLE_ITEMS  2             /* 除去标题行，一页显示 2 个菜单项 */

/* ======================== 状态机 ======================== */
typedef enum {
    UI_STATE_MENU,      /* 普通菜单导航 */
    UI_STATE_COMMAND    /* 命令活动（绘制一次，由 HandleInput 接管按键） */
} UIState_t;

static UIState_t ui_state = UI_STATE_MENU;
static uint8_t menu_dirty = 1;           /* 需要刷新屏幕 */
static uint8_t (*cmd_key_handler)(char ch) = NULL;  /* 命令自定义按键处理 */
static void (*cmd_draw_handler)(void) = NULL;        /* 命令自定义绘制回调 */

/* ======================== 内部：递归修复 parent 和 Back 按钮 ======================== */
static void menu_fix_pointers(Menu_t* menu)
{
    if (!menu) return;

    for (int i = 0; i < menu->item_count; i++) {
        MenuItem_t* item = &menu->items[i];

        if (item->Type == MENU_ITEM_SUBMENU && item->content.submenu) {
            Menu_t* sub = item->content.submenu;
            sub->parent = menu;             /* 设置 parent 指向 */
            menu_fix_pointers(sub);         /* 递归修复子菜单 */
        }

        if (item->Type == MENU_ITEM_BACK) {
            item->content.submenu = menu->parent;  /* Back 指向父菜单 */
        }
    }
}

/* ======================== 公开 API ======================== */

void Menu_Init(Menu_t* root)
{
    current_menu = root;
    selected_index = 0;
    scroll_offset = 0;
    ui_state = UI_STATE_MENU;
    cmd_key_handler = NULL;
    menu_dirty = 1;
    menu_fix_pointers(current_menu);
}

void Menu_SetCmdKeyHandler(uint8_t (*handler)(char ch))
{
    cmd_key_handler = handler;
}

void Menu_SetCmdDrawHandler(void (*drawer)(void))
{
    cmd_draw_handler = drawer;
}

void Menu_ExitCommand(void)
{
    ui_state = UI_STATE_MENU;
    cmd_key_handler = NULL;
    cmd_draw_handler = NULL;
    menu_dirty = 1;
}

void Menu_RequestRedraw(void)
{
    menu_dirty = 1;
}

void Menu_Render(void)
{
    if (!menu_dirty) return;               /* 脏标记：无变化不刷新 */
    menu_dirty = 0;

    if (ui_state == UI_STATE_COMMAND) {
        if (cmd_draw_handler) {
            cmd_draw_handler();            /* 调用命令自定义绘制 */
            Display_ShowFrame();
        }
        return;
    }

    Display_NewFrame();

    /* 标题行 */
    Display_PrintString(0, 0, current_menu->title, FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);

    /* 滚动显示菜单项 */
    int16_t start = scroll_offset;
    int16_t end = start + MAX_VISIBLE_ITEMS;
    if (end > current_menu->item_count)
        end = current_menu->item_count;

    for (int i = start; i < end; i++) {
        char line[16] = {0};
        sprintf(line, "%s %s",
                (i == selected_index) ? ">" : " ",
                current_menu->items[i].name);
        Display_PrintString(0, ROW_PITCH * (i - start + 1), line,
                            FONT_W, FONT_H, DISPLAY_COLOR_NORMAL);
    }
    Display_ShowFrame();
}

void Menu_HandleInput(void)
{
    if (!Sys_UART_Available()) return;

    char ch = Sys_UART_ReadChar();

    /* ---------- 命令状态：交由命令按键处理器 ---------- */
    if (ui_state == UI_STATE_COMMAND) {
        if (ch == 'a' || ch == 'A') {
            /* A 键默认退出命令，返回菜单 */
            Menu_ExitCommand();
            return;
        }
        if (cmd_key_handler) {
            if (cmd_key_handler(ch))
                menu_dirty = 1;   /* handler 返回非 0 表示需要刷新 */
        }
        return;
    }

    /* ---------- 菜单导航状态 ---------- */
    switch (ch) {
        case 'w':
        case 'W':
            if (selected_index > 0) {
                selected_index--;
                if (selected_index < scroll_offset)
                    scroll_offset = selected_index;
                menu_dirty = 1;
            }
            break;
        case 's':
        case 'S':
            if (selected_index < current_menu->item_count - 1) {
                selected_index++;
                if (selected_index >= scroll_offset + MAX_VISIBLE_ITEMS)
                    scroll_offset = selected_index - MAX_VISIBLE_ITEMS + 1;
                menu_dirty = 1;
            }
            break;
        case 'd':
        case 'D': {
            MenuItem_t* item = &current_menu->items[selected_index];
            if (item->Type == MENU_ITEM_SUBMENU && item->content.submenu) {
                current_menu = item->content.submenu;
                selected_index = 0;
                scroll_offset = 0;
                menu_dirty = 1;
            } else if (item->Type == MENU_ITEM_BACK) {
                if (current_menu->parent) {
                    current_menu = current_menu->parent;
                    selected_index = 0;
                    scroll_offset = 0;
                    menu_dirty = 1;
                }
            } else {
                /* 进入命令模式：命令函数绘制一次后立即返回 */
                cmd_key_handler = NULL;
                ui_state = UI_STATE_COMMAND;
                menu_dirty = 1;
                item->content.command();     /* 命令函数绘制一帧 */
                Menu_Render();               /* 立即刷新到屏幕 */
            }
            break;
        }
        case 'a':
        case 'A':
            if (current_menu->parent != NULL) {
                current_menu = current_menu->parent;
                selected_index = 0;
                scroll_offset = 0;
                menu_dirty = 1;
            }
            break;
        default:
            break;
    }
}
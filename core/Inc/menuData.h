#ifndef __MENU_DATA_H__
#define __MENU_DATA_H__

#include "menu.h"

/* 命令函数声明 */
void cmd_set_time(void);
void cmd_reboot(void);
void cmd_ADC(void);
void cmd_about(void);

/* 根菜单声明 */
extern Menu_t main_menu;

#endif /* __MENU_DATA_H__ */

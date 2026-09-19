#include "ti_msp_dl_config.h"
#include "menu.h"
#include "menuData.h"
#include "display_hal.h"
#include "system_hal.h"
#include "sys.h"

int main(void)
{
    SYSCFG_DL_init();

    Display_Init();
    delay_ms(100);

    Sys_UART_Init();
    Menu_Init(&main_menu);

    while (1) {
        Menu_HandleInput();
        Menu_Render();
    }
}

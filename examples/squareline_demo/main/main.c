#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lv_port.h"


void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());
    lv_port_init();

    lv_port_sem_take();
    ui_init();
    lv_port_sem_give();
}

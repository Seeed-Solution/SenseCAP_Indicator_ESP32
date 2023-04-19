#include "terminal_model.h"
#include "terminal_storage.h"
#include "terminal_wifi.h"
#include "terminal_display.h"
#include "terminal_time.h"
#include "terminal_btn.h"
#include "terminal_city.h"

int terminal_model_init(void)
{
    terminal_storage_init();
    terminal_sensor_init();
    terminal_wifi_init();
    terminal_time_init();
    terminal_city_init();
    terminal_display_init();  // lcd bl on
    terminal_btn_init();
}
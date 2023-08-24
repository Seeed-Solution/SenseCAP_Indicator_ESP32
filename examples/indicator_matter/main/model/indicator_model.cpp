#include "indicator_model.h"
#include "indicator_sensor.h"
#include "indicator_display.h"
#include "indicator_time.h"
#include "indicator_btn.h"
#include "indicator_city.h"
#include "indicator_matter.h"

int indicator_model_init(void)
{
    indicator_sensor_init();
    indicator_time_init();
    indicator_city_init();
    indicator_display_init();  // lcd bl on
    indicator_btn_init();
    indicator_matter_init();
    return 0;
}
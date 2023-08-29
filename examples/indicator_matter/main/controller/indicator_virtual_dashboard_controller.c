#include "indicator_virtual_dashboard_controller.h"
#include "lvgl.h"
#include "ui.h"
#include "indicator_view.h"
#include "indicator_virtual_dashboard.h"

#include "nvs.h"
#include "freertos/semphr.h"
#include "ui_helpers.h"
#include <time.h>
#include <sys/time.h>

static const char *TAG = "indicator_virtual_dashboard_controller";

static void __dashboard_arc_update_cb(lv_event_t * e)
{
    lv_arc_t * arc =  (lv_arc_t *)lv_event_get_target(e);  
    struct view_data_matter_dashboard_data data;
    data.dashboard_data_type = DASHBOARD_DATA_ARC;
    data.value = lv_arc_get_value(arc);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA, \
                    &data, sizeof(struct view_data_matter_dashboard_data ), portMAX_DELAY);
}

static void __dashboard_slider_update_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    struct view_data_matter_dashboard_data data;
    data.dashboard_data_type = DASHBOARD_DATA_SLIDER;
    data.value = lv_slider_get_value(slider);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA, \
                    &data, sizeof(struct view_data_matter_dashboard_data ), portMAX_DELAY);
}

static void __dashboard_switch_update_cb(lv_event_t * e)
{
    lv_obj_t * switch1 = lv_event_get_target(e);
    struct view_data_matter_dashboard_data data;
    data.dashboard_data_type = DASHBOARD_DATA_SWITCH;
    data.value = (int)lv_obj_has_state(switch1, LV_STATE_CHECKED);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA, \
                    &data, sizeof(struct view_data_matter_dashboard_data ), portMAX_DELAY);
}

static void __dashboard_button_1_update_cb(lv_event_t * e)
{
    lv_obj_t * button = lv_event_get_target(e);
    struct view_data_matter_dashboard_data data;
    data.dashboard_data_type = DASHBOARD_DATA_BUTTON1;
    data.value = (int)lv_obj_has_state(button, LV_STATE_CHECKED);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA, \
                    &data, sizeof(struct view_data_matter_dashboard_data ), portMAX_DELAY);
}

static void __dashboard_button_2_update_cb(lv_event_t * e)
{
    lv_obj_t * button = lv_event_get_target(e);
    struct view_data_matter_dashboard_data data;
    data.dashboard_data_type = DASHBOARD_DATA_BUTTON2;
    data.value = (int)lv_obj_has_state(button, LV_STATE_CHECKED);

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_MATTER_DASHBOARD_DATA, \
                    &data, sizeof(struct view_data_matter_dashboard_data ), portMAX_DELAY);
}

static void __virtual_dashboard_event_init(void)
{
    lv_obj_add_event_cb(ui_arc, __dashboard_arc_update_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_toggle_button1, __dashboard_button_1_update_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_toggle_button2, __dashboard_button_2_update_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_slider1, __dashboard_slider_update_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_switch1, __dashboard_switch_update_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

int indicator_virtual_dashboard_controller_init(void)
{ 
    __virtual_dashboard_event_init();

    return 0;
}
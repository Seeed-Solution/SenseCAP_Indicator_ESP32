#include "ui.h"
#include "ui_events.h"

#include "view_data.h"

void light_ctrl(bool light_st)
{
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LIGHT_ST, (void *)&light_st, sizeof(bool), portMAX_DELAY);
}

void lora_light_on(lv_event_t *e)
{
    light_ctrl(true);
}
void lora_light_off(lv_event_t *e)
{
    light_ctrl(false);
}
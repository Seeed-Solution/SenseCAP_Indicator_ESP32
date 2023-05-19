#include "indicator_view_ha.h"
#include "ui.h"

static const char *TAG = "view-ha";

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    lv_port_sem_take();
    switch (id)
    {
        case VIEW_EVENT_HA_SENSOR: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SENSOR");
            struct view_data_ha_sensor_data *p_data = (struct view_data_ha_sensor_data *)event_data;
            
            switch (p_data->index)
            {
                case 0: {
                    lv_label_set_text(ui_sensor1_data, p_data->value);
                    break;
                }
                case 1: {
                     lv_label_set_text(ui_sensor2_data, p_data->value);
                    break;
                }  
                case 2: {
                     lv_label_set_text(ui_sensor3_data, p_data->value);
                    break;
                }
                case 3: {
                     lv_label_set_text(ui_sensor4_data, p_data->value);
                    break;
                }
                case 4: {
                     lv_label_set_text(ui_sensor5_data, p_data->value);
                    break;
                }
                case 5: {
                     lv_label_set_text(ui_sensor6_data, p_data->value);
                    break;
                }  
                default:
                    break;
            }
            break;
        }

        case VIEW_EVENT_HA_SWITCH_SET: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_HA_SWITCH_SET");
            struct view_data_ha_switch_data *p_data = (struct view_data_ha_switch_data *)event_data;
            
            ESP_LOGI(TAG, "set switch%d: %d", (p_data->index)+1, p_data->value);
            switch (p_data->index)
            {
                case 0: {
                    
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch1, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch1, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch1, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 1: {
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch2, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch2, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch2, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 2: {
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch3, LV_STATE_CHECKED);
                        lv_obj_add_state(ui_switch3_switch, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch3, LV_STATE_CHECKED);
                        lv_obj_clear_state(ui_switch3_switch, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch3, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 3: {
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch4, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch4, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch4, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 4: {
                    lv_arc_set_value(ui_switch5_arc, p_data->value); //todo check value
                    lv_event_send((lv_obj_t * )ui_switch5_arc, LV_EVENT_VALUE_CHANGED, NULL);
                    break;
                }
                case 5: {
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch6, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch6, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch6, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 6: {
                    if( p_data->value ) {
                        lv_obj_add_state(ui_switch7, LV_STATE_CHECKED);
                        lv_obj_add_state(ui_switch7_switch, LV_STATE_CHECKED);
                    } else {
                        lv_obj_clear_state(ui_switch7, LV_STATE_CHECKED);
                        lv_obj_clear_state(ui_switch7_switch, LV_STATE_CHECKED);
                    }
                    lv_event_send((lv_obj_t * )ui_switch7, LV_EVENT_CLICKED, NULL);
                    break;
                }
                case 7: {
                    lv_slider_set_value(ui_switch8_slider, p_data->value, true); //todo check value
                    lv_event_send((lv_obj_t * )ui_switch8_slider, LV_EVENT_VALUE_CHANGED, NULL);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    lv_port_sem_give();
}


int indicator_view_ha_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_HA_SENSOR, 
                                                            __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_HA_SWITCH_SET, 
                                                            __view_event_handler, NULL, NULL));
}
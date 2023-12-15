/**
 * @file lorawan_view.view
 * @brief LoRaWAN Configuration Interface
 *
 * This file provides an interface for configuring and testing the LoRaWAN network parameters.
 * It includes user interaction for setting network information, performing tests, and displaying results.
 * The view is designed to be user-friendly, providing clear prompts and feedback.
 *
 * Features:
 * - Set LoRaWAN Network Information (EUI, App Key, etc.)
 * - Perform various LoRaWAN tests (such as signal strength, connectivity, etc.)
 * - Display results in a readable format
 *
 * @author Spencer
 * @version 0.1
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2023 Seeed Studio
 *
 * @note This file is part of the LoRaWAN application developed for Seeed Studio.
 */
#include "indicator_lorawan.h"
#include "ui.h"

#include "RegionCommon.h"
static const char *TAG = "lorawan_view";

static struct view_data_lorawan_network_info lorawan_network_info;
static struct view_data_lorawan_basic_cfg    lorawan_basic_cfg;

static void update_panel_network_info(struct view_data_lorawan_network_info *info);
static void update_panel_network_cfg(struct view_data_lorawan_basic_cfg *cfg);
static void update_lorawan_network_status(enum view_data_lorawan_join status);

static void __view_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    switch (event_id) {
        case VIEW_EVENT_LORAWAN_JOIN_ST:
            update_lorawan_network_status(*(enum view_data_lorawan_join *)event_data);
            break;
        case VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE: // restore after power-on & info updated from console
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE");
            memcpy(&lorawan_network_info, event_data, sizeof(struct view_data_lorawan_network_info));
            update_panel_network_info(&lorawan_network_info);
            break;
        case VIEW_EVENT_LORAWAN_CFG_UPDATE: // cfg read after power-on
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_CFG_UPDATE");
            memcpy(&lorawan_basic_cfg, event_data, sizeof(struct view_data_lorawan_basic_cfg));
            update_panel_network_cfg(&lorawan_basic_cfg);
            lorawan_basic_cfg_printf(&lorawan_basic_cfg);
            break;
        case VIEW_EVENT_LORAWAN_CFG_APPLY: // When updating cfg | | from the console, updating oneself in the view
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_CFG_APPLY");
            memcpy(&lorawan_basic_cfg, event_data, sizeof(struct view_data_lorawan_basic_cfg));
            update_panel_network_cfg(&lorawan_basic_cfg);
            break;
        case VIEW_EVENT_LORAWAN_LIGHT_ST: // Update light data from `view` and TTN
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_LIGHT_ST");
            bool light_st = *(bool *)event_data;
            ESP_LOGI(TAG, "light_st = %d", light_st);
            lv_port_sem_take();
            if (light_st) {
                lv_obj_add_state(ui_SwitchBulb, LV_STATE_CHECKED);
                _ui_image_set_property(ui_ImgBulb, _UI_IMAGE_PROPERTY_IMAGE, &ui_img_bulb_light_png);
            } else {
                lv_obj_clear_state(ui_SwitchBulb, LV_STATE_CHECKED);
                _ui_image_set_property(ui_ImgBulb, _UI_IMAGE_PROPERTY_IMAGE, &ui_img_bulb_dark_png);
            }
            lv_port_sem_give();
            break;
        case VIEW_EVENT_LORAWAN_JOIN_ACTION: // shangdian
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_JOIN_ACTION");
            bool startup = *(bool *)event_data; // switch the start and stop
            lv_port_sem_take();
            if (startup) {
                lv_obj_add_state(ui_BtnLoRaWANStatus, LV_STATE_CHECKED);
                lv_label_set_text(ui_LblLoRaWANStatus, "STOP");
                lv_obj_clear_state(ui_toTerminal, LV_STATE_CHECKED);

                lv_obj_clear_state(ui_toTerminal, LV_STATE_CHECKED);
                lv_obj_add_flag(ui_toTerminal, LV_OBJ_FLAG_CLICKABLE);

                // lv_label_set_text(ui_LblLoRaWANStatus, "STOP");
                lv_obj_clear_flag(ui_SwClass, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(ui_DdFreq, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(ui_SwActivation, LV_OBJ_FLAG_CLICKABLE);

                lv_obj_clear_flag(ui_PnlNum, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(ui_NumInterval, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(ui_BtnMinus, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(ui_BtnPlus, LV_OBJ_FLAG_CLICKABLE);


                lv_obj_add_flag(ui_toTerminal, LV_OBJ_FLAG_CLICKABLE);

            } else {
                lv_obj_clear_state(ui_BtnLoRaWANStatus, LV_STATE_CHECKED);
                lv_obj_add_state(ui_toTerminal, LV_STATE_CHECKED);
                lv_obj_clear_flag(ui_toTerminal, LV_OBJ_FLAG_CLICKABLE);

                lv_label_set_text(ui_LblLoRaWANStatus, "START");
                lv_obj_add_flag(ui_SwClass, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(ui_DdFreq, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(ui_SwActivation, LV_OBJ_FLAG_CLICKABLE);

                lv_obj_add_flag(ui_PnlNum, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(ui_NumInterval, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(ui_BtnMinus, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(ui_BtnPlus, LV_OBJ_FLAG_CLICKABLE);

                lv_obj_clear_flag(ui_toTerminal, LV_OBJ_FLAG_CLICKABLE);
            }
            lv_port_sem_give();
            break;
        default:
            break;
    }
}

void fn_LoRaWANStatus_clicked(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t       *target     = lv_event_get_target(e);
    bool            startup    = false;
    ESP_LOGI(TAG, "bbb event_code: %d", event_code);
    struct view_data_lorawan_basic_cfg cfg = {0};

    if (lv_obj_has_state(target, LV_STATE_CHECKED)) { // Being STOP

        // get the uplink interval and freqency and class & activation
        if (lv_obj_get_state(ui_SwClass) == LV_STATE_CHECKED) {
            cfg.class = CLASS_C;
        } else {
            cfg.class = CLASS_A;
        }

        char str[32];
        lv_dropdown_get_selected_str(ui_DdFreq, str, sizeof(str));
        if (strcmp(str, "AS923") == 0) {
            cfg.region = LORAMAC_REGION_AS923;
        } else if (strcmp(str, "AU915") == 0) {
            cfg.region = LORAMAC_REGION_AU915;
        } else if (strcmp(str, "CN779") == 0) {
            cfg.region = LORAMAC_REGION_CN779;
        } else if (strcmp(str, "EU433") == 0) {
            cfg.region = LORAMAC_REGION_EU433;
        } else if (strcmp(str, "EU868") == 0) {
            cfg.region = LORAMAC_REGION_EU868;
        } else if (strcmp(str, "KR920") == 0) {
            cfg.region = LORAMAC_REGION_KR920;
        } else if (strcmp(str, "IN865") == 0) {
            cfg.region = LORAMAC_REGION_IN865;
        } else if (strcmp(str, "US915") == 0) {
            cfg.region = LORAMAC_REGION_US915;
        } else if (strcmp(str, "RU864") == 0) {
            cfg.region = LORAMAC_REGION_RU864;
        } else {
            ESP_LOGE(TAG, "Incorrect input parameters :%s", str);
            cfg.region = LORAMAC_REGION_US915;
        }

        if (lv_obj_get_state(ui_SwActivation) == LV_STATE_CHECKED) {
            cfg.IsOtaaActivation = false;
        } else {
            cfg.IsOtaaActivation = true;
        }

        cfg.uplink_interval_min = atoi(lv_textarea_get_text(ui_NumInterval));

        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_APPLY, (void *)&cfg, sizeof(cfg), portMAX_DELAY);
        startup = true;
    } else {
        startup = false; // now it's waiting for starting.
    }
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ACTION, (void *)&startup, sizeof(startup), portMAX_DELAY);
}


void view_lorawan_init(void)
{
    // refresh the lvgl

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ST,
                                                             __view_event_handler,
                                                             NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_UPDATE,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_APPLY,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LIGHT_ST,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ACTION,
                                                             __view_event_handler, NULL, NULL));
    // terminal log display
}
void bytesToHexString(char *dest, const uint8_t *src, size_t length)
{
    char *ptr = dest;
    for (size_t i = 0; i < length; ++i) {
        sprintf(ptr, "%02X", src[i]);
        ptr += 2;
        if (i < length - 1) {
            *ptr++ = ':'; // Add colon separator
        }
    }
    *ptr = '\0'; // Null-terminate the string
}
static void update_panel_network_info(struct view_data_lorawan_network_info *info)
{
    char str[50]; // Adjust size if needed

    lv_port_sem_take();

    bytesToHexString(str, info->eui, sizeof(info->eui));
    lv_label_set_text(ui_LblDataDevEUI1, str);
    lv_label_set_text(ui_LblDataDevEUI2, str); // Assuming you want to set the same EUI here

    bytesToHexString(str, info->join_eui, sizeof(info->join_eui));
    lv_label_set_text(ui_LblDataKeyJoinEUI, str);

    bytesToHexString(str, info->app_key, sizeof(info->app_key));
    lv_label_set_text(ui_LblDataAppkey, str);

    bytesToHexString(str, info->dev_addr, sizeof(info->dev_addr));
    lv_label_set_text(ui_LblDataDevAddr, str);

    bytesToHexString(str, info->apps_key, sizeof(info->apps_key));
    lv_label_set_text(ui_LblDataAppSKey, str);

    bytesToHexString(str, info->nwks_key, sizeof(info->nwks_key));
    lv_label_set_text(ui_LblDataNwkSKey, str);

    lv_port_sem_give();
}


static void update_panel_network_cfg(struct view_data_lorawan_basic_cfg *cfg)
{
    lv_port_sem_take();
    char str[11]; // uint32_t最大为4294967295，占用10位字符 + 1位用于字符串终结符'\0'

    if (cfg->class == CLASS_A) {
        lv_obj_clear_state(ui_SwClass, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(ui_SwClass, LV_STATE_CHECKED);
    }
    ESP_LOGI(TAG, "region: %d", cfg->region);

    if (cfg->region >= LORAMAC_REGION_CN470) {
        lv_dropdown_set_selected(ui_DdFreq, cfg->region - 1);
        ESP_LOGI(TAG, "select freq: %d", cfg->region - 1);
    } else {
        lv_dropdown_set_selected(ui_DdFreq, cfg->region);
        ESP_LOGI(TAG, "select freq: %d", cfg->region);
    }
    if (cfg->uplink_interval_min > 0) {
        sprintf(str, "%u", cfg->uplink_interval_min);
        lv_textarea_set_text(ui_NumInterval, str);
    }

    if (cfg->IsOtaaActivation) {
        lv_obj_clear_state(ui_SwActivation, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(ui_SwActivation, LV_STATE_CHECKED);
    }
    lv_port_sem_give();
}

static void update_lorawan_network_status(enum view_data_lorawan_join status)
{
    lv_img_dsc_t *p_img = &ui_img_lora_disconnect_png;
    switch (status) {
        case LORAWAN_JOIN_ST_OK:
            p_img = &ui_img_lora_connected_png;
            break;
        case LORAWAN_JOIN_ST_FAIL:
            p_img = &ui_img_lora_disconnect_png;
            break;
        case LORAWAN_JOIN_ST_PROCEED:
            p_img = &ui_img_lora_ing_png;
            break;
        default:
            break;
    }
    lv_port_sem_take();
    lv_img_set_src(ui_LblLoRaStatus1, p_img);
    lv_port_sem_give();
}
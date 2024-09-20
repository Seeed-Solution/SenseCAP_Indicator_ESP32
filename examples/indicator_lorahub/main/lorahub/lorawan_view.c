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
#include "indicator_lorahub.h"
#include "ui.h"
#include "view_pages.h"

#include "esp_log.h"

static const char *TAG = "lorawan_view";


static void __view_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    lv_port_sem_take();
    switch (event_id) {      
        case VIEW_EVENT_LORAHUB_DATA_UPDATE:{
            ESP_LOGI(TAG, "VIEW_EVENT_LORAHUB_DATA_UPDATE");
            struct lorahub *lorahub_st = (struct lorahub *)event_data;
            int datarate = atoi(lorahub_st->web_cfg_chan_datarate_str);
            int bandwidth = atoi(lorahub_st->web_cfg_chan_bandwidth_khz_str);

            lv_textarea_set_text(objects.adresstext, lorahub_st->web_cfg_lns_address);
            lv_textarea_set_text(objects.porttext, lorahub_st->web_cfg_lns_port_str);
            lv_textarea_set_text(objects.freqtext, lorahub_st->web_cfg_chan_freq_mhz_str);
            lv_dropdown_set_selected(objects.spreadfactordw, datarate - 7);
            if(bandwidth == 125)
            {
                lv_dropdown_set_selected(objects.bandwidth, 0);
            }else if(bandwidth == 250)
            {
                lv_dropdown_set_selected(objects.bandwidth, 1);
            }else{
                lv_dropdown_set_selected(objects.bandwidth, 2);
            }
            
            lv_textarea_set_text(objects.sntptext, lorahub_st->web_cfg_sntp_address);

            break;
        }

        case VIEW_EVENT_LORAHUB_MAC:{
            ESP_LOGI(TAG, "VIEW_EVENT_LORAHUB_MAC");
           char *lorahub_mac = (char *)event_data;

            lv_label_set_text(objects.mactext, lorahub_mac);

            break;
        }

        default:
            break;
    }
    lv_port_sem_give();
}



void view_lorawan_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_DATA_UPDATE,
                                                             __view_event_handler,
                                                             NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAHUB_MAC,
                                                             __view_event_handler,
                                                             NULL, NULL));
}
#ifndef INDICATOR_HA_H
#define INDICATOR_HA_H

#include "config.h"
#include "indicator_mqtt.h"
#include "view_data.h"

#define MQTT_HA_CFG_STORAGE "ha-mqtt"

#ifdef __cplusplus
extern "C" {
#endif

enum HA_CFG_EVENT {
    HA_CFG_SET,
    HA_CFG_EVENT_ALL,
};

typedef struct
{
    char broker_url[128];
    char client_id[16];
    char username[32];
    char password[64];
} ha_cfg_interface;

int  indicator_ha_init(void);
void ha_cfg_get(ha_cfg_interface *cfg);
void ha_cfg_set(ha_cfg_interface *cfg);

#ifdef __cplusplus
}
#endif

#endif

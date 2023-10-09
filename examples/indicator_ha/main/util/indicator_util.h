


#ifndef INDICATOR_UTIL_H
#define INDICATOR_UTIL_H

#include "config.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int wifi_rssi_level_get(int rssi);
bool isValidIP(const char *input);
bool isValidDomain(const char *input);

#ifdef __cplusplus
}
#endif

#endif

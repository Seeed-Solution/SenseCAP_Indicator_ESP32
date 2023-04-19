#ifndef TERMINAL_TIME_H
#define TERMINAL_TIME_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif


//ntp sync
int terminal_time_init(void);

// set TZ
int terminal_time_net_zone_set( char *p);

#ifdef __cplusplus
}
#endif

#endif

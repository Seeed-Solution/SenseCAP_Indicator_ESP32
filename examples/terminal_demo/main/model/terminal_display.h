#ifndef TERMINAL_DISPLAY_H
#define TERMINAL_DISPLAY_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif


int terminal_display_init(void);

bool terminal_display_st_get(void);

int terminal_display_on(void);

int terminal_display_off(void);

int terminal_display_sleep_restart(void);

#ifdef __cplusplus
}
#endif

#endif

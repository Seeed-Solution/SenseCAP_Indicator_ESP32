/**
 * @file indicator_terminal.h
 * @date  14 December 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef INDICATOR_TERMINAL_H
#define INDICATOR_TERMINAL_H
#include "ui.h"
#ifdef __cplusplus
extern "C" {
#endif

#define Panel_Container ui_PnlConsole

typedef struct indicator_terminal
{
    lv_obj_t *container; // Panel
    bool auto_scroll;
    bool scrollable;
    bool user_scrolling;
    uint32_t last_click_time;

    void (*addLogLine)(struct indicator_terminal *, const char *logLine, lv_color_t color);
    void (*clearLog)(struct indicator_terminal *);
    void (*scrollToBottom)(struct indicator_terminal *);
    void (*scrollDisable)(struct indicator_terminal *);
    void (*scrollEnable)(struct indicator_terminal *);

} indicator_terminal_t, *indicator_terminal_p;

void indicator_terminal_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*INDICATOR_TERMINAL_H*/
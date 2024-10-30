#ifndef VIEW_PAGES_H
#define VIEW_PAGES_H

#include <lvgl/lvgl.h>
#include "ui/ui.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _objects_t {
    lv_obj_t *page_single_channel_gw;
    lv_obj_t *lorahubcontainer;
    lv_obj_t *infopanel;
    lv_obj_t *mactext;
    lv_obj_t *euitext;
    lv_obj_t *networkserverpanel;
    lv_obj_t *adresstext;
    lv_obj_t *porttext;
    lv_obj_t *rxchannelpanel;
    lv_obj_t *freqtext;
    lv_obj_t *spreadfactordw;
    lv_obj_t *bandwidth;
    lv_obj_t *miscellaneouspanel;
    lv_obj_t *sntptext;
    lv_obj_t *buttonpanel;
    lv_obj_t *btnreboot;
    lv_obj_t *btnconfigure;
    lv_obj_t *lorakb;
} objects_t;

extern objects_t objects;

void view_pages_init();



void ui_sing_channel_gw_init();




#ifdef __cplusplus
}
#endif

#endif /*VIEW_PAGES_H*/
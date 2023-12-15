#ifndef INDICATOR_WIFI_H
#define INDICATOR_WIFI_H


#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*model*/
int indicator_wifi_init(void);

/*View*/
void wifi_list_event_init(void);

/**For View Event**/
void view_event_wifi_st(void *event_data);
void view_event_wifi_list(void *event_data);
void view_event_wifi_connet_ret(void *event_data);

#ifdef __cplusplus
}
#endif

#endif

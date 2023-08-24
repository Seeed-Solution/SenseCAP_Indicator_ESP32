#ifndef INDICATOR_VIRTUAL_DASHBOARD_H
#define INDICATOR_VIRTUAL_DASHBOARD_H

#include "config.h"
#include "view_data.h"


#ifdef __cplusplus
extern "C" {
#endif

struct virtual_dashboard
{
    int  arc_value;
    bool switch_state;
    int  slider_value;
    bool button1;
    bool button2;
};

int indicator_virtual_dashboard_init(void);
void dashboard_data_get(struct virtual_dashboard *dashboard_data );

#ifdef __cplusplus
}
#endif

#endif

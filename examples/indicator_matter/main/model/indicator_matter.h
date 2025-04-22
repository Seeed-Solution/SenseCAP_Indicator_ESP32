#ifndef INDICATOR_MATTER_H
#define INDICATOR_MATTER_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STANDARD_BRIGHTNESS 100
#define MATTER_BRIGHTNESS 254

int indicator_matter_setup(void);
int indicator_matter_init(void);

#ifdef __cplusplus
}
#endif

#endif

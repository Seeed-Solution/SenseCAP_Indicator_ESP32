#ifndef indicator_lorahub_H
#define indicator_lorahub_H

#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LRHB_ERROR_NONE,
    LRHB_ERROR_WIFI,
    LRHB_ERROR_LNS,
    LRHB_ERROR_OS,
    LRHB_ERROR_HAL,
    LRHB_ERROR_UNKNOWN,
} lorahub_error_t;

void wait_on_error( lorahub_error_t error, int line );

#ifdef __cplusplus
}
#endif

#endif

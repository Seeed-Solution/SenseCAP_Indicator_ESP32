#ifndef indicator_OPENAI_H
#define indicator_OPENAI_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPENAI_API_KEY_STORAGE  "openai_api_key"   //52 bytes

int indicator_openai_init(void);

#ifdef __cplusplus
}
#endif

#endif

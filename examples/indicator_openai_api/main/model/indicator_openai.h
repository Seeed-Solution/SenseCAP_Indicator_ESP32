#ifndef indicator_OPENAI_H
#define indicator_OPENAI_H

#include "config.h"
#include "view_data.h"

#ifdef __cplusplus
extern "C" {
#endif

// Constants
#define MAX_DATA_SIZE 2048
#define MAX_API_KEY_LENGTH 164+15+1
#define MAX_API_URL_LENGTH 64+1
#define MAX_MODEL_NAME_LENGTH 50+1
#define MAX_SYSTEM_PROMPT_LENGTH 256+1
#define DEFAULT_TIMEOUT_MS 60000

#define API_KEY_STORAGE     "api_key"         // API KEY, max 164 bytes
#define API_URL_STORAGE     "api_url"         // API URL, max 100 bytes
#define MODEL_NAME_STORAGE  "model_name"      // Model Name, max 50 bytes
#define SYSTEM_PROMPT_STORAGE "system_prompt" // System Prompt, max 512 bytes

int indicator_openai_init(void);

#ifdef __cplusplus
}
#endif

#endif

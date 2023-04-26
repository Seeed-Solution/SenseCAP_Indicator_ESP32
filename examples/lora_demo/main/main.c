#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"

extern void demo_lora_cw(void);
extern void demo_lora_rx(void);
extern void demo_lora_tx(void);

void app_main(void)
{
    //demo_lora_cw();
    //demo_lora_rx();
    demo_lora_tx();
    while(1){}
}

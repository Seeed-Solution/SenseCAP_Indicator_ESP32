#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void demo_lora_cw( void );
extern void demo_lora_rx( void );
extern void demo_lora_tx( void );
extern void func_pingpong( void );
extern void func_beep( void );

#define USING_LORA_BEEP

void app_main( void ) {
#if defined( USING_LORA_CW )
    demo_lora_cw();  // Tx lora Continuous Wave.
#elif defined( USING_LORA_RX )
    demo_lora_rx();  // Rx lora data.
#elif defined( USING_LORA_TX )
    demo_lora_tx();  // Tx lora data.
#elif defined( USING_LORA_PINGPONG )
    func_pingpong();
#elif defined( USING_LORA_BEEP )
    func_beep();
#else
#error "Please define one marco to run an example."
#endif

    for ( ;; )
        ;
}

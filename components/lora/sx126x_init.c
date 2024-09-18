#include "sx126x_init.h"
#include "esp_log.h"

const char * TAG = "sx126x_io_init";


io_expander_ops_t *indicator_io_expander = NULL;
static bool init_flag = false;

static void sx126x_io_init( void )
{
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_NSS, 1); //output
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_RST, 1); //output
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_BUSY, 0); //input
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_DIO_1, 0); //input
}

void sx126x_init(void)
{
    if(init_flag){
        return;
    }
    init_flag = true;

    ESP_LOGI(TAG, "bsp sx126x init");
    
    board_res_desc_t *brd = bsp_board_get_description();

    indicator_io_expander = brd->io_expander_ops;


    sx126x_io_init();
}
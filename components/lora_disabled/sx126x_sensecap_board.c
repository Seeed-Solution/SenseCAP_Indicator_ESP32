#include <stdlib.h>
#include "bsp_sx126x.h"
#include "radio.h"
#include "sx126x-board.h"
#include "driver/i2c.h"
#include <string.h>

#include "bsp_i2c.h"
#include "timer.h"
//#define SX126X_SPI_DBUG 

#define HOST_ID                 SPI3_HOST

#define GPIO_QUEUE_STACK  (10240)

static const int SPI_Frequency = 2000000;
static spi_device_handle_t SpiHandle;
static SemaphoreHandle_t   radio_mutex;


io_expander_ops_t *indicator_io_expander = NULL;
static QueueHandle_t gpio_evt_queue = NULL;
static DioIrqHandler * g_dioIrq;

static const char *TAG = "sx126x";

static RadioOperatingModes_t OperatingMode;
static bool init_flag = false;

TimerTime_t g_lora_irq_time = 0;
bool  g_have_tcxo = false;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    TimerTime_t irq_time = TimerGetCurrentTime();
    xQueueSendFromISR(gpio_evt_queue, &irq_time, NULL);
}

uint32_t SX126xGetDio1PinState( void );

static void expander_io_int(void* arg)
{
    TimerTime_t irq_time;
    RadioOperatingModes_t  mode;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &irq_time, portMAX_DELAY)) {
            g_lora_irq_time = irq_time;
            // printf("lora irq:%d\n", g_lora_irq_time);

            if( SX126xGetDio1PinState()) { // Only handle DIO1 interrupt
                xSemaphoreTake(radio_mutex, portMAX_DELAY);
                mode = SX126xGetOperatingMode();
                xSemaphoreGive(radio_mutex);
                if( mode !=  MODE_SLEEP ) { // Filter out sleep mode interrupts
                    (g_dioIrq) (0); //handle irq
                }
            }
        }
    }
}

static bool spi_write_byte(uint8_t* data_in, size_t DataLength )
{
#ifdef SX126X_SPI_DBUG
    printf("spi write: ");
    for(int i =0; i < DataLength ; i++) {
        printf("%x ",data_in[i]);
    }
    printf("\r\n");
#endif

	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = data_in;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( SpiHandle, &SPITransaction );
	}

	return true;
}

static bool spi_read_byte(uint8_t* data_out, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = NULL;
		SPITransaction.rx_buffer = data_out;
		spi_device_transmit( SpiHandle, &SPITransaction );
	}

#ifdef SX126X_SPI_DBUG
    printf("spi read: ");
    for(int i =0; i < DataLength ; i++) {
        printf("%x ",data_out[i]);
    }
    printf("\r\n");
#endif
	return true;
}

static uint8_t spi_transfer(uint8_t* data_in, uint8_t* data_out, size_t DataLength)
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = data_in;
		SPITransaction.rx_buffer = data_out;
		spi_device_transmit( SpiHandle, &SPITransaction );
	}

#ifdef SX126X_SPI_DBUG
    printf("spi write: ");
    for(int i =0; i < DataLength ; i++) {
        printf("%x ",data_in[i]);
    }
    printf(",read: ");
    for(int i =0; i < DataLength ; i++) {
        printf("%x ",data_out[i]);
    }
    printf("\r\n");
#endif
	return true;
}

void SX126xIoInit( void )
{
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_NSS, 1); //output
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_RST, 1); //output
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_BUSY, 0); //input
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_DIO_1, 0); //input
    indicator_io_expander->set_direction(EXPANDER_IO_RADIO_VER, 0); //input

    uint16_t pin_val;
    uint8_t cnt = 5;
    uint8_t hight_cnt = 0;
    for (size_t i = 0; i < cnt; i++)
    {
        pin_val = 0;
        esp_err_t ret = indicator_io_expander->read_input_pins((uint8_t *)&pin_val);
        if( (pin_val & (0x01 << EXPANDER_IO_RADIO_VER)) ) {
            hight_cnt++;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    if( hight_cnt >= 3) {
        g_have_tcxo = true;
    } else {
        g_have_tcxo = false;
    }
    printf("TCXO:%d,VOLTAGE:%d\r\n", g_have_tcxo, SX126X_TCXO_CTRL_VOLTAGE);
}



void bsp_sx126x_init(void)
{
    if(init_flag){
        return;
    }
    init_flag = true;

    ESP_LOGI(TAG, "bsp sx126x init");
    
    board_res_desc_t *brd = bsp_board_get_description();

    indicator_io_expander = brd->io_expander_ops;

    radio_mutex =xSemaphoreCreateMutex();


    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num = brd->GPIO_SPI_MISO,
        .mosi_io_num = brd->GPIO_SPI_MOSI,
        .sclk_io_num = brd->GPIO_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    //Initialize the SPI bus
    ret = spi_bus_initialize(HOST_ID, &buscfg, SPI_DMA_CH_AUTO);
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset( &devcfg, 0, sizeof( spi_device_interface_config_t ) );
	devcfg.clock_speed_hz = SPI_Frequency;
	// It does not work with hardware CS control.
	//devcfg.spics_io_num = SX126x_SPI_SELECT;
	// It does work with software CS control.
	devcfg.spics_io_num = -1;
	devcfg.queue_size = 7;
	devcfg.mode = 0;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

	//spi_device_handle_t handle;
	ret = spi_bus_add_device( HOST_ID, &devcfg, &SpiHandle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);

    SX126xIoInit();
}

spi_device_handle_t bsp_sx126x_spi_handle_get(void)
{
    return SpiHandle;
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    g_dioIrq = dioIrq;

    static bool Ioirq_init_flag = false;
    if( Ioirq_init_flag ){
        return;
    }
    Ioirq_init_flag = true;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; //falling edge
    io_conf.pin_bit_mask = (1ULL << ESP32_EXPANDER_IO_INT);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(10, sizeof(TimerTime_t));
    xTaskCreate(expander_io_int, "expander_io_int", GPIO_QUEUE_STACK, NULL, 10, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(ESP32_EXPANDER_IO_INT, gpio_isr_handler, (void*)ESP32_EXPANDER_IO_INT);
}

void SX126xIoDeInit( void )
{
}

void SX126xIoDbgInit( void )
{
}

void SX126xIoTcxoInit( void )
{
    if( g_have_tcxo ) {
        SX126xSetDio3AsTcxoCtrl(SX126X_TCXO_CTRL_VOLTAGE,  SX126xGetBoardTcxoWakeupTime()<<6);
    }

    CalibrationParams_t calibParam;
    calibParam.Value = 0x7F;
    SX126xCalibrate( calibParam );
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
#define BOARD_TCXO_WAKEUP_TIME                      5
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
}

RadioOperatingModes_t SX126xGetOperatingMode( void )
{
    return OperatingMode;
}

void SX126xSetOperatingMode( RadioOperatingModes_t mode )
{
    OperatingMode = mode;
}

void SX126xReset( void )
{
    vTaskDelay(10 / portTICK_PERIOD_MS);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_RST, 0);
    vTaskDelay(30 / portTICK_PERIOD_MS);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_RST, 1);
    vTaskDelay(20 / portTICK_PERIOD_MS);
}

void SX126xWaitOnBusy( void )
{
    uint16_t pin_val;
    while(1) {
        esp_err_t ret = indicator_io_expander->read_input_pins((uint8_t *)&pin_val);
        if( ret == ESP_OK ) {
            if( !(pin_val & (0x01 << EXPANDER_IO_RADIO_BUSY)) ) {
                return;
            }
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

void SX126xWakeup( void )
{
    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    uint8_t tx_buf[2];
    tx_buf[0] = RADIO_GET_STATUS;
    tx_buf[1] = 0x00;
    spi_write_byte(( uint8_t *)tx_buf, sizeof(tx_buf));
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    SX126xWaitOnBusy( );
    SX126xSetOperatingMode( MODE_STDBY_RC );

}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    if(  command == RADIO_SET_SLEEP) {
        // Update mode in advance to prevent interrupts from being triggered when the device is sleeping
        SX126xSetOperatingMode( MODE_SLEEP ); 
    }
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    spi_write_byte(( uint8_t *)&command, 1);
    spi_write_byte( buffer, size);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t status = 0;

    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    spi_write_byte(( uint8_t *)&command, 1);
    uint8_t data = 0x00;
    spi_transfer(&data, &status, 1);
    spi_read_byte(buffer, size);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    SX126xWaitOnBusy( );

    return status;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    uint8_t tx_buf[3];
    tx_buf[0] = RADIO_WRITE_REGISTER;
    tx_buf[1] = ( address & 0xFF00 ) >> 8;
    tx_buf[2] = address & 0x00FF;
    spi_write_byte(( uint8_t *)tx_buf, 3);
    spi_write_byte( buffer, size);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    uint8_t tx_buf[4];
    tx_buf[0] = RADIO_READ_REGISTER;
    tx_buf[1] = ( address & 0xFF00 ) >> 8;
    tx_buf[2] = address & 0x00FF;
    tx_buf[3] = 0;
    spi_write_byte(( uint8_t *)tx_buf, 4);
    spi_read_byte(buffer, size);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    uint8_t tx_buf[2];
    tx_buf[0] = RADIO_WRITE_BUFFER;
    tx_buf[1] = offset;
    spi_write_byte(( uint8_t *)tx_buf, sizeof(tx_buf));
    spi_write_byte( buffer, size);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );

    xSemaphoreTake(radio_mutex, portMAX_DELAY);
    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 0);
    uint8_t tx_buf[3];
    tx_buf[0] = RADIO_READ_BUFFER;
    tx_buf[1] = offset;
    tx_buf[2] = 0;
    spi_write_byte(( uint8_t *)tx_buf, sizeof(tx_buf));
    spi_read_byte(buffer, size);

    indicator_io_expander->set_level(EXPANDER_IO_RADIO_NSS, 1);
    xSemaphoreGive(radio_mutex);
    
    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetDeviceId( void )
{
    return SX1262;
}

void SX126xAntSwOn( void )
{
}

void SX126xAntSwOff( void )
{
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

uint32_t SX126xGetDio1PinState( void )
{
    uint16_t pin_val;
    esp_err_t ret = indicator_io_expander->read_input_pins((uint8_t *)&pin_val);
    if( ret == ESP_OK ) {
        if( (pin_val & (0x01 << EXPANDER_IO_RADIO_DIO_1)) ) {
            return 1;
        }
    }
    return 0;
}


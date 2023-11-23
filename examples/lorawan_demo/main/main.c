#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "bsp_board.h"

/* LoRaMac */
#include "utilities.h"
#include "RegionCommon.h"
#include "Commissioning.h"
#include "LmHandler.h"
#include "LmhpCompliance.h"
#include "LmHandlerMsgDisplay.h"
#include "githubVersion.h"
#include "timer.h"

#define FIRMWARE_VERSION                              0x01000000 // 1.0.0.0

#ifndef ACTIVE_REGION

#warning "No active region defined, LORAMAC_REGION_EU868 will be used as default."

#define ACTIVE_REGION LORAMAC_REGION_EU868

#endif


/*!
 * LoRaWAN default end-device class
 */
#ifndef LORAWAN_DEFAULT_CLASS
#define LORAWAN_DEFAULT_CLASS                       CLASS_A
#endif

/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            30000

/*!
 * Defines a random delay for application data transmission duty cycle. 1s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND                        1000

/*!
 * LoRaWAN Adaptive Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_STATE                           LORAMAC_HANDLER_ADR_ON

/*!
 * Default datarate
 *
 * \remark Please note that LORAWAN_DEFAULT_DATARATE is used only when ADR is disabled 
 */
#define LORAWAN_DEFAULT_DATARATE                    DR_0

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_DEFAULT_CONFIRMED_MSG_STATE         LORAMAC_HANDLER_UNCONFIRMED_MSG

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        true

/*!
 * LoRaWAN application port
 * @remark The allowed port range is from 1 up to 223. Other values are reserved.
 */
#define LORAWAN_APP_PORT                            2

/*!
 * LoRaWAN Deivce EUI
 */
static uint8_t device_eui[8] = { 0x6E, 0x92, 0xD1, 0x39, 0x1E, 0x80, 0xC4, 0x4B } ;

/*!
 * LoRaWAN Deivce APP KEY
 */
static uint8_t app_key[16] = { 0x2C, 0xBB, 0x9C, 0x1B, 0xB7, 0x94, 0xE3, 0xCC,  \
                              0x36, 0x92, 0x00, 0xFD, 0x84, 0xD6, 0xF8, 0x7F } ;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_TX_ON_TIMER,
    LORAMAC_HANDLER_TX_ON_EVENT,
}LmHandlerTxEvents_t;

/*!
 * User application data
 */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/*!
 * User application data structure
 */
static LmHandlerAppData_t AppData =
{
    .Buffer = AppDataBuffer,
    .BufferSize = 0,
    .Port = 0,
};

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t TxTimer;

static void OnMacProcessNotify( void );
static void OnNvmDataChange( LmHandlerNvmContextStates_t state, uint16_t size );
static void OnNetworkParametersChange( CommissioningParams_t* params );
static void OnMacMcpsRequest( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxIn );
static void OnMacMlmeRequest( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxIn );
static void OnJoinRequest( LmHandlerJoinParams_t* params );
static void OnTxData( LmHandlerTxParams_t* params );
static void OnRxData( LmHandlerAppData_t* appData, LmHandlerRxParams_t* params );
static void OnClassChange( DeviceClass_t deviceClass );
static void OnBeaconStatusChange( LoRaMacHandlerBeaconParams_t* params );
#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
static void OnSysTimeUpdate( bool isSynchronized, int32_t timeCorrection );
#else
static void OnSysTimeUpdate( void );
#endif
static void PrepareTxFrame( void );
static void StartTxProcess( LmHandlerTxEvents_t txEvent );
static void UplinkProcess( void );

static void OnTxPeriodicityChanged( uint32_t periodicity );
static void OnTxFrameCtrlChanged( LmHandlerMsgTypes_t isTxConfirmed );
static void OnPingSlotPeriodicityChanged( uint8_t pingSlotPeriodicity );

/*!
 * Function executed on TxTimer event
 */
static void OnTxTimerEvent( void* context );

static LmHandlerCallbacks_t LmHandlerCallbacks =
{
    .GetBatteryLevel = NULL,
    .GetTemperature = NULL,
    .GetRandomSeed = NULL,
    .OnMacProcess = OnMacProcessNotify,
    .OnNvmDataChange = OnNvmDataChange,
    .OnNetworkParametersChange = OnNetworkParametersChange,
    .OnMacMcpsRequest = OnMacMcpsRequest,
    .OnMacMlmeRequest = OnMacMlmeRequest,
    .OnJoinRequest = OnJoinRequest,
    .OnTxData = OnTxData,
    .OnRxData = OnRxData,
    .OnClassChange= OnClassChange,
    .OnBeaconStatusChange = OnBeaconStatusChange,
    .OnSysTimeUpdate = OnSysTimeUpdate,
};

static LmHandlerParams_t LmHandlerParams =
{
    .Region = ACTIVE_REGION,
    .AdrEnable = LORAWAN_ADR_STATE,
    .IsTxConfirmed = LORAWAN_DEFAULT_CONFIRMED_MSG_STATE,
    .TxDatarate = LORAWAN_DEFAULT_DATARATE,
    .PublicNetworkEnable = LORAWAN_PUBLIC_NETWORK,
    .DutyCycleEnabled = LORAWAN_DUTYCYCLE_ON,
    .DataBufferMaxSize = LORAWAN_APP_DATA_BUFFER_MAX_SIZE,
    .DataBuffer = AppDataBuffer,
    .PingSlotPeriodicity = REGION_COMMON_DEFAULT_PING_SLOT_PERIODICITY,
};

static LmhpComplianceParams_t LmhpComplianceParams =
{
    .FwVersion.Value = FIRMWARE_VERSION,
    .OnTxPeriodicityChanged = OnTxPeriodicityChanged,
    .OnTxFrameCtrlChanged = OnTxFrameCtrlChanged,
    .OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged,
};

/*!
 * Indicates if LoRaMacProcess call is pending.
 * 
 * \warning If variable is equal to 0 then the MCU can be set in low power mode
 */
static volatile uint8_t IsMacProcessPending = 0;

static volatile uint8_t IsTxFramePending = 0;

static volatile uint32_t TxPeriodicity = 0;

/*!
 * Main application entry point.
 */
void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());

    nvm_init();
    
    //NvmDataMgmtFactoryReset(); // If you modify the built-in region, you must perform a restore of nvs

    // Initialize transmission periodicity variable
    TxPeriodicity = APP_TX_DUTYCYCLE + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );

    const Version_t appVersion = { .Value = FIRMWARE_VERSION };
    const Version_t gitHubVersion = { .Value = GITHUB_VERSION };
    DisplayAppInfo( "periodic-uplink-lpp", 
                    &appVersion,
                    &gitHubVersion );


    if ( LmHandlerInit( &LmHandlerCallbacks, &LmHandlerParams ) != LORAMAC_HANDLER_SUCCESS )
    {
        printf( "LoRaMac wasn't properly initialized\n" );
        // Fatal error, endless loop.
        while ( 1 )
        {
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }
    
    printf( "Set Device EUI & APP KEY\n" );
    printf( "DevEui :" );
    for( int i =0; i < 8; i++ ) {
        printf("%02X", device_eui[i]);
    }
    printf("\r\n");
    printf( "AppKEY :" );
    for( int i =0; i < 16; i++ ) {
        printf("%02X",app_key[i]);
    }
    printf("\r\n");

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_DEV_EUI;
    mibReq.Param.DevEui = device_eui;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_APP_KEY;
    mibReq.Param.AppKey = app_key;
    LoRaMacMibSetRequestConfirm( &mibReq );


    mibReq.Type = MIB_NWK_KEY;
    mibReq.Param.NwkKey = app_key;
    LoRaMacMibSetRequestConfirm( &mibReq );

    printf( "LoRaMac was initialized\n" );
    
    // Set system maximum tolerated rx error in milliseconds
    LmHandlerSetSystemMaxRxError( 20 );

    // The LoRa-Alliance Compliance protocol package should always be
    // initialized and activated.
    LmHandlerPackageRegister( PACKAGE_ID_COMPLIANCE, &LmhpComplianceParams );

    LmHandlerJoin( );

    StartTxProcess( LORAMAC_HANDLER_TX_ON_TIMER );

    while( 1 )
    {
        // Processes the LoRaMac events
        LmHandlerProcess( );

        // Process application uplinks management
        UplinkProcess( );

        if( IsMacProcessPending == 1 )
        {
            // Clear flag and prevent MCU to go into low power modes.
            IsMacProcessPending = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void OnMacProcessNotify( void )
{
    IsMacProcessPending = 1;
}

static void OnNvmDataChange( LmHandlerNvmContextStates_t state, uint16_t size )
{
    DisplayNvmDataChange( state, size );
}

static void OnNetworkParametersChange( CommissioningParams_t* params )
{
    DisplayNetworkParametersUpdate( params );
}

static void OnMacMcpsRequest( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxIn )
{
    DisplayMacMcpsRequestUpdate( status, mcpsReq, nextTxIn );
}

static void OnMacMlmeRequest( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxIn )
{
    DisplayMacMlmeRequestUpdate( status, mlmeReq, nextTxIn );
}

static void OnJoinRequest( LmHandlerJoinParams_t* params )
{
    DisplayJoinRequestUpdate( params );
    if( params->Status == LORAMAC_HANDLER_ERROR )
    {
        LmHandlerJoin( );
    }
    else
    {
        LmHandlerRequestClass( LORAWAN_DEFAULT_CLASS );
    }
}

static void OnTxData( LmHandlerTxParams_t* params )
{
    DisplayTxUpdate( params );
}

static void OnRxData( LmHandlerAppData_t* appData, LmHandlerRxParams_t* params )
{
    DisplayRxUpdate( appData, params );
}

static void OnClassChange( DeviceClass_t deviceClass )
{
    DisplayClassUpdate( deviceClass );

    // Inform the server as soon as possible that the end-device has switched to ClassB
    LmHandlerAppData_t appData =
    {
        .Buffer = NULL,
        .BufferSize = 0,
        .Port = 0,
    };
    LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG );
}

static void OnBeaconStatusChange( LoRaMacHandlerBeaconParams_t* params )
{

    DisplayBeaconUpdate( params );
}

#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
static void OnSysTimeUpdate( bool isSynchronized, int32_t timeCorrection )
{

}
#else
static void OnSysTimeUpdate( void )
{

}
#endif

/*!
 * Prepares the payload of the frame and transmits it.
 */
static void PrepareTxFrame( void )
{
    if( LmHandlerIsBusy( ) == true )
    {
        return;
    }

    static uint32_t cnt = 0;
    uint8_t channel = 0;

    cnt++;
    AppData.Buffer[0] = cnt & 0xff;
    AppData.Buffer[1] = (cnt>>8) & 0xff;
    AppData.Buffer[2] = (cnt>>16) & 0xff;
    AppData.Buffer[3] = (cnt>>24) & 0xff;
    AppData.BufferSize = 4;

    AppData.Port = LORAWAN_APP_PORT;

    if( LmHandlerSend( &AppData, LmHandlerParams.IsTxConfirmed ) == LORAMAC_HANDLER_SUCCESS )
    {
        printf("send success!\r\n");
    }
}

static void StartTxProcess( LmHandlerTxEvents_t txEvent )
{
    switch( txEvent )
    {
    default:
        // Intentional fall through
    case LORAMAC_HANDLER_TX_ON_TIMER:
        {
            // Schedule 1st packet transmission
            TimerInit( &TxTimer, OnTxTimerEvent );
            TimerSetValue( &TxTimer, TxPeriodicity );
            OnTxTimerEvent( NULL );
        }
        break;
    case LORAMAC_HANDLER_TX_ON_EVENT:
        {
        }
        break;
    }
}

static void UplinkProcess( void )
{
    uint8_t isPending = 0;
    CRITICAL_SECTION_BEGIN( );
    isPending = IsTxFramePending;
    IsTxFramePending = 0;
    CRITICAL_SECTION_END( );
    if( isPending == 1 )
    {
        PrepareTxFrame( );
    }
}

static void OnTxPeriodicityChanged( uint32_t periodicity )
{
    TxPeriodicity = periodicity;

    if( TxPeriodicity == 0 )
    { // Revert to application default periodicity
        TxPeriodicity = APP_TX_DUTYCYCLE + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
    }

    // Update timer periodicity
    TimerStop( &TxTimer );
    TimerSetValue( &TxTimer, TxPeriodicity );
    TimerStart( &TxTimer );
}

static void OnTxFrameCtrlChanged( LmHandlerMsgTypes_t isTxConfirmed )
{
    LmHandlerParams.IsTxConfirmed = isTxConfirmed;
}

static void OnPingSlotPeriodicityChanged( uint8_t pingSlotPeriodicity )
{
    LmHandlerParams.PingSlotPeriodicity = pingSlotPeriodicity;
}

/*!
 * Function executed on TxTimer event
 */
static void OnTxTimerEvent( void* context )
{
    TimerStop( &TxTimer );

    IsTxFramePending = 1;

    // Schedule next transmission
    TimerSetValue( &TxTimer, TxPeriodicity );
    TimerStart( &TxTimer );
}

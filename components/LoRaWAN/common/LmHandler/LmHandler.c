/*!
 * \file      LmHandler.c
 *
 * \brief     Implements the LoRaMac layer handling. 
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "utilities.h"
#include "timer.h"
#include "Commissioning.h"
#include "NvmDataMgmt.h"
#include "radio.h"
#include "LmHandler.h"
#include "LmhPackage.h"
#include "LmhpCompliance.h"
#include "LmhpClockSync.h"
#include "LmhpRemoteMcastSetup.h"
#include "LmhpFragmentation.h"

#include "LoRaMacTest.h"

static CommissioningParams_t CommissioningParams =
{
    .IsOtaaActivation = OVER_THE_AIR_ACTIVATION,
    .DevEui = { 0 },  // Automatically filed from secure-element
    .JoinEui = { 0 }, // Automatically filed from secure-element
    .SePin = { 0 },   // Automatically filed from secure-element
    .NetworkId = LORAWAN_NETWORK_ID,
    .DevAddr = LORAWAN_DEVICE_ADDRESS,
};

static LmhPackage_t *LmHandlerPackages[PKG_MAX_NUMBER];

/*!
 * Upper layer LoRaMac parameters
 */
static LmHandlerParams_t *LmHandlerParams;

/*!
 * Upper layer callbacks
 */
static LmHandlerCallbacks_t *LmHandlerCallbacks;

/*!
 * Used to notify LmHandler of LoRaMac events
 */
static LoRaMacPrimitives_t LoRaMacPrimitives;

/*!
 * LoRaMac callbacks
 */
static LoRaMacCallback_t LoRaMacCallbacks;

static LmHandlerJoinParams_t JoinParams =
{
    .CommissioningParams = &CommissioningParams,
    .Datarate = DR_0,
    .Status = LORAMAC_HANDLER_ERROR
};

static LmHandlerTxParams_t TxParams =
{
    .CommissioningParams = &CommissioningParams,
    .MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG,
    .AckReceived = 0,
    .Datarate = DR_0,
    .UplinkCounter = 0,
    .AppData =
    {
        .Port = 0,
        .BufferSize = 0,
        .Buffer = NULL,
    },
    .TxPower = TX_POWER_0,
    .Channel = 0,
};

static LmHandlerRxParams_t RxParams =
{
    .CommissioningParams = &CommissioningParams,
    .Rssi = 0,
    .Snr = 0,
    .DownlinkCounter = 0,
    .RxSlot = -1,
};

static LoRaMacHandlerBeaconParams_t BeaconParams =
{
    .State = LORAMAC_HANDLER_BEACON_ACQUIRING,
    .Info =
    {
        .Time = { .Seconds = 0, .SubSeconds = 0 },
        .Frequency = 0,
        .Datarate = 0,
        .Rssi = 0,
        .Snr = 0,
        .GwSpecific =
        {
            .InfoDesc = 0,
            .Info = { 0 },
        },
    },
};

/*!
 * Indicates if a switch to Class B operation is pending or not.
 *
 * TODO: Create a new structure to store the current handler states/status
 *       and add the below variable to it.
 */
static bool IsClassBSwitchPending = false;

/*!
 * Stores the time to wait before next transmission
 *
 * TODO: Create a new structure to store the current handler states/status
 *       and add the below variable to it.
 */
static TimerTime_t DutyCycleWaitTime = 0;

/*!
 * Indicates if an uplink is pending upon MAC layer request
 * 
 * TODO: Create a new structure to store the current handler states/status
 *       and add the below variable to it.
 */
static bool IsUplinkTxPending = false;

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *                             containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm );

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication );

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication );

/*!
 * Calls the OnClassChange callback to indicate the new active device class
 *
 * \param [in] deviceClass Active device class
 */
static void OnClassChangeNotify( DeviceClass_t deviceClass )
{
    if( LmHandlerCallbacks->OnClassChange != NULL )
    {
        LmHandlerCallbacks->OnClassChange( deviceClass );
    }
}

/*!
 * Calls the OnBeaconStatusChange callback to indicate current beacon status
 *
 * \param [in] params Current beacon parameters
 */
static void OnBeaconStatusChangeNotify( LoRaMacHandlerBeaconParams_t *params )
{
    if( LmHandlerCallbacks->OnBeaconStatusChange != NULL )
    {
        LmHandlerCallbacks->OnBeaconStatusChange( params );
    }
}

/*!
 * Starts the beacon search
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
static LmHandlerErrorStatus_t LmHandlerBeaconReq( void );

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */
typedef enum PackageNotifyTypes_e
{
    PACKAGE_MCPS_CONFIRM,
    PACKAGE_MCPS_INDICATION,
    PACKAGE_MLME_CONFIRM,
    PACKAGE_MLME_INDICATION,
}PackageNotifyTypes_t;

/*!
 * Notifies the package to process the LoRaMac callbacks.
 *
 * \param [IN] notifyType MAC notification type [PACKAGE_MCPS_CONFIRM,
 *                                               PACKAGE_MCPS_INDICATION,
 *                                               PACKAGE_MLME_CONFIRM,
 *                                               PACKAGE_MLME_INDICATION]
 * \param[IN] params      Notification parameters. The params type can be
 *                        [McpsConfirm_t, McpsIndication_t, MlmeConfirm_t, MlmeIndication_t]
 */
static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params );

static bool LmHandlerPackageIsTxPending( void );

static void LmHandlerPackagesProcess( void );

LmHandlerErrorStatus_t LmHandlerInit( LmHandlerCallbacks_t *handlerCallbacks,
                                      LmHandlerParams_t *handlerParams )
{
    //
    uint16_t nbNvmData = 0;
    MibRequestConfirm_t mibReq;
    LmHandlerParams = handlerParams;
    LmHandlerCallbacks = handlerCallbacks;

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
    LoRaMacCallbacks.GetBatteryLevel = LmHandlerCallbacks->GetBatteryLevel;
    LoRaMacCallbacks.GetTemperatureLevel = LmHandlerCallbacks->GetTemperature;
    LoRaMacCallbacks.NvmDataChange  = NvmDataMgmtEvent;
    LoRaMacCallbacks.MacProcessNotify = LmHandlerCallbacks->OnMacProcess;

    IsClassBSwitchPending = false;
    IsUplinkTxPending = false;

    if( LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LmHandlerParams->Region ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    // Restore data if required
    nbNvmData = NvmDataMgmtRestore( );

    // Try to restore from NVM and query the mac if possible.
    if( ( LmHandlerCallbacks->OnNvmDataChange != NULL ) && ( nbNvmData > 0 ) )
    {
        LmHandlerCallbacks->OnNvmDataChange( LORAMAC_HANDLER_NVM_RESTORE, nbNvmData );
    }
    else
    {
        // Configure the default datarate
        mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;
        mibReq.Param.ChannelsDefaultDatarate = LmHandlerParams->TxDatarate;
        LoRaMacMibSetRequestConfirm( &mibReq );

        mibReq.Type = MIB_CHANNELS_DATARATE;
        mibReq.Param.ChannelsDatarate = LmHandlerParams->TxDatarate;
        LoRaMacMibSetRequestConfirm( &mibReq );

#if( OVER_THE_AIR_ACTIVATION == 0 )
        // Tell the MAC layer which network server version are we connecting too.
        mibReq.Type = MIB_ABP_LORAWAN_VERSION;
        mibReq.Param.AbpLrWanVersion.Value = ABP_ACTIVATION_LRWAN_VERSION;
        LoRaMacMibSetRequestConfirm( &mibReq );

        mibReq.Type = MIB_NET_ID;
        mibReq.Param.NetID = LORAWAN_NETWORK_ID;
        LoRaMacMibSetRequestConfirm( &mibReq );

#if( STATIC_DEVICE_ADDRESS != 1 )
        // TODO: Remove STATIC_DEVICE_ADDRESS = 0. Up to the application to decide what to do
        // Random seed initialization
        if( LmHandlerCallbacks->GetRandomSeed != NULL )
        {
            srand1( LmHandlerCallbacks->GetRandomSeed( ) );
        }
        // Choose a random device address
        CommissioningParams.DevAddr = randr( 0, 0x01FFFFFF );
#endif

        mibReq.Type = MIB_DEV_ADDR;
        mibReq.Param.DevAddr = CommissioningParams.DevAddr;
        LoRaMacMibSetRequestConfirm( &mibReq );
#endif // #if( OVER_THE_AIR_ACTIVATION == 0 )
    }

    // Read secure-element DEV_EUI, JOI_EUI and SE_PIN values.
    mibReq.Type = MIB_DEV_EUI;
    LoRaMacMibGetRequestConfirm( &mibReq );
    memcpy1( CommissioningParams.DevEui, mibReq.Param.DevEui, 8 );

    mibReq.Type = MIB_JOIN_EUI;
    LoRaMacMibGetRequestConfirm( &mibReq );
    memcpy1( CommissioningParams.JoinEui, mibReq.Param.JoinEui, 8 );

    mibReq.Type = MIB_SE_PIN;
    LoRaMacMibGetRequestConfirm( &mibReq );
    memcpy1( CommissioningParams.SePin, mibReq.Param.SePin, 4 );

    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = LmHandlerParams->PublicNetworkEnable;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LmHandlerParams->AdrEnable;
    LoRaMacMibSetRequestConfirm( &mibReq );

    LoRaMacTestSetDutyCycleOn( LmHandlerParams->DutyCycleEnabled );

    LoRaMacStart( );

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
        {
            if( LmHandlerCallbacks->OnNetworkParametersChange != NULL )
            {
                LmHandlerCallbacks->OnNetworkParametersChange( &CommissioningParams );
            }
        }
    }
    return LORAMAC_HANDLER_SUCCESS;
}

bool LmHandlerIsBusy( void )
{
    if( LoRaMacIsBusy( ) == true )
    {
        return true;
    }
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        // The network isn't yet joined, try again later.
        LmHandlerJoin( );
        return true;
    }

    if( LmHandlerPackageIsTxPending( ) == true )
    {
        return true;
    }

    return false;
}

void LmHandlerProcess( void )
{
    uint16_t size = 0;

    // Process Radio IRQ
    if( Radio.IrqProcess != NULL )
    {
        Radio.IrqProcess( );
    }

    // Processes the LoRaMac events
    LoRaMacProcess( );

    // Store to NVM if required
    size = NvmDataMgmtStore( );

    if( size > 0 )
    {
        if( LmHandlerCallbacks->OnNvmDataChange != NULL )
        {
            LmHandlerCallbacks->OnNvmDataChange( LORAMAC_HANDLER_NVM_STORE, size );
        }
    }

    // Call all packages process functions
    LmHandlerPackagesProcess( );

    // Check if a package transmission is pending.
    // If it is the case exit function earlier
    if( LmHandlerPackageIsTxPending( ) == true )
    {
        return;
    }

    // If a MAC layer scheduled uplink is still pending try to send it.
    if( IsUplinkTxPending == true )
    {
        // Send an empty message
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0,
        };

        if( LmHandlerSend( &appData, LmHandlerParams->IsTxConfirmed ) == LORAMAC_HANDLER_SUCCESS )
        {
            IsUplinkTxPending = false;
        }
    }
}

TimerTime_t LmHandlerGetDutyCycleWaitTime( void )
{
    return DutyCycleWaitTime;
}

/*!
 * Join a LoRa Network in classA
 *
 * \Note if the device is ABP, this is a pass through function
 *
 * \param [IN] isOtaa Indicates which activation mode must be used
 */
static void LmHandlerJoinRequest( bool isOtaa )
{
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.Datarate = LmHandlerParams->TxDatarate;

    if( isOtaa == true )
    {
        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_OTAA;
        // Update commissioning parameters activation type variable.
        CommissioningParams.IsOtaaActivation = true;
    }
    else
    {
        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_ABP;
        // Update commissioning parameters activation type variable.
        CommissioningParams.IsOtaaActivation = false;
    }
    // Starts the join procedure
    LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
    if( LmHandlerCallbacks->OnMacMlmeRequest != NULL )
    {
        LmHandlerCallbacks->OnMacMlmeRequest( status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime );
    }
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;
}

void LmHandlerJoin( void )
{
    LmHandlerJoinRequest( CommissioningParams.IsOtaaActivation );
}

LmHandlerFlagStatus_t LmHandlerJoinStatus( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if( status == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
        {
            return LORAMAC_HANDLER_RESET;
        }
        else
        {
            return LORAMAC_HANDLER_SET;
        }
    }
    else
    {
        return LORAMAC_HANDLER_RESET;
    }
}

LmHandlerErrorStatus_t LmHandlerSend( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed )
{
    LoRaMacStatus_t status;
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        // The network isn't joined, try again.
        LmHandlerJoinRequest( CommissioningParams.IsOtaaActivation );
        return LORAMAC_HANDLER_ERROR;
    }

    TxParams.MsgType = isTxConfirmed;
    mcpsReq.Type = ( isTxConfirmed == LORAMAC_HANDLER_UNCONFIRMED_MSG ) ? MCPS_UNCONFIRMED : MCPS_CONFIRMED;
    mcpsReq.Req.Unconfirmed.Datarate = LmHandlerParams->TxDatarate;
    if( LoRaMacQueryTxPossible( appData->BufferSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    }
    else
    {
        mcpsReq.Req.Unconfirmed.fPort = appData->Port;
        mcpsReq.Req.Unconfirmed.fBufferSize = appData->BufferSize;
        mcpsReq.Req.Unconfirmed.fBuffer = appData->Buffer;
    }

    TxParams.AppData = *appData;
    TxParams.Datarate = LmHandlerParams->TxDatarate;

    status = LoRaMacMcpsRequest( &mcpsReq );
    if( LmHandlerCallbacks->OnMacMcpsRequest != NULL )
    {
        LmHandlerCallbacks->OnMacMcpsRequest( status, &mcpsReq, mcpsReq.ReqReturn.DutyCycleWaitTime );
    }
    DutyCycleWaitTime = mcpsReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        IsUplinkTxPending = false;
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerDeviceTimeReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_DEVICE_TIME;

    status = LoRaMacMlmeRequest( &mlmeReq );
    if( LmHandlerCallbacks->OnMacMlmeRequest != NULL )
    {
        LmHandlerCallbacks->OnMacMlmeRequest( status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime );
    }
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static LmHandlerErrorStatus_t LmHandlerBeaconReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_BEACON_ACQUISITION;

    status = LoRaMacMlmeRequest( &mlmeReq );
    if( LmHandlerCallbacks->OnMacMlmeRequest != NULL )
    {
        LmHandlerCallbacks->OnMacMlmeRequest( status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime );
    }
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerPingSlotReq( uint8_t periodicity )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_PING_SLOT_INFO;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = periodicity;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

    status = LoRaMacMlmeRequest( &mlmeReq );
    if( LmHandlerCallbacks->OnMacMlmeRequest != NULL )
    {
        LmHandlerCallbacks->OnMacMlmeRequest( status, &mlmeReq, mlmeReq.ReqReturn.DutyCycleWaitTime );
    }
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        // Send an empty message
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0,
        };
        return LmHandlerSend( &appData, LmHandlerParams->IsTxConfirmed );
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerRequestClass( DeviceClass_t newClass )
{
    MibRequestConfirm_t mibReq;
    DeviceClass_t currentClass;
    LmHandlerErrorStatus_t errorStatus = LORAMAC_HANDLER_SUCCESS;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm( &mibReq );
    currentClass = mibReq.Param.Class;

    // Attempt to switch only if class update
    if( currentClass != newClass )
    {
        switch( newClass )
        {
        case CLASS_A:
            {
                if( currentClass != CLASS_A )
                {
                    mibReq.Param.Class = CLASS_A;
                    if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                    {
                        // Switch is instantaneous
                        OnClassChangeNotify( CLASS_A );
                    }
                    else
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                }
            }
            break;
        case CLASS_B:
            {
                if( IsClassBSwitchPending == false )
                {
                    if( currentClass != CLASS_A )
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                    // Beacon must first be acquired
                    errorStatus = LmHandlerDeviceTimeReq( );
                    if( errorStatus == LORAMAC_HANDLER_SUCCESS)
                    {
                        IsClassBSwitchPending = true;
                    }
                }
            }
            break;
        case CLASS_C:
            {
                if( currentClass != CLASS_A )
                {
                    errorStatus = LORAMAC_HANDLER_ERROR;
                }
                // Switch is instantaneous
                mibReq.Param.Class = CLASS_C;
                if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                {
                    OnClassChangeNotify( CLASS_C );
                }
                else
                {
                    errorStatus = LORAMAC_HANDLER_ERROR;
                }
            }
            break;
        default:
            break;
        }
    }
    return errorStatus;
}

DeviceClass_t LmHandlerGetCurrentClass( void )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm( &mibReq );

    return mibReq.Param.Class;
}

int8_t LmHandlerGetCurrentDatarate( void )
{
    MibRequestConfirm_t mibGet;

    mibGet.Type = MIB_CHANNELS_DATARATE;
    LoRaMacMibGetRequestConfirm( &mibGet );

    return mibGet.Param.ChannelsDatarate;
}

LoRaMacRegion_t LmHandlerGetActiveRegion( void )
{
    return LmHandlerParams->Region;
}

LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
    mibReq.Param.SystemMaxRxError = maxErrorInMs;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

/*
 *=============================================================================
 * LORAMAC NOTIFICATIONS HANDLING
 *=============================================================================
 */

static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    TxParams.IsMcpsConfirm = 1;
    TxParams.Status = mcpsConfirm->Status;
    TxParams.Datarate = mcpsConfirm->Datarate;
    TxParams.UplinkCounter = mcpsConfirm->UpLinkCounter;
    TxParams.TxPower = mcpsConfirm->TxPower;
    TxParams.Channel = mcpsConfirm->Channel;
    TxParams.AckReceived = mcpsConfirm->AckReceived;

    if( LmHandlerCallbacks->OnTxData != NULL )
    {
        LmHandlerCallbacks->OnTxData( &TxParams );
    }

    LmHandlerPackagesNotify( PACKAGE_MCPS_CONFIRM, mcpsConfirm );
}

static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    LmHandlerAppData_t appData;

    RxParams.IsMcpsIndication = 1;
    RxParams.Status = mcpsIndication->Status;

    if( RxParams.Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    RxParams.Datarate = mcpsIndication->RxDatarate;
    RxParams.Rssi = mcpsIndication->Rssi;
    RxParams.Snr = mcpsIndication->Snr;
    RxParams.DownlinkCounter = mcpsIndication->DownLinkCounter;
    RxParams.RxSlot = mcpsIndication->RxSlot;

    appData.Port = mcpsIndication->Port;
    appData.BufferSize = mcpsIndication->BufferSize;
    appData.Buffer = mcpsIndication->Buffer;

    if( LmHandlerCallbacks->OnRxData != NULL )
    {
        LmHandlerCallbacks->OnRxData( &appData, &RxParams );
    }

    if( mcpsIndication->DeviceTimeAnsReceived == true )
    {
        if( LmHandlerCallbacks->OnSysTimeUpdate != NULL )
        {
#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
            // Provide fix values. DeviceTimeAns is accurate
            LmHandlerCallbacks->OnSysTimeUpdate( true, 0 );
#else
            LmHandlerCallbacks->OnSysTimeUpdate( );
#endif
        }
    }
    // Call packages RxProcess function
    LmHandlerPackagesNotify( PACKAGE_MCPS_INDICATION, mcpsIndication );

    if( mcpsIndication->IsUplinkTxPending != 0 )
    {
        // The server signals that it has pending data to be sent.
        // We schedule an uplink as soon as possible to flush the server.
        IsUplinkTxPending = true;
    }
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    TxParams.IsMcpsConfirm = 0;
    TxParams.Status = mlmeConfirm->Status;
    if( LmHandlerCallbacks->OnTxData != NULL )
    {
        LmHandlerCallbacks->OnTxData( &TxParams );
    }

    LmHandlerPackagesNotify( PACKAGE_MLME_CONFIRM, mlmeConfirm );

    switch( mlmeConfirm->MlmeRequest )
    {
    case MLME_JOIN:
        {
            MibRequestConfirm_t mibReq;
            mibReq.Type = MIB_DEV_ADDR;
            LoRaMacMibGetRequestConfirm( &mibReq );
            JoinParams.CommissioningParams->DevAddr = mibReq.Param.DevAddr;
            JoinParams.Datarate = LmHandlerGetCurrentDatarate( );

            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Status is OK, node has joined the network
                JoinParams.Status = LORAMAC_HANDLER_SUCCESS;
            }
            else
            {
                // Join was not successful. Try to join again
                JoinParams.Status = LORAMAC_HANDLER_ERROR;
            }
            // Notify upper layer
            if( LmHandlerCallbacks->OnJoinRequest != NULL )
            {
                LmHandlerCallbacks->OnJoinRequest( &JoinParams );
            }
        }
        break;
    case MLME_LINK_CHECK:
        {
            // Check DemodMargin
            // Check NbGateways
        }
        break;
    case MLME_DEVICE_TIME:
        {
            if( IsClassBSwitchPending == true )
            {
                LmHandlerBeaconReq( );
            }
        }
        break;
    case MLME_BEACON_ACQUISITION:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Beacon has been acquired
                // Request server for ping slot
                LmHandlerPingSlotReq( LmHandlerParams->PingSlotPeriodicity );
            }
            else
            {
                // Beacon not acquired
                // Request Device Time again.
                LmHandlerDeviceTimeReq( );
            }
        }
        break;
    case MLME_PING_SLOT_INFO:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                MibRequestConfirm_t mibReq;

                // Class B is now activated
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_B;
                LoRaMacMibSetRequestConfirm( &mibReq );
                // Notify upper layer
                OnClassChangeNotify( CLASS_B );
                IsClassBSwitchPending = false;
            }
            else
            {
                LmHandlerPingSlotReq( LmHandlerParams->PingSlotPeriodicity );
            }
        }
        break;
    default:
        break;
    }
}

static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
    RxParams.IsMcpsIndication = 0;
    RxParams.Status = mlmeIndication->Status;
    if( RxParams.Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
    {
        if( LmHandlerCallbacks->OnRxData != NULL )
        {
            LmHandlerCallbacks->OnRxData( NULL, &RxParams );
        }
    }

    // Call packages RxProcess function
    LmHandlerPackagesNotify( PACKAGE_MLME_INDICATION, mlmeIndication );

    switch( mlmeIndication->MlmeIndication )
    {
    case MLME_BEACON_LOST:
        {
            MibRequestConfirm_t mibReq;
            // Switch to class A again
            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = CLASS_A;
            LoRaMacMibSetRequestConfirm( &mibReq );

            BeaconParams.State = LORAMAC_HANDLER_BEACON_LOST;
            BeaconParams.Info.Time.Seconds = 0;
            BeaconParams.Info.GwSpecific.InfoDesc = 0;
            memset1( BeaconParams.Info.GwSpecific.Info, 0, 6 );

            OnClassChangeNotify( CLASS_A );
            OnBeaconStatusChangeNotify( &BeaconParams );

            LmHandlerDeviceTimeReq( );
        }
        break;
    case MLME_BEACON:
    {
        if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
        {
            BeaconParams.State = LORAMAC_HANDLER_BEACON_RX;
            BeaconParams.Info = mlmeIndication->BeaconInfo;

            OnBeaconStatusChangeNotify( &BeaconParams );
        }
        else
        {
            BeaconParams.State = LORAMAC_HANDLER_BEACON_NRX;
            BeaconParams.Info = mlmeIndication->BeaconInfo;

            OnBeaconStatusChangeNotify( &BeaconParams );
        }
        break;
    }
    default:
        break;
    }
}

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */

LmHandlerErrorStatus_t LmHandlerPackageRegister( uint8_t id, void *params )
{
    LmhPackage_t *package = NULL;
    switch( id )
    {
        case PACKAGE_ID_COMPLIANCE:
        {
            package = LmphCompliancePackageFactory( );
            break;
        }
        case PACKAGE_ID_CLOCK_SYNC:
        {
            package = LmphClockSyncPackageFactory( );
            break;
        }
        case PACKAGE_ID_REMOTE_MCAST_SETUP:
        {
            package = LmhpRemoteMcastSetupPackageFactory( );
            break;
        }
        case PACKAGE_ID_FRAGMENTATION:
        {
            package = LmhpFragmentationPackageFactory( );
            break;
        }
    }
    if( package != NULL )
    {
        LmHandlerPackages[id] = package;
        LmHandlerPackages[id]->OnMacMcpsRequest = LmHandlerCallbacks->OnMacMcpsRequest;
        LmHandlerPackages[id]->OnMacMlmeRequest = LmHandlerCallbacks->OnMacMlmeRequest;
        LmHandlerPackages[id]->OnJoinRequest = LmHandlerJoinRequest;
        LmHandlerPackages[id]->OnDeviceTimeRequest = LmHandlerDeviceTimeReq;
        LmHandlerPackages[id]->OnSysTimeUpdate = LmHandlerCallbacks->OnSysTimeUpdate;
        LmHandlerPackages[id]->Init( params, LmHandlerParams->DataBuffer, LmHandlerParams->DataBufferMaxSize );

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

bool LmHandlerPackageIsInitialized( uint8_t id )
{
    if( LmHandlerPackages[id]->IsInitialized != NULL )
    {
        return LmHandlerPackages[id]->IsInitialized( );
    }
    else
    {
        return false;
    }
}

static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( LmHandlerPackages[i] != NULL )
        {
            switch( notifyType )
            {
                case PACKAGE_MCPS_CONFIRM:
                {
                    if( LmHandlerPackages[i]->OnMcpsConfirmProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMcpsConfirmProcess( ( McpsConfirm_t* ) params );
                    }
                    break;
                }
                case PACKAGE_MCPS_INDICATION:
                {
                    if( LmHandlerPackages[i]->OnMcpsIndicationProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMcpsIndicationProcess( ( McpsIndication_t* )params );
                    }
                    break;
                }
                case PACKAGE_MLME_CONFIRM:
                {
                    if( LmHandlerPackages[i]->OnMlmeConfirmProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMlmeConfirmProcess( ( MlmeConfirm_t* )params );
                    }
                    break;
                }
                case PACKAGE_MLME_INDICATION:
                {
                    if( LmHandlerPackages[i]->OnMlmeIndicationProcess != NULL )
                    {
                        LmHandlerPackages[i]->OnMlmeIndicationProcess( params );
                    }
                    break;
                }
            }
        }
    }
}

static bool LmHandlerPackageIsTxPending( void )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( LmHandlerPackages[i] != NULL )
        {
            if( LmHandlerPackages[i]->IsTxPending( ) == true )
            {
                return true;
            }
        }
    }
    return false;
}

static void LmHandlerPackagesProcess( void )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( ( LmHandlerPackages[i] != NULL ) &&
            ( LmHandlerPackages[i]->Process != NULL ) &&
            ( LmHandlerPackageIsInitialized( i ) != false ) )
        {
            LmHandlerPackages[i]->Process( );
        }
    }
}

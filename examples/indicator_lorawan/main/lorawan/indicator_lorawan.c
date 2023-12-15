#include "indicator_lorawan.h"
#include "freertos/semphr.h"
#include "nvs.h"
#include "esp_err.h"

#include "indicator_sensor.h"

/* LoRaMac */
#include "utilities.h"
#include "RegionCommon.h"
#include "Commissioning.h"
#include "LmHandler.h"
#include "LmhpCompliance.h"
#include "LmHandlerMsgDisplay.h"
#include "githubVersion.h"
#include "timer.h"

#define DISPLAY_CFG_STORAGE "lorawan"

static const char *TAG = "lorawan-model";

extern const char *MacStatusStrings[];

static SemaphoreHandle_t __g_data_mutex;

#define USING_NVS 1
#if  USING_NVS == 0
#include "indicator_storage_spiffs.h"
#endif
struct indicator_lorawan {
    struct view_data_lorawan_basic_cfg    basic_cfg;
    struct view_data_lorawan_network_info info;
    bool                                  auto_join;
};


struct indicator_lorawan_sensor_data {
    bool  is_valid;
    float value;
};

static struct indicator_lorawan __g_lorawan_model;


#define LORAWAN_DATA_INDEX_TEMP     0
#define LORAWAN_DATA_INDEX_HUMIDITY 1
#define LORAWAN_DATA_INDEX_CO2      2
#define LORAWAN_DATA_INDEX_TVOC     3
#define LORAWAN_DATA_INDEX_LIGHT    4
#define LORAWAN_DATA_INDEX_MAX      5

static struct indicator_lorawan_sensor_data lorawan_data[LORAWAN_DATA_INDEX_MAX];

/*****************************************************************************/
//   config handle
/*****************************************************************************/

static void __lorawan_basic_cfg_set(struct view_data_lorawan_basic_cfg *p_data)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(&__g_lorawan_model.basic_cfg, p_data, sizeof(struct view_data_lorawan_basic_cfg));
    xSemaphoreGive(__g_data_mutex);
}

static void __lorawan_basic_cfg_get(struct view_data_lorawan_basic_cfg *p_data)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(p_data, &__g_lorawan_model.basic_cfg, sizeof(struct view_data_lorawan_basic_cfg));
    xSemaphoreGive(__g_data_mutex);
}

static void __lorawan_network_info_set(struct view_data_lorawan_network_info *p_data)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(&__g_lorawan_model.info, p_data, sizeof(struct view_data_lorawan_network_info));
    xSemaphoreGive(__g_data_mutex);
}

static void __lorawan_network_info_get(struct view_data_lorawan_network_info *p_data)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    memcpy(p_data, &__g_lorawan_model.info, sizeof(struct view_data_lorawan_network_info));
    xSemaphoreGive(__g_data_mutex);
}

void lorawan_network_info_printf(struct view_data_lorawan_network_info *p_info)
{
    int i = 0;
    printf("EUI: ");
    for (i = 0; i < 8; i++) {
        printf("%02X", p_info->eui[i]);
    }
    printf("\r\n");

    printf("JOIN EUI: ");
    for (i = 0; i < 8; i++) {
        printf("%02X", p_info->join_eui[i]);
    }
    printf("\r\n");

    printf("APP KEY: ");
    for (i = 0; i < 16; i++) {
        printf("%02X", p_info->app_key[i]);
    }
    printf("\r\n");

    printf("DEV ADDR (ABP): ");
    for (i = 0; i < 4; i++) {
        printf("%02X", p_info->dev_addr[i]);
    }
    printf("\r\n");

    printf("APPS KEY (ABP): ");
    for (i = 0; i < 16; i++) {
        printf("%02X", p_info->apps_key[i]);
    }
    printf("\r\n");

    printf("NWKS KEY (ABP): ");
    for (i = 0; i < 16; i++) {
        printf("%02X", p_info->nwks_key[i]);
    }
    printf("\r\n\r\n");
}

void lorawan_basic_cfg_printf(struct view_data_lorawan_basic_cfg *p_info)
{
    printf("CLASS: ");
    switch (p_info->class) {
        case CLASS_A:
            printf("A\r\n");
            break;
        case CLASS_B:
            printf("B\r\n");
            break;
        case CLASS_C:
            printf("C\r\n");
            break;
        default:
            printf("\r\n");
            break;
    }

    printf("REGION: ");
    switch (p_info->region) {
        case LORAMAC_REGION_AS923:
            printf("AS923\r\n");
            break;
        case LORAMAC_REGION_AU915:
            printf("AU915\r\n");
            break;
        case LORAMAC_REGION_CN470:
            printf("CN470\r\n");
            break;
        case LORAMAC_REGION_CN779:
            printf("CN779\r\n");
            break;
        case LORAMAC_REGION_EU433:
            printf("EU433\r\n");
            break;
        case LORAMAC_REGION_EU868:
            printf("EU868\r\n");
            break;
        case LORAMAC_REGION_KR920:
            printf("KR920\r\n");
            break;
        case LORAMAC_REGION_IN865:
            printf("IN865\r\n");
            break;
        case LORAMAC_REGION_US915:
            printf("US915\r\n");
            break;
        case LORAMAC_REGION_RU864:
            printf("RU864\r\n");
            break;
        default:
            printf("\r\n");
            break;
    }
    printf("OTAA: %d\r\n", p_info->IsOtaaActivation);
    printf("INTERVAL: %d min\r\n", p_info->uplink_interval_min);
}


static void __lorawan_basic_cfg_save(struct view_data_lorawan_basic_cfg *p_data)
{
    esp_err_t ret = 0;
#if USING_NVS == 1
    ret = indicator_nvs_write(LORAWAN_BASIC_CFG_STORAGE, (void *)p_data, sizeof(struct view_data_lorawan_basic_cfg));
#else
    ret = indicator_spiffs_write(SPIFFS_BASE_PATH "/" LORAWAN_BASIC_CFG_STORAGE, (void *)p_data, sizeof(struct view_data_lorawan_basic_cfg));
#endif
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "cfg write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "cfg write successful");
    }
}

static void __lorawan_join_auto_flag_save(bool flag)
{
    esp_err_t ret = 0;
#if USING_NVS == 1
    ret = indicator_nvs_write(LORAWAN_AUTO_JOIN_FLAG_STORAGE, (void *)&flag, sizeof(flag));
#else
    ret = indicator_spiffs_write(SPIFFS_BASE_PATH "/" LORAWAN_AUTO_JOIN_FLAG_STORAGE, (void *)&flag, sizeof(flag));
#endif
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "flag write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "flag write successful");
    }
}

static void __lorawan_light_st_save(bool st)
{
    esp_err_t ret = 0;
#if USING_NVS == 1
    ret = indicator_nvs_write(LORAWAN_LIGHT_FLAG_STORAGE, (void *)&st, sizeof(st));
#else
    ret = indicator_spiffs_write(SPIFFS_BASE_PATH "/" LORAWAN_LIGHT_FLAG_STORAGE, (void *)&st, sizeof(st));
#endif
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "st write err:%d", ret);
    } else {
        ESP_LOGI(TAG, "st write successful");
    }
}


static void __lorawan_cfg_restore(void)
{
    esp_err_t ret                          = 0;

    struct view_data_lorawan_basic_cfg cfg = {0};
    size_t                             len = sizeof(cfg);
#if USING_NVS == 1
    ret = indicator_nvs_read(LORAWAN_BASIC_CFG_STORAGE, (void *)&cfg, &len);
#else
    ret = indicator_spiffs_read(SPIFFS_BASE_PATH "/" LORAWAN_BASIC_CFG_STORAGE, (void *)&cfg, &len);
#endif
    if (ret == ESP_OK && len == (sizeof(cfg))) {
        ESP_LOGI(TAG, "lorawan basic cfg read successful");
    } else {
        // err or not find
        if (ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "lorawan basic cfg not find");
        } else {
            ESP_LOGI(TAG, "lorawan basic cfg read err:%d", ret);
        }

        cfg.class               = CLASS_A;
        cfg.region              = LORAMAC_REGION_US915;
        cfg.IsOtaaActivation    = true;
        cfg.uplink_interval_min = 5;
    }

    lorawan_basic_cfg_printf(&cfg);
    __lorawan_basic_cfg_set(&cfg);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_UPDATE, (void *)&cfg, sizeof(cfg), portMAX_DELAY);

    struct view_data_lorawan_network_info info = {0};
    len                                        = sizeof(info);
#if USING_NVS == 1
    ret = indicator_nvs_read(LORAWAN_NETWORK_INFO_STORAGE, (void *)&info, &len);
#else
    ret = indicator_spiffs_read(SPIFFS_BASE_PATH "/" LORAWAN_NETWORK_INFO_STORAGE, (void *)&info, &len);
#endif
    if (ret == ESP_OK && len == (sizeof(info))) {
        ESP_LOGI(TAG, "lorawan network info read successful");
    } else {
        if (ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "lorawan network info not find");
        } else {
            ESP_LOGI(TAG, "lorawan network info read err:%d", ret);
        }
        memset(&info, 0, sizeof(info));
    }
    __lorawan_network_info_set(&info);
    lorawan_network_info_printf(&info);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE, (void *)&info, sizeof(info), portMAX_DELAY);

    bool auto_join = false;
    len            = 1;
#if USING_NVS == 1
    ret = indicator_nvs_read(LORAWAN_AUTO_JOIN_FLAG_STORAGE, (void *)&auto_join, &len);
#else
    ret = indicator_spiffs_read(SPIFFS_BASE_PATH "/" LORAWAN_AUTO_JOIN_FLAG_STORAGE, (void *)&auto_join, &len);
#endif
    if (ret == ESP_OK && len == (sizeof(auto_join))) {
        __g_lorawan_model.auto_join = auto_join;
        ESP_LOGI(TAG, "lorawan auto join: %d", auto_join);
    } else {
        auto_join                   = false;
        __g_lorawan_model.auto_join = false;
    }
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ACTION, (void *)&auto_join, 1, portMAX_DELAY);

    bool light_st = false;
    len           = 1;
#if USING_NVS == 1
    ret = indicator_nvs_read(LORAWAN_LIGHT_FLAG_STORAGE, (void *)&light_st, &len);
#else
    ret = indicator_spiffs_read(SPIFFS_BASE_PATH "/" LORAWAN_LIGHT_FLAG_STORAGE, (void *)&light_st, &len);
#endif
    if (ret == ESP_OK && len == (sizeof(light_st))) {
        ESP_LOGI(TAG, "light last st: %d", light_st);
    } else {
        light_st = false;
    }

    lorawan_data[LORAWAN_DATA_INDEX_LIGHT].is_valid = true;
    if (light_st) {
        lorawan_data[LORAWAN_DATA_INDEX_LIGHT].value = 1.0;
    } else {
        lorawan_data[LORAWAN_DATA_INDEX_LIGHT].value = 0.0;
    }

    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LIGHT_ST, (void *)&light_st, 1, portMAX_DELAY);
}


static int lorawan_log_display(uint8_t level, const char *fmt, ...)
{
    int                         n;
    struct view_data_lorawa_log log = {0};

    va_list args;
    va_start(args, fmt);
    n = vsnprintf(log.data, sizeof(log.data), fmt, args);
    if (n > (int)sizeof(log.data)) {
        n = sizeof(log.data);
    }
    va_end(args);

    log.level = level;

    printf("%s", log.data);
    esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LOG, (void *)&log, sizeof(log), portMAX_DELAY);

    return n;
}

/*****************************************************************************/
//   lorawan handle
/*****************************************************************************/

#define ACTIVE_REGION LORAMAC_REGION_US915

static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

static TimerEvent_t TxTimer;

static volatile uint8_t  IsTxFramePending = 0;
static volatile uint32_t TxPeriodicity    = 0;

static LmHandlerAppData_t AppData =
    {
        .Buffer     = AppDataBuffer,
        .BufferSize = 0,
        .Port       = 0,
};

static void OnTxTimerEvent(void *context);
static void OnNetworkParametersChange(CommissioningParams_t *params)
{
}

static void OnMacMcpsRequest(LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxIn)
{
    // DisplayMacMcpsRequestUpdate( status, mcpsReq, nextTxIn );
    switch (mcpsReq->Type) {
        case MCPS_CONFIRMED: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Send confirm Packet, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        case MCPS_UNCONFIRMED: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Send unconfirm Packet, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        default: {
            break;
        }
    }
    if (status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED) {
        lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Next Tx in  : %lu [ms]\r\n", nextTxIn);
    }
}

static void OnMacMlmeRequest(LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxIn)
{
    switch (mlmeReq->Type) {
        case MLME_JOIN: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Start join, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        case MLME_LINK_CHECK: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Link check, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        case MLME_DEVICE_TIME: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Device time, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        case MLME_TXCW: {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Txcw, status: %s\r\n", MacStatusStrings[status]);
            break;
        }
        default: {
            break;
        }
    }
    if (status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED) {
        lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Next Tx in  : %lu [ms]\r\n", nextTxIn);
    }
}

static void OnJoinRequest(LmHandlerJoinParams_t *params)
{
    if (params->CommissioningParams->IsOtaaActivation == true) {
        if (params->Status == LORAMAC_HANDLER_SUCCESS) {
            lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Join Successful , DevAddr: %08lX \r\n", params->CommissioningParams->DevAddr);
        } else {
            lorawan_log_display(LORAWAN_LOG_LEVEL_ERROR, "Join fail!\r\n");
        }
    } else {
        lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Joind, DevAddr: %08lX \r\n", params->CommissioningParams->DevAddr);
    }

    if (params->Status == LORAMAC_HANDLER_ERROR) {
        if (__g_lorawan_model.auto_join) {
            LmHandlerJoin();
        }
    } else {

        enum view_data_lorawan_join join_st = LORAWAN_JOIN_ST_OK;
        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ST, (void *)&join_st, sizeof(join_st), portMAX_DELAY);

        LmHandlerRequestClass(__g_lorawan_model.basic_cfg.class);

        OnTxTimerEvent(NULL); // send data right now
    }
}

static void OnTxData(LmHandlerTxParams_t *params)
{
    // DisplayTxUpdate( params );
    MibRequestConfirm_t mibGet;

    uint8_t ack_buf[64]   = {0};
    uint8_t data_buf[128] = {0};
    uint8_t level         = LORAWAN_LOG_LEVEL_INFO;
    uint8_t index         = 0;

    if (params->AppData.BufferSize != 0) {
        if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG) {
            if (params->AckReceived == 0) {
                level = LORAWAN_LOG_LEVEL_WARN;
            }

            snprintf(ack_buf, sizeof(ack_buf), "confirm-%s", (params->AckReceived != 0) ? "ACK" : "NACK");
        } else {
            snprintf(ack_buf, sizeof(ack_buf), "unconfirm");
        }

        for (uint8_t i = 0; i < params->AppData.BufferSize; i++) {
            index += sprintf(data_buf + index, "%02X", params->AppData.Buffer[i]);
        }

        mibGet.Type = MIB_CHANNELS;
        LoRaMacMibGetRequestConfirm(&mibGet);
        lorawan_log_display(level, "--> Uplink frame(%lu), FREQ:%lu, DR:%d, %s\r\n", params->UplinkCounter, mibGet.Param.ChannelList[params->Channel].Frequency, params->Datarate, ack_buf);
        lorawan_log_display(level, "Port:%d, data: %s\r\n\r\n", params->AppData.Port, data_buf);
    }
}

static lorawan_rx_data_handle(uint8_t *p_buf, uint8_t len, uint8_t port);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
    uint8_t     data_buf[128] = {0};
    const char *slotStrings[] = {"1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot"};
    uint8_t     index         = 0;
    lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "<-- Downlink frame(%lu), window:%s, DR:%d, RSSI:%d, SNR:%d\r\n", params->DownlinkCounter, slotStrings[params->RxSlot], params->Datarate, params->Rssi, params->Snr);
    if (appData->BufferSize != 0) {
        for (uint8_t i = 0; i < appData->BufferSize; i++) {
            index += sprintf(data_buf + index, "%02X", appData->Buffer[i]);
        }
        lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Port:%d, data: %s\r\n\r\n", appData->Port, data_buf);
    }


    if (appData->BufferSize != 0) {
        lorawan_rx_data_handle(appData->Buffer, appData->BufferSize, appData->Port);
    }
}


static void OnClassChange(DeviceClass_t deviceClass)
{
    lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Switch to Class %c done\r\n", "ABC"[deviceClass]);

    // Inform the server as soon as possible that the end-device has switched to ClassB
    LmHandlerAppData_t appData =
        {
            .Buffer     = NULL,
            .BufferSize = 0,
            .Port       = 0,
        };
    LmHandlerSend(&appData, LORAMAC_HANDLER_UNCONFIRMED_MSG);
}

static void OnBeaconStatusChange(LoRaMacHandlerBeaconParams_t *params)
{
}

static LmHandlerCallbacks_t LmHandlerCallbacks =
    {
        .GetBatteryLevel           = NULL,
        .GetTemperature            = NULL,
        .GetRandomSeed             = NULL,
        .OnMacProcess              = NULL,
        .OnNvmDataChange           = NULL,
        .OnNetworkParametersChange = OnNetworkParametersChange,
        .OnMacMcpsRequest          = OnMacMcpsRequest,
        .OnMacMlmeRequest          = OnMacMlmeRequest,
        .OnJoinRequest             = OnJoinRequest,
        .OnTxData                  = OnTxData,
        .OnRxData                  = OnRxData,
        .OnClassChange             = OnClassChange,
        .OnBeaconStatusChange      = OnBeaconStatusChange,
        .OnSysTimeUpdate           = NULL,
};

static LmHandlerParams_t LmHandlerParams =
    {
        .Region              = ACTIVE_REGION,
        .AdrEnable           = LORAWAN_ADR_STATE,
        .IsTxConfirmed       = LORAWAN_DEFAULT_CONFIRMED_MSG_STATE,
        .TxDatarate          = LORAWAN_DEFAULT_DATARATE,
        .PublicNetworkEnable = LORAWAN_PUBLIC_NETWORK,
        .DutyCycleEnabled    = LORAWAN_DUTYCYCLE_ON,
        .DataBufferMaxSize   = LORAWAN_APP_DATA_BUFFER_MAX_SIZE,
        .DataBuffer          = AppDataBuffer,
        .PingSlotPeriodicity = REGION_COMMON_DEFAULT_PING_SLOT_PERIODICITY,
};

static void OnTxPeriodicityChanged(uint32_t periodicity);

static void OnTxFrameCtrlChanged(LmHandlerMsgTypes_t isTxConfirmed)
{
    LmHandlerParams.IsTxConfirmed = isTxConfirmed;
}
static void OnPingSlotPeriodicityChanged(uint8_t pingSlotPeriodicity)
{
    LmHandlerParams.PingSlotPeriodicity = pingSlotPeriodicity;
}
static LmhpComplianceParams_t LmhpComplianceParams =
    {
        .FwVersion.Value              = FIRMWARE_VERSION,
        .OnTxPeriodicityChanged       = OnTxPeriodicityChanged,
        .OnTxFrameCtrlChanged         = OnTxFrameCtrlChanged,
        .OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged,
};

static void OnTxTimerEvent(void *context)
{
    TimerStop(&TxTimer);

    IsTxFramePending = 1;

    // Schedule next transmission
    TimerSetValue(&TxTimer, TxPeriodicity);
    TimerStart(&TxTimer);
}

static void OnTxPeriodicityChanged(uint32_t periodicity)
{
    TxPeriodicity = periodicity;

    if (TxPeriodicity == 0) { // Revert to application default periodicity
        TxPeriodicity = __g_lorawan_model.basic_cfg.uplink_interval_min * 60 * 1000 + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
    }

    // Update timer periodicity
    TimerStop(&TxTimer);
    TimerSetValue(&TxTimer, TxPeriodicity);
    TimerStart(&TxTimer);
}


static lorawan_rx_data_handle(uint8_t *p_buf, uint8_t len, uint8_t port)
{
    if (len < 2 || (len % 2 != 0)) {
        return 0;
    }

    // handle one cmd
    uint8_t cmd = p_buf[0];

    switch (cmd) {
        case 0: {
            uint8_t data = p_buf[1];
            if (data) {
                data = 1;
                lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Turn on light\r\n");
            } else {
                lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "Turn off light\r\n");
            }
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LIGHT_ST, (void *)&data, sizeof(data), portMAX_DELAY);
            break;
        }
        default:
            break;
    }
}

static void PrepareTxFrame(void)
{
    if (LmHandlerIsBusy() == true) {
        return;
    }

    int i     = 0;
    int index = 0;

    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    for (i = 0; i < LORAWAN_DATA_INDEX_MAX; i++) {
        if (lorawan_data[i].is_valid) {
            AppData.Buffer[index++] = i; // data id
            uint32_t value          = 0;
            memcpy(&value, &lorawan_data[i].value, 4);
            AppData.Buffer[index++] = (value) & 0xff;
            AppData.Buffer[index++] = ((value) >> 8) & 0xff;
            AppData.Buffer[index++] = ((value) >> 16) & 0xff;
            AppData.Buffer[index++] = ((value) >> 24) & 0xff;
        }
    }
    xSemaphoreGive(__g_data_mutex);

    if (index == 0) {
        return;
    }

    AppData.BufferSize = index;

    AppData.Port       = LORAWAN_APP_PORT;

    if (LmHandlerSend(&AppData, LmHandlerParams.IsTxConfirmed) == LORAMAC_HANDLER_SUCCESS) {
        printf("send success!\r\n");
    }
}


static int __lorawan_init(void)
{
    struct indicator_lorawan *p_model = &__g_lorawan_model;

    LmHandlerParams.Region            = p_model->basic_cfg.region;

    if (LmHandlerInit(&LmHandlerCallbacks, &LmHandlerParams) != LORAMAC_HANDLER_SUCCESS) {
        printf("LoRaMac wasn't properly initialized\n");
        // Fatal error, endless loop.
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }

    MibRequestConfirm_t mibReq;

    // eui
    mibReq.Type         = MIB_DEV_EUI;
    mibReq.Param.DevEui = p_model->info.eui;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // join eui
    mibReq.Type          = MIB_JOIN_EUI;
    mibReq.Param.JoinEui = p_model->info.join_eui;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // app key
    mibReq.Type         = MIB_APP_KEY;
    mibReq.Param.AppKey = p_model->info.app_key;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type         = MIB_NWK_KEY;
    mibReq.Param.NwkKey = p_model->info.app_key;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // dev addr MSB
    uint32_t DevAddr = 0;

    DevAddr += p_model->info.dev_addr[0] << 24;
    DevAddr += p_model->info.dev_addr[1] << 16;
    DevAddr += p_model->info.dev_addr[2] << 8;
    DevAddr += p_model->info.dev_addr[3];

    mibReq.Type          = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = DevAddr;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // apps key
    mibReq.Type          = MIB_APP_S_KEY;
    mibReq.Param.AppSKey = p_model->info.apps_key;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // nwks key
    mibReq.Type              = MIB_F_NWK_S_INT_KEY;
    mibReq.Param.FNwkSIntKey = p_model->info.nwks_key;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type              = MIB_S_NWK_S_INT_KEY;
    mibReq.Param.SNwkSIntKey = p_model->info.nwks_key;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type             = MIB_NWK_S_ENC_KEY;
    mibReq.Param.NwkSEncKey = p_model->info.nwks_key;
    LoRaMacMibSetRequestConfirm(&mibReq);


    LmHandlerSetSystemMaxRxError(50);
    LmHandlerPackageRegister(PACKAGE_ID_COMPLIANCE, &LmhpComplianceParams);

    LmHandlerJoin1(p_model->basic_cfg.IsOtaaActivation);

    // TX timer
    TxPeriodicity = __g_lorawan_model.basic_cfg.uplink_interval_min * 60 * 1000 + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
    TimerInit(&TxTimer, OnTxTimerEvent);
    TimerSetValue(&TxTimer, TxPeriodicity);
}

static bool __lorawan_deinit(void)
{
    IsTxFramePending = 0;
    TimerStop(&TxTimer);
    LoRaMacStop();
    LoRaMacStatus_t st = LoRaMacDeInitialization();
    if (st != LORAMAC_STATUS_OK) {
        ESP_LOGE(TAG, "LoRaMacDeInitialization: %d", st);
        return false;
    }
    return true;
}

static void UplinkProcess(void)
{
    uint8_t isPending = 0;
    CRITICAL_SECTION_BEGIN();
    isPending        = IsTxFramePending;
    IsTxFramePending = 0;
    CRITICAL_SECTION_END();
    if (isPending == 1) {
        PrepareTxFrame();
    }
}


static void __lorawan_task(void *p_arg)
{
    bool start     = false;
    bool init_flag = false;

    while (1) {

        start = __g_lorawan_model.auto_join;

        if (start) {
            if (!init_flag) {
                init_flag = true;
                lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "STARTUP LoRaWAN\r\n");
                enum view_data_lorawan_join join_st = LORAWAN_JOIN_ST_PROCEED;
                esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ST, (void *)&join_st, sizeof(join_st), portMAX_DELAY);

                __lorawan_init();
            }

            LmHandlerProcess();
            UplinkProcess();
        } else {

            if (init_flag) {

                if (LoRaMacIsBusy()) {
                    // ESP_LOGI(TAG, " continue handle lorawan...");
                    LmHandlerProcess();
                    UplinkProcess();

                } else {
                    if (__lorawan_deinit()) {
                        init_flag = false;
                        lorawan_log_display(LORAWAN_LOG_LEVEL_INFO, "STOP LoRaWAN\r\n");
                        enum view_data_lorawan_join join_st = LORAWAN_JOIN_ST_FAIL; // stop
                        esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ST, (void *)&join_st, sizeof(join_st), portMAX_DELAY);

                    } else {
                        vTaskDelay(pdMS_TO_TICKS(1000));
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static void __view_event_handler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
        case VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE: {
            struct view_data_lorawan_network_info *p_info = (struct view_data_lorawan_network_info *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE");
            __lorawan_network_info_set(p_info);
            break;
        }
        case VIEW_EVENT_LORAWAN_CFG_APPLY: {
            struct view_data_lorawan_basic_cfg *p_cfg = (struct view_data_lorawan_basic_cfg *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_CFG_APPLY");
            __lorawan_basic_cfg_set(p_cfg);
            __lorawan_basic_cfg_save(p_cfg);
            break;
        }
        case VIEW_EVENT_LORAWAN_LIGHT_ST: {
            bool *p_st = (bool *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_LIGHT_ST");

            __lorawan_light_st_save(*p_st);

            xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
            if (*p_st) {
                lorawan_data[LORAWAN_DATA_INDEX_LIGHT].value = 1.0;
            } else {
                lorawan_data[LORAWAN_DATA_INDEX_LIGHT].value = 0.0;
            }
            xSemaphoreGive(__g_data_mutex);

            IsTxFramePending = 1;
            break;
        }
        case VIEW_EVENT_LORAWAN_JOIN_ACTION: {
            bool *p_st = (bool *)event_data;
            ESP_LOGI(TAG, "event: VIEW_EVENT_LORAWAN_JOIN_ACTION");
            __g_lorawan_model.auto_join = *p_st;
            __lorawan_join_auto_flag_save(__g_lorawan_model.auto_join);

            break;
        }
        case VIEW_EVENT_SENSOR_DATA: {
            struct view_data_sensor_data *p_data = (struct view_data_sensor_data *)event_data;
            xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
            // ESP_LOGI(TAG, "%s: %.0f", getSensorName(p_data->sensor_type), p_data->value);
            switch (p_data->sensor_type) {
                case SENSOR_DATA_CO2: {
                    lorawan_data[LORAWAN_DATA_INDEX_CO2].is_valid = true;
                    lorawan_data[LORAWAN_DATA_INDEX_CO2].value    = p_data->value;
                    break;
                }
                case SENSOR_DATA_TVOC: {
                    lorawan_data[LORAWAN_DATA_INDEX_TVOC].is_valid = true;
                    lorawan_data[LORAWAN_DATA_INDEX_TVOC].value    = p_data->value;
                    break;
                }
                case SENSOR_DATA_TEMP: {
                    lorawan_data[LORAWAN_DATA_INDEX_TEMP].is_valid = true;
                    lorawan_data[LORAWAN_DATA_INDEX_TEMP].value    = p_data->value;
                    break;
                }
                case SENSOR_DATA_HUMIDITY: {
                    lorawan_data[LORAWAN_DATA_INDEX_HUMIDITY].is_valid = true;
                    lorawan_data[LORAWAN_DATA_INDEX_HUMIDITY].value    = p_data->value;
                    break;
                }
                default:
                    break;
            }
            xSemaphoreGive(__g_data_mutex);
            break;
        }
        default:
            break;
    }
}


int indicator_lorawan_init(void)
{
    __g_data_mutex = xSemaphoreCreateMutex();

    __lorawan_cfg_restore();

    xTaskCreate(&__lorawan_task, "__lorawan_task", 1024 * 5, NULL, 2, NULL);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_NETWORK_INFO_UPDATE,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_CFG_APPLY,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_JOIN_ACTION,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LIGHT_ST,
                                                             __view_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA,
                                                             __view_event_handler, NULL, NULL));

    return 0;
}
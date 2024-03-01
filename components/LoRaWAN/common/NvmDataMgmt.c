/*!
 * \file      NvmDataMgmt.c
 *
 * \brief     NVM context management implementation
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
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */

#include <stdio.h>
#include "utilities.h"
#include "LoRaMac.h"
#include "NvmDataMgmt.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <string.h>
/*!
 * Enables/Disables the context storage management storage.
 * Must be enabled for LoRaWAN 1.0.4 or later.
 */
#ifndef CONTEXT_MANAGEMENT_ENABLED
#define CONTEXT_MANAGEMENT_ENABLED         1
#endif
#define  STORAGE_NAMESPACE     "lorawan"


#define  NVS_DATA_CRYPTO_KEY     "LM_Crypto"
#define  NVS_DATA_MACGROUP1_KEY  "LM_MacGroup1"
#define  NVS_DATA_MACGROUP2_KEY  "LM_MacGroup2"
#define  NVS_DATA_SECURE_KEY     "LM_Secure"
#define  NVS_DATA_REGION1_KEY    "LM_Region1"
#define  NVS_DATA_REGION2_KEY    "LM_Region2"
#define  NVS_DATA_CLASSB_KEY     "LM_Classb"

static uint16_t NvmNotifyFlags = 0;

static esp_err_t __nvs_write(char *p_key, void *p_data, size_t len)
{
    nvs_handle_t my_handle;
    esp_err_t err;
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(my_handle,  p_key, p_data, len);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return err;
    }
    err = nvs_commit(my_handle);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return err;
    }
    nvs_close(my_handle);
    return ESP_OK;
}

static esp_err_t __nvs_read(char *p_key, void *p_data, size_t *p_len)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_get_blob(my_handle, p_key, p_data, p_len);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return err;
    }
    nvs_close(my_handle);
    return ESP_OK;
}

static esp_err_t __nvs_read_then_check(char *p_key, void *p_data, size_t r_len)
{
    nvs_handle_t my_handle;
    esp_err_t err;
    uint32_t calculatedCrc32 = 0;
    uint32_t readCrc32 = 0;
    int len = 0;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_get_blob(my_handle, p_key, p_data, &r_len);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return err;
    }
    nvs_close(my_handle);

    len = r_len;
    
    calculatedCrc32 = Crc32Init( );
    for( uint16_t i = 0; i < ( len - sizeof( readCrc32 ) ); i++ )
    {
        calculatedCrc32 = Crc32Update( calculatedCrc32, &p_data[i], 1 );
    }
    calculatedCrc32 = Crc32Finalize( calculatedCrc32 );

    memcpy(&readCrc32, &p_data[len - sizeof( readCrc32 )],  sizeof( readCrc32 ));

    if( calculatedCrc32 != readCrc32 )
    {
        ESP_LOGE("lorawan", "nvs crc check fail");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t __nvm_reset(char *p_key)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_erase_key(my_handle, p_key);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return err;
    }
    nvs_close(my_handle);
    return ESP_OK;
}

void NvmDataMgmtEvent( uint16_t notifyFlags )
{
    NvmNotifyFlags = notifyFlags;
}

uint16_t NvmDataMgmtStore( void )
{
#if( CONTEXT_MANAGEMENT_ENABLED == 1 )
    uint16_t offset = 0;
    uint16_t dataSize = 0;
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );
    LoRaMacNvmData_t* nvm = mibReq.Param.Contexts;

    // Input checks
    if( NvmNotifyFlags == LORAMAC_NVM_NOTIFY_FLAG_NONE )
    {
        // There was no update.
        return 0;
    }
    if( LoRaMacStop( ) != LORAMAC_STATUS_OK )
    {
        return 0;
    }

     // Crypto
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_CRYPTO ) ==
        LORAMAC_NVM_NOTIFY_FLAG_CRYPTO )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_CRYPTO_KEY, ( uint8_t* ) &nvm->Crypto,  sizeof( nvm->Crypto ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs Crypto write err:0x%x", err);
        }        
    }

    // MacGroup1
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP1 ) ==
        LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP1 )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_MACGROUP1_KEY, ( uint8_t* ) &nvm->MacGroup1,  sizeof( nvm->MacGroup1 ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs MacGroup1 write err:0x%x", err);
        }                
    }


    // MacGroup2
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP2 ) ==
        LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP2 )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_MACGROUP2_KEY, ( uint8_t* ) &nvm->MacGroup2,  sizeof( nvm->MacGroup2 ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs MacGroup2 write err:0x%x", err);
        }     
    }

    // Secure element
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_SECURE_ELEMENT ) ==
        LORAMAC_NVM_NOTIFY_FLAG_SECURE_ELEMENT )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_SECURE_KEY, ( uint8_t* ) &nvm->SecureElement,  sizeof( nvm->SecureElement ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs SecureElement write err:0x%x", err);
        }   
    }

    // Region group 1
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP1 ) ==
        LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP1 )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_REGION1_KEY, ( uint8_t* ) &nvm->RegionGroup1,  sizeof( nvm->RegionGroup1 ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs RegionGroup1 write err:0x%x", err);
        }   
    }

    // Region group 2
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP2 ) ==
        LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP2 )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_REGION2_KEY, ( uint8_t* ) &nvm->RegionGroup2,  sizeof( nvm->RegionGroup2 ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs RegionGroup2 write err:0x%x", err);
        }     
    }

    // Class b
    if( ( NvmNotifyFlags & LORAMAC_NVM_NOTIFY_FLAG_CLASS_B ) ==
        LORAMAC_NVM_NOTIFY_FLAG_CLASS_B )
    {
        esp_err_t err  = __nvs_write(NVS_DATA_CLASSB_KEY, ( uint8_t* ) &nvm->ClassB,  sizeof( nvm->ClassB ));
        if( err != ESP_OK ) {
            ESP_LOGE("lorawan", "nvs ClassB write err:0x%x", err);
        }    
    }

    // Reset notification flags
    NvmNotifyFlags = LORAMAC_NVM_NOTIFY_FLAG_NONE;
   
    // Resume LoRaMac
    LoRaMacStart( );
    return dataSize;
#else
    return 0;
#endif
}

uint16_t NvmDataMgmtRestore( void )
{
#if( CONTEXT_MANAGEMENT_ENABLED == 1 )
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );
    LoRaMacNvmData_t* nvm = mibReq.Param.Contexts;
    uint16_t offset = 0;
    
    static LoRaMacNvmData_t data;
    esp_err_t err;
    
    LoRaMacRegion_t new_region = nvm->MacGroup2.Region;


    // Crypto
    err = __nvs_read_then_check(NVS_DATA_CRYPTO_KEY, ( uint8_t* ) &data.Crypto,  sizeof( data.Crypto ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t * ) &nvm->Crypto, (uint8_t*) &data.Crypto ,sizeof( data.Crypto ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs Crypto read err:0x%x", err);
        return  0;
    }

    // Secure element
    err = __nvs_read_then_check(NVS_DATA_SECURE_KEY, ( uint8_t* ) &data.SecureElement,  sizeof( data.SecureElement ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t*) &nvm->SecureElement, (uint8_t*) &data.SecureElement ,sizeof( data.SecureElement ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs SecureElement read err:0x%x", err);
        return  0;
    }

    // Mac Group 2
    err = __nvs_read_then_check(NVS_DATA_MACGROUP2_KEY, ( uint8_t* ) &data.MacGroup2,  sizeof( data.MacGroup2 ));
    if( err == ESP_OK )
    {
        if( new_region != data.MacGroup2.Region ) {
            ESP_LOGI("lorawan", "change region: %d --> %d", data.MacGroup2.Region, new_region);
            return 0; //When changing the region, the following fields cannot be restored.
        }

        memcpy( (uint8_t*) &nvm->MacGroup2, (uint8_t*) &data.MacGroup2 ,sizeof( data.MacGroup2 ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs MacGroup2 read err:0x%x", err);
        return  0;
    }

    // Mac Group 1
    err = __nvs_read_then_check(NVS_DATA_MACGROUP1_KEY, ( uint8_t* ) &data.MacGroup1,  sizeof( data.MacGroup1 ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t*) &nvm->MacGroup1, (uint8_t*) &data.MacGroup1 ,sizeof( data.MacGroup1 ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs MacGroup1 read err:0x%x", err);
        return  0;
    }

    // Region group 1
    err = __nvs_read_then_check(NVS_DATA_REGION1_KEY, ( uint8_t* ) &data.RegionGroup1,  sizeof( data.RegionGroup1 ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t*) &nvm->RegionGroup1, (uint8_t*) &data.RegionGroup1 ,sizeof( data.RegionGroup1 ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs RegionGroup1 read err:0x%x", err);
        return  0;
    }

    // Region group 2
    err = __nvs_read_then_check(NVS_DATA_REGION2_KEY, ( uint8_t* ) &data.RegionGroup2,  sizeof( data.RegionGroup2 ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t*) &nvm->RegionGroup2, (uint8_t*) &data.RegionGroup2 ,sizeof( data.RegionGroup2 ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs RegionGroup2 read err:0x%x", err);
        return  0;
    }

    // Class b
    err = __nvs_read_then_check(NVS_DATA_CLASSB_KEY, ( uint8_t* ) &data.ClassB,  sizeof( data.ClassB ));
    if( err == ESP_OK )
    {
        memcpy( (uint8_t*) &nvm->ClassB, (uint8_t*) &data.ClassB ,sizeof( data.ClassB ));
    } else if( err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("lorawan", "nvs ClassB read err:0x%x", err);
        return  0;
    }

    ESP_LOGI("lorawan", "NvmDataMgmtRestore ok.");
    return sizeof( LoRaMacNvmData_t );
#endif
    return 0;
}

bool NvmDataMgmtFactoryReset( void )
{
    uint16_t offset = 0;
#if( CONTEXT_MANAGEMENT_ENABLED == 1 )
    esp_err_t  err;
    err = __nvm_reset(NVS_DATA_CRYPTO_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_MACGROUP1_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_MACGROUP2_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_SECURE_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_REGION1_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_REGION2_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
    err = __nvm_reset(NVS_DATA_CLASSB_KEY);
    if(  err != ESP_OK ) {
        return false;
    }
#endif
    return true;
}

int nvm_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
}


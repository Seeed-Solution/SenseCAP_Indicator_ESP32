#include "indicator_sensor.h"
#include "driver/uart.h"
#include "cobs.h"
#include "esp_timer.h"
#include "nvs.h"
#include<stdlib.h>
#include "time.h"

#define SENSOR_HISTORY_DATA_DEBUG  0
#define SENSOR_COMM_DEBUG    0


#define ESP32_RP2040_TXD (19)
#define ESP32_RP2040_RXD (20)
#define ESP32_RP2040_RTS (UART_PIN_NO_CHANGE)
#define ESP32_RP2040_CTS (UART_PIN_NO_CHANGE)

#define ESP32_COMM_PORT_NUM      (2)
#define ESP32_COMM_BAUD_RATE     (115200)
#define ESP32_RP2040_COMM_TASK_STACK_SIZE    (1024*4)
#define BUF_SIZE (512)

uint8_t buf[BUF_SIZE];   //recv 
uint8_t data[BUF_SIZE];  //decode

enum  pkt_type {

    PKT_TYPE_CMD_COLLECT_INTERVAL = 0xA0, //uin32_t 
    PKT_TYPE_CMD_BEEP_ON  = 0xA1,  //uin32_t  ms: on time 
    PKT_TYPE_CMD_BEEP_OFF = 0xA2,
    PKT_TYPE_CMD_SHUTDOWN = 0xA3, //uin32_t 
    PKT_TYPE_CMD_POWER_ON = 0xA4,

    PKT_TYPE_SENSOR_SCD41_TEMP  = 0xB0, // float
    PKT_TYPE_SENSOR_SCD41_HUMIDITY = 0xB1, // float
    PKT_TYPE_SENSOR_SCD41_CO2 = 0xB2, // float

    PKT_TYPE_SENSOR_SHT41_TEMP = 0xB3, // float
    PKT_TYPE_SENSOR_SHT41_HUMIDITY = 0xB4, // float

    PKT_TYPE_SENSOR_TVOC_INDEX = 0xB5, // float

    //todo
};


struct sensor_present_data
{
    float average;
    float sum;
    int per_hour_cnt;
    //time_t hour_timestamp;

    float  day_min;
    float  day_max;
    //time_t day_timestamp;
    int    per_day_cnt;
};
struct sensor_history_data
{
    struct sensor_data_average data_day[24];
    struct sensor_data_minmax data_week[7];
};

struct indicator_sensor_present_data
{
    struct sensor_present_data temp;
    struct sensor_present_data humidity;
    struct sensor_present_data co2;
    struct sensor_present_data tvoc;
};


struct indicator_sensor_history_data
{
    struct sensor_history_data temp;
    struct sensor_history_data humidity;
    struct sensor_history_data co2;
    struct sensor_history_data tvoc;
};

struct updata_queue_msg
{
    uint8_t flag; //1  day data, 2 week data
    time_t  time;
};

#define SENSOR_HISTORY_DATA_STORAGE  "sensor-data"

static const char *TAG = "sensor-model";

static SemaphoreHandle_t       __g_data_mutex;

static struct indicator_sensor_history_data  __g_sensor_history_data;
static struct indicator_sensor_present_data  __g_sensor_present_data;

static esp_timer_handle_t   sensor_history_data_timer_handle;

static QueueHandle_t updata_queue_handle = NULL;

static void __sensor_history_data_get( struct sensor_history_data  *p_history, struct view_data_sensor_history_data *p_data)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    
    struct sensor_data_average *p_data_day = &p_history->data_day[0];
    struct sensor_data_minmax  *p_data_week = &p_history->data_week[0];

    memcpy(p_data->data_day, (uint8_t *)p_data_day, sizeof( struct sensor_data_average) * 24);
    memcpy(p_data->data_week, (uint8_t *)p_data_week, sizeof( struct sensor_data_minmax) * 7);
    

    //calculate max and min 
    float min=10000;
    float max=-10000;

    for(int i =0; i < 24; i++ ) {
        struct sensor_data_average *p_item = p_data_day + i;
        if( p_item->valid ) {
            if( min > p_item->data ){
                min = p_item->data;
            }
            if( max < p_item->data ){
                max = p_item->data;
            }
        }   
    }
    p_data->day_max = max;
    p_data->day_min = min;

    min=10000;
    max=-10000;

    for(int i =0; i < 7; i++ ) {
        struct sensor_data_minmax *p_item = p_data_week + i;
        if( p_item->valid ) {
            if( min > p_item->min ){
                min = p_item->min;
            }
            if( max < p_item->max ){
                max = p_item->max;
            }
        }
    }

    if ( max < min) {
        max=4;
        min=0;
    }

    p_data->week_max = max;
    p_data->week_min = min;

    xSemaphoreGive(__g_data_mutex);
}

static void __sensor_history_data_save(void)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    esp_err_t ret = 0;
    ret = indicator_storage_write(SENSOR_HISTORY_DATA_STORAGE, (void *)&__g_sensor_history_data, sizeof(struct indicator_sensor_history_data));
    xSemaphoreGive(__g_data_mutex);

    if( ret != ESP_OK ) {
        ESP_LOGI(TAG, "sensor history data save err:%d", ret);
    } else {
        ESP_LOGI(TAG, "sensor history data save successful");
    }   
}

static void __sensor_history_data_day_check(struct sensor_data_average p_data_day[], const char *p_sensor_name, time_t now)
{
    //check history data day
    int history_hour =0;
    int cur_hour = 0;

    history_hour =p_data_day[23].timestamp/3600;
    cur_hour = now/3600;

    for( int i =0;  i < 24; i++) {
        if( p_data_day[i].valid) {
            ESP_LOGI(TAG, "%s index:%d, data:%.0f, time:%d", p_sensor_name, i, p_data_day[i].data, p_data_day[i].timestamp);
        }
    }

    if( history_hour  >  cur_hour) {
        ESP_LOGI(TAG, "%s History day data pull ahead, clear data", p_sensor_name);
        memset(p_data_day, 0, sizeof(struct sensor_data_average)*24);
        return;
    }

    for(int i =0; i < 23; i++ ) {
       int hour1 =p_data_day[i].timestamp/3600;
       int hour2 =p_data_day[i+1].timestamp/3600;
       if( (hour2-hour1) != 1) {
            ESP_LOGI(TAG, "%s History day data error, clear data", p_sensor_name);
            memset(p_data_day, 0, sizeof(struct sensor_data_average)*24);
            return;
       }
    }

    if( history_hour == cur_hour) {
        ESP_LOGI(TAG, "%s History day data valid", p_sensor_name);
        return;
    }

    if( history_hour < ( cur_hour -23) ) {
        ESP_LOGI(TAG, "%s History day data expired, clear data!", p_sensor_name);
        memset(p_data_day, 0, sizeof(struct sensor_data_average)*24);
    } else {

        int overlap_cnt = history_hour - ( cur_hour -23)+1;

        ESP_LOGI(TAG, "%s History day data  %d overlap !", p_sensor_name, overlap_cnt);

        for(int i =0; i < 24; i++ ) {
            if( i < overlap_cnt) {
                p_data_day[i].data = p_data_day[24-overlap_cnt+i].data;
                p_data_day[i].valid = p_data_day[24-overlap_cnt+i].valid;
                p_data_day[i].timestamp = p_data_day[24-overlap_cnt+i].timestamp;
            } else {
                p_data_day[i].data = 0;
                p_data_day[i].valid = false;
                p_data_day[i].timestamp = now - (23 -i) * 3600;;
            }
        }
    }
}
static void __sensor_history_data_week_check(struct sensor_data_minmax p_data_week[], const char *p_sensor_name, time_t now)
{
    //check history data week
    int history_day =0;
    int cur_day = 0;

    history_day =p_data_week[6].timestamp/ (3600 * 24);
    cur_day = now/ (3600 * 24);

    for( int i =0;  i < 7; i++) {
        if( p_data_week[i].valid) {
            ESP_LOGI(TAG, "%s, index:%d, min:%.0f, max:%.0f, time:%d", p_sensor_name, i, p_data_week[i].min, p_data_week[i].max , p_data_week[i].timestamp);
        }
    }

    if( history_day  >  cur_day){
        ESP_LOGI(TAG, "%s History week data pull ahead, clear data", p_sensor_name);
        memset(p_data_week, 0, sizeof(struct sensor_data_minmax)*7);
        return;
    }


    for(int i =0; i < 6; i++ ) {
       int  day1 =p_data_week[i].timestamp/ (3600 * 24);
       int  day2 =p_data_week[i+1].timestamp/ (3600 * 24);
       if( (day2-day1) != 1) {
            ESP_LOGI(TAG, "%s History week data error, clear data", p_sensor_name);
            memset(p_data_week, 0, sizeof(struct sensor_data_minmax)*7);
            return;
       }
    }

    if( history_day  == cur_day){
        ESP_LOGI(TAG, "%s History week data valid", p_sensor_name);
        return;
    }

    if( history_day < ( cur_day -6) ) {
        ESP_LOGI(TAG, "%s History week data expired, clear data!", p_sensor_name);
        memset(p_data_week, 0, sizeof(struct sensor_data_minmax)*7);

    } else {

        int overlap_cnt = history_day - ( cur_day -6)+1;

        ESP_LOGI(TAG, "%s History week data , %d overlap!", p_sensor_name, overlap_cnt);

        for(int i =0; i < 7; i++ ) {
            if( i < overlap_cnt) {
                p_data_week[i].max = p_data_week[7-overlap_cnt+i].max;
                p_data_week[i].min = p_data_week[7-overlap_cnt+i].min;
                p_data_week[i].timestamp = p_data_week[7-overlap_cnt+i].timestamp;
                p_data_week[i].valid = p_data_week[7-overlap_cnt+i].valid;
            } else {

                p_data_week[i].max = 0;
                p_data_week[i].min = 0;
                p_data_week[i].timestamp = now - (6 -i) * 3600 * 24;
                p_data_week[i].valid = false;
            }
        }
    }
}



static void __sensor_history_data_day_insert(struct sensor_data_average p_data_day[],  struct sensor_present_data  *p_cur,  time_t now)
{
    int history_hour =0;
    int cur_hour = 0;
    struct tm timeinfo;

    localtime_r( &now, &timeinfo);
    cur_hour = timeinfo.tm_hour;
    
    localtime_r( &(p_data_day[23].timestamp), &timeinfo);
    history_hour = timeinfo.tm_hour;

    if( cur_hour == history_hour) {
        return;
    }

    for( int i =0;  i < 23; i++) {
        p_data_day[i].data = p_data_day[i+1].data;
        p_data_day[i].valid = p_data_day[i+1].valid;
        p_data_day[i].timestamp = p_data_day[i+1].timestamp;
        
        if( !p_data_day[i].valid) {
            p_data_day[i].timestamp = now - (23 -i) * 3600;
        }
    }
    if( p_cur->per_hour_cnt >=1) {
        p_data_day[23].valid = true;
        p_data_day[23].data = p_cur->average;

        //clear present data 
        p_cur->per_hour_cnt = 0;
        p_cur->sum = 0.0;

    }  else {
        p_data_day[23].valid = false;
    }
    p_data_day[23].timestamp = now;

#if SENSOR_HISTORY_DATA_DEBUG
    for( int i =0;  i < 24; i++) {
        if( p_data_day[i].valid) {
            ESP_LOGI(TAG, "index:%d, data:%.0f, time:%d", i, p_data_day[i].data, p_data_day[i].timestamp);
        }
    }
#endif

}

static void __sensor_history_data_week_insert(struct sensor_data_minmax p_data_week[],  struct sensor_present_data  *p_cur, time_t now)
{
    int history_mday =0;
    int cur_mday = 0;
    struct tm timeinfo;

    localtime_r( &now, &timeinfo);
    cur_mday = timeinfo.tm_mday;
    
    localtime_r( &(p_data_week[6].timestamp), &timeinfo);
    history_mday= timeinfo.tm_mday;

    if( history_mday ==  cur_mday) {
        return;
    }

    for( int i =0;  i < 6; i++) {
        p_data_week[i].max = p_data_week[i+1].max;
        p_data_week[i].min = p_data_week[i+1].min;
        p_data_week[i].timestamp = p_data_week[i+1].timestamp;
        p_data_week[i].valid = p_data_week[i+1].valid;

        if( !p_data_week[i].valid) {
            p_data_week[i].timestamp = now - (6 -i) * 3600 * 24;
        }
    }
    if( p_cur->per_day_cnt >=1) {
        p_data_week[6].valid = true;
        p_data_week[6].max = p_cur->day_max;
        p_data_week[6].min = p_cur->day_min;

        //clear present data
        p_cur->day_max = 0.0;
        p_cur->day_min = 0.0;
        p_cur->per_day_cnt = 0;

    }  else {
        p_data_week[6].valid = false;
    }
    p_data_week[6].timestamp = now;

#if SENSOR_HISTORY_DATA_DEBUG
    for( int i =0;  i < 7; i++) {
        if( p_data_week[i].valid) {
            ESP_LOGI(TAG, "index:%d, min:%.0f, max:%.0f, time:%d", i, p_data_week[i].min, p_data_week[i].max , p_data_week[i].timestamp);
        }
    }
#endif
}


static void __sensor_history_data_check(time_t now)
{
    static bool  check_flag = false;
    if(check_flag) {
        return;
    }
    check_flag =  true;

    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    __sensor_history_data_day_check(__g_sensor_history_data.temp.data_day,   "Temp", now - 3600);
    __sensor_history_data_week_check(__g_sensor_history_data.temp.data_week, "Temp",  now - 3600 * 24);

    __sensor_history_data_day_check(__g_sensor_history_data.humidity.data_day,   "Humidity", now - 3600);
    __sensor_history_data_week_check(__g_sensor_history_data.humidity.data_week, "Humidity",  now - 3600 * 24);

    __sensor_history_data_day_check(__g_sensor_history_data.co2.data_day,   "CO2", now - 3600);
    __sensor_history_data_week_check(__g_sensor_history_data.co2.data_week, "CO2",  now - 3600 * 24);


    __sensor_history_data_day_check(__g_sensor_history_data.tvoc.data_day,   "TVOC", now - 3600);
    __sensor_history_data_week_check(__g_sensor_history_data.tvoc.data_week, "TVOC",  now - 3600 * 24);

    xSemaphoreGive(__g_data_mutex);
}

static void __sensor_history_data_day_update(time_t now)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    
    __sensor_history_data_day_insert( __g_sensor_history_data.temp.data_day, &__g_sensor_present_data.temp, now);

    __sensor_history_data_day_insert( __g_sensor_history_data.humidity.data_day, &__g_sensor_present_data.humidity, now);

    __sensor_history_data_day_insert( __g_sensor_history_data.co2.data_day, &__g_sensor_present_data.co2, now);

    __sensor_history_data_day_insert( __g_sensor_history_data.tvoc.data_day, &__g_sensor_present_data.tvoc, now);

    xSemaphoreGive(__g_data_mutex);

    __sensor_history_data_save();
}

static void __sensor_history_data_week_update(time_t now)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);

    __sensor_history_data_week_insert(__g_sensor_history_data.temp.data_week, &__g_sensor_present_data.temp, now);

    __sensor_history_data_week_insert(__g_sensor_history_data.humidity.data_week, &__g_sensor_present_data.humidity, now);

    __sensor_history_data_week_insert(__g_sensor_history_data.co2.data_week, &__g_sensor_present_data.co2, now);
    
    __sensor_history_data_week_insert(__g_sensor_history_data.tvoc.data_week, &__g_sensor_present_data.tvoc, now);

    xSemaphoreGive(__g_data_mutex);

    __sensor_history_data_save();

}


static void __sensor_history_data_update_callback(void* arg)
{
    static int last_hour = 25;
    static int last_day  = 32;
    static time_t  last_timestamp1 = 0;
    static time_t  last_timestamp2 = 0;
    time_t now = 0;

    now = time(NULL);

    //  todo test
    // static int time_cnt = 1;
    // now += time_cnt * 3600;
    // time_cnt++;

    struct tm timeinfo = { 0 };
    localtime_r( &now, &timeinfo);

    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

    int cur_hour = timeinfo.tm_hour;
    int cur_day  = timeinfo.tm_mday;

    ESP_LOGI(TAG, "__sensor_history_data_update_callback: %s", strftime_buf);

    //if greater than 2020 year mean time is right 
    if( timeinfo.tm_year  < 120) { 
        ESP_LOGI(TAG, "The time is not right!!!");
        return;
    }

    __sensor_history_data_check( now);

    // Hour change and the duration is greater than 1 hour (to prevent hour change during time zone synchronization) 
    // 小时变化并且 时长大于1h （防止在时区同步时, 小时变化的情况）
    if( cur_hour != last_hour  &&  ((now - last_timestamp1) > 3600) ) {
        last_hour = cur_hour;
        
        if( last_timestamp1 == 0) {
            last_timestamp1 = ((now - 3600)/3600) * 3600;
        }
        
        struct updata_queue_msg msg = {
            .flag = 1,
            .time = last_timestamp1,
        };

        xQueueSendFromISR(updata_queue_handle, &msg, NULL);
        //__sensor_history_data_day_update(last_timestamp1);

        last_timestamp1 = ((now)/3600) * 3600; // Sample at the hour
    }

    if( cur_day != last_day  &&  ((now - last_timestamp2) > (3600*24))) {
        last_day = cur_day;
        if( last_timestamp2 == 0) {
            last_timestamp2 = ((now - 3600 * 24) / (3600 * 24)) * (3600 * 24);
        }

        struct updata_queue_msg msg = {
            .flag = 2,
            .time = last_timestamp2,
        };

        xQueueSendFromISR(updata_queue_handle, &msg, NULL);

        //__sensor_history_data_week_update(last_timestamp2);
        
        last_timestamp2 = ((now) / (3600 * 24)) * (3600 * 24); //Sample at the day
    }
}

static __sensor_history_data_update_init(void)
{
    const esp_timer_create_args_t timer_args = {
            .callback = &__sensor_history_data_update_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) sensor_history_data_timer_handle,
            .name = "sensor data update"
    };
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &sensor_history_data_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(sensor_history_data_timer_handle, 1000000 * 10)); //10s

}

static void sensor_history_data_updata_task(void *arg)
{
    struct updata_queue_msg msg = {};
    while(1) {
        if(xQueueReceive(updata_queue_handle, &msg, portMAX_DELAY)) {
            if( msg.flag == 1) {
                ESP_LOGI(TAG, "update day history data");
                __sensor_history_data_day_update(msg.time);
            } else if( msg.flag == 2) {
                ESP_LOGI(TAG, "update week history data");
                __sensor_history_data_week_update(msg.time);
            }
        }

    }
}

static void __sensor_history_data_restore(void)
{
    esp_err_t ret = 0;

    size_t len = sizeof(__g_sensor_history_data);
    
    ret = indicator_storage_read(SENSOR_HISTORY_DATA_STORAGE, (void *)&__g_sensor_history_data, &len);
    if( ret == ESP_OK  && len== (sizeof(__g_sensor_history_data)) ) {
        ESP_LOGI(TAG, "sensor history data read successful");
    } else {
        // err or not find
        if( ret == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "sensor history data not find");
        } else {
            ESP_LOGI(TAG, "sensor history data read err:%d", ret);
        }
        memset(&__g_sensor_history_data, 0 ,sizeof(__g_sensor_history_data));
    }
}


static void __sensor_present_data_update(struct sensor_present_data *p_data, float vaule)
{
    xSemaphoreTake(__g_data_mutex, portMAX_DELAY);
    p_data->per_hour_cnt++;
    p_data->sum += vaule;
    p_data->average = p_data->sum / p_data->per_hour_cnt;


    p_data->per_day_cnt++;
    if( p_data->per_day_cnt != 1) {
        if( p_data->day_min > vaule) {
            p_data->day_min = vaule;
        }
        if( p_data->day_max < vaule) {
            p_data->day_max = vaule;
        }
    } else {
        p_data->day_min = vaule;
        p_data->day_max = vaule;
    }
    xSemaphoreGive(__g_data_mutex);
}

static int __data_parse_handle(uint8_t *p_data, ssize_t len)
{
    uint8_t pkt_type = p_data[0];
    switch ( pkt_type)
    {
        case PKT_TYPE_SENSOR_SCD41_CO2: {
            struct view_data_sensor_data data;
            if( len < (sizeof(data.vaule) +1)) {
                break;
            }
    
            data.sensor_type = SENSOR_DATA_CO2;
            memcpy(&data.vaule, &p_data[1], sizeof(data.vaule));
            __sensor_present_data_update(&__g_sensor_present_data.co2, data.vaule);

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, \
                           &data, sizeof(struct view_data_sensor_data ), portMAX_DELAY);
            break; 
        } 
        
        case PKT_TYPE_SENSOR_SHT41_TEMP: {
            struct view_data_sensor_data data;
            if( len < (sizeof(data.vaule) +1)) {
                break;
            }
    
            data.sensor_type = SENSOR_DATA_TEMP;
            memcpy(&data.vaule, &p_data[1], sizeof(data.vaule));
            __sensor_present_data_update(&__g_sensor_present_data.temp, data.vaule);

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, \
                           &data, sizeof(struct view_data_sensor_data ), portMAX_DELAY);
            break;
        } 

        case PKT_TYPE_SENSOR_SHT41_HUMIDITY: {
            struct view_data_sensor_data data;
            if( len < (sizeof(data.vaule) +1)) {
                break;
            }
        
            data.sensor_type = SENSOR_DATA_HUMIDITY;
            memcpy(&data.vaule, &p_data[1], sizeof(data.vaule));
            __sensor_present_data_update(&__g_sensor_present_data.humidity, data.vaule);

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, \
                           &data, sizeof(struct view_data_sensor_data ), portMAX_DELAY);
            break;
        } 

        case PKT_TYPE_SENSOR_TVOC_INDEX: {
            struct view_data_sensor_data data;
            if( len < (sizeof(data.vaule) +1)) {
                break;
            }
    
            data.sensor_type = SENSOR_DATA_TVOC;
            memcpy(&data.vaule, &p_data[1], sizeof(data.vaule));
            __sensor_present_data_update(&__g_sensor_present_data.tvoc, data.vaule);

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA, \
                           &data, sizeof(struct view_data_sensor_data ), portMAX_DELAY);
            break; 
        } 

        default:
            break;
    }

}

static int __cmd_send(uint8_t cmd, void *p_data, uint8_t len)
{
    uint8_t buf[32] = {0};
    uint8_t data[32] = {0};

    if( len > 31) {
        return -1;
    }
    
    uint8_t index =1;

    data[0] = cmd;

    if( len > 0 && p_data != NULL) {
        memcpy(&data[1], p_data, len);
        index += len;
    }
    cobs_encode_result ret = cobs_encode(buf, sizeof(buf),  data, index);
#if 1//SENSOR_COMM_DEBUG
    ESP_LOGI(TAG, "encode status:%d, len:%d",  ret.status,  ret.out_len);
    for(int i=0; i < ret.out_len; i++ ) {
        printf( "0x%x ", buf[i] );
    }
    printf("\r\n");
#endif

    if( ret.status == COBS_ENCODE_OK ) {
        return uart_write_bytes(ESP32_COMM_PORT_NUM,  buf, ret.out_len+1);
    }
    return -1;
}

static void esp32_rp2040_comm_task(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = ESP32_COMM_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ESP32_COMM_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ESP32_COMM_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ESP32_COMM_PORT_NUM, ESP32_RP2040_TXD, ESP32_RP2040_RXD, ESP32_RP2040_RTS, ESP32_RP2040_CTS));

    __cmd_send(PKT_TYPE_CMD_POWER_ON, NULL, 0);
    cobs_decode_result ret;
    
    while (1) {
        int len = uart_read_bytes(ESP32_COMM_PORT_NUM, buf, (BUF_SIZE - 1), 1 / portTICK_PERIOD_MS);
#if SENSOR_COMM_DEBUG
        ESP_LOGI(TAG, "len:%d",  len);
#endif 
        int index  = 0;
        uint8_t *p_buf_start =  buf;
        uint8_t *p_buf_end = buf;
        if( len > 0 ) {

#if SENSOR_COMM_DEBUG
            printf("recv:  ");
            for(int i=0; i < len; i++ ) {
                printf( "0x%x ", buf[i] );
            }
            printf("\r\n");
#endif 
            while ( p_buf_start < (buf + len)) {
                uint8_t *p_buf_end = p_buf_start;
                while( p_buf_end < (buf + len) ) {
                    if( *p_buf_end == 0x00) {
                        break;
                    }
                    p_buf_end++;
                }
                // decode buf 
                memset(data, 0, sizeof(data));
                ret = cobs_decode(data, sizeof(data),  p_buf_start, p_buf_end - p_buf_start);

#if SENSOR_COMM_DEBUG
                ESP_LOGI(TAG, "decode status:%d, len:%d, type:0x%x  ",  ret.status,  ret.out_len, data[0]);
                printf("decode: ");
                for(int i=0; i < ret.out_len; i++ ) {
                    printf( "0x%x ", data[i] );
                }
                printf("\r\n");
#endif 
                if( ret.out_len > 1  &&  ret.status == COBS_DECODE_OK ) { //todo  ret.status
                    __data_parse_handle((uint8_t *)data, ret.out_len);
                }

                p_buf_start = p_buf_end + 1; // next message
            }
        }
    }
}

static void __sensor_shutdown(void)
{
    int ret = 0;
    ret = __cmd_send(PKT_TYPE_CMD_SHUTDOWN, NULL, 0);
    if( ret <= 0) {
        ESP_LOGI(TAG, "sensor shutdown fail!. %d", ret);
    }
}

static void __view_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    int i;
    switch (id)
    {
        case VIEW_EVENT_SENSOR_TEMP_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_TEMP_HISTORY");
            struct view_data_sensor_history_data data;
            __sensor_history_data_get( &__g_sensor_history_data.temp,  &data);
            data.sensor_type = SENSOR_DATA_TEMP;
            data.resolution  = 1;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }

        case VIEW_EVENT_SENSOR_HUMIDITY_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_HUMIDITY_HISTORY");
            struct view_data_sensor_history_data data;
            __sensor_history_data_get( &__g_sensor_history_data.humidity,  &data);
            data.sensor_type = SENSOR_DATA_HUMIDITY;
            data.resolution  = 0;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_SENSOR_CO2_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_CO2_HISTORY");
            struct view_data_sensor_history_data data;
            __sensor_history_data_get( &__g_sensor_history_data.co2,  &data);
            data.sensor_type = SENSOR_DATA_CO2;
            data.resolution  = 0;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_SENSOR_TVOC_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_TVOC_HISTORY");
            struct view_data_sensor_history_data data;
            __sensor_history_data_get( &__g_sensor_history_data.tvoc,  &data);
            data.sensor_type = SENSOR_DATA_TVOC;
            data.resolution  = 0;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }

        case VIEW_EVENT_SHUTDOWN: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SHUTDOWN");
            __sensor_shutdown();
            break;
        }
//debug ui 
#if 0
        case VIEW_EVENT_SENSOR_TEMP_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_TEMP_HISTORY");
            struct view_data_sensor_history_data data;
            data.sensor_type = SENSOR_DATA_TEMP;
            data.resolution = 1;

            //test
            float min=100;
            float max=0;
            
            time_t now = 0;
            time(&now);
            time_t time1 = (time_t)(now / 3600) * 3600;
            time_t time2 = (time_t)(now / 3600 / 24) * 3600 * 24;
            
            for(i = 0; i < 24; i++) { 
                data.data_day[i].data = (float)lv_rand(10, 40);
                data.data_day[i].timestamp = time1 + i *3600;
                data.data_day[i].valid = true;

                if( min > data.data_day[i].data) {
                    min = data.data_day[i].data;
                }
                if( max < data.data_day[i].data) {
                    max = data.data_day[i].data;
                }
            }
            data.day_max = max;
            data.day_min = min;

            min=100;
            max=0;

            for(i = 0; i < 7; i++) { 
                data.data_week[i].min = (float)lv_rand(10, 20);
                data.data_week[i].max = (float)lv_rand(20, 40);
                data.data_week[i].timestamp =  time2 + i * 3600 * 24;;
                data.data_week[i].valid = true;
                
                if( min > data.data_week[i].min) {
                    min = data.data_week[i].min;
                }
                if( max < data.data_week[i].max) {
                    max = data.data_week[i].max;
                }
            }
            data.week_max = max;
            data.week_min = min;

            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_SENSOR_HUMIDITY_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_HUMIDITY_HISTORY");
            struct view_data_sensor_history_data data;
            data.sensor_type = SENSOR_DATA_HUMIDITY;
            data.resolution = 0;

            //test
            float min=100;
            float max=0;

            time_t now = 0;
            time(&now);
            time_t time1 = (time_t)(now / 3600) * 3600;
            time_t time2 = (time_t)(now / 3600 / 24) * 3600 * 24;
            
            for(i = 0; i < 24; i++) { 
                data.data_day[i].data = (float)lv_rand(30, 50);
                data.data_day[i].timestamp = time1 + i *3600;
                data.data_day[i].valid = true;
                if( min > data.data_day[i].data) {
                    min = data.data_day[i].data;
                }
                if( max < data.data_day[i].data) {
                    max = data.data_day[i].data;
                }
            }
            data.day_max = max;
            data.day_min = min;

            min=100;
            max=0;
            for(i = 0; i < 7; i++) { 
                data.data_week[i].max = (float)lv_rand(40, 50);
                data.data_week[i].min = (float)lv_rand(30, 40);
                data.data_week[i].timestamp =  time2 + i * 3600 * 24;;
                data.data_week[i].valid = true;
                if( min > data.data_week[i].min) {
                    min = data.data_week[i].min;
                }
                if( max < data.data_week[i].max) {
                    max = data.data_week[i].max;
                }
            }
            data.week_max = max;
            data.week_min = min;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_SENSOR_TVOC_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_TVOC_HISTORY");
            struct view_data_sensor_history_data data;
            data.sensor_type = SENSOR_DATA_TVOC;
            data.resolution = 0;

            //test
            float min=120;
            float max=0;

            time_t now = 0;
            time(&now);
            time_t time1 = (time_t)(now / 3600) * 3600;
            time_t time2 = (time_t)(now / 3600 / 24) * 3600 * 24;
            
            for(i = 0; i < 24; i++) { 
                data.data_day[i].data = (float)lv_rand(100, 120);
                data.data_day[i].timestamp = time1 + i *3600;
                data.data_day[i].valid = true;
                if( min > data.data_day[i].data) {
                    min = data.data_day[i].data;
                }
                if( max < data.data_day[i].data) {
                    max = data.data_day[i].data;
                }
            }
            data.day_max = max;
            data.day_min = min;

            min=120;
            max=0;
            for(i = 0; i < 7; i++) { 
                data.data_week[i].max = (float)lv_rand(100, 120);
                data.data_week[i].min = (float)lv_rand(0, 100);
                data.data_week[i].timestamp =  time2 + i * 3600 * 24;;
                data.data_week[i].valid = true;
                if( min > data.data_week[i].min) {
                    min = data.data_week[i].min;
                }
                if( max < data.data_week[i].max) {
                    max = data.data_week[i].max;
                }
            }
            data.week_max = max;
            data.week_min = min;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
        case VIEW_EVENT_SENSOR_CO2_HISTORY: {
            ESP_LOGI(TAG, "event: VIEW_EVENT_SENSOR_CO2_HISTORY");
            struct view_data_sensor_history_data data;
            data.sensor_type = SENSOR_DATA_CO2;
            data.resolution = 0;

            //test
            float min=700;
            float max=0;

            time_t now = 0;
            time(&now);
            time_t time1 = (time_t)(now / 3600) * 3600;
            time_t time2 = (time_t)(now / 3600 / 24) * 3600 * 24;
            
            for(i = 0; i < 24; i++) { 
                data.data_day[i].data = (float)lv_rand(500, 600);
                data.data_day[i].timestamp = time1 + i *3600;
                data.data_day[i].valid = true;
                if( min > data.data_day[i].data) {
                    min = data.data_day[i].data;
                }
                if( max < data.data_day[i].data) {
                    max = data.data_day[i].data;
                }
            }
            data.day_max = max;
            data.day_min = min;

            min=700;
            max=0;
            for(i = 0; i < 7; i++) { 
                data.data_week[i].min = (float)lv_rand(500, 600);
                data.data_week[i].max = (float)lv_rand(600, 700);
                data.data_week[i].timestamp =  time2 + i * 3600 * 24;;
                data.data_week[i].valid = true;
                if( min > data.data_week[i].min) {
                    min = data.data_week[i].min;
                }
                if( max < data.data_week[i].max) {
                    max = data.data_week[i].max;
                }
            }
            data.week_max = max;
            data.week_min = min;
            esp_event_post_to(view_event_handle, VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_DATA_HISTORY, &data, sizeof(struct view_data_sensor_history_data ), portMAX_DELAY);
            break;
        }
#endif

    default:
        break;
    }
}

int indicator_sensor_init(void)
{
    __g_data_mutex  =  xSemaphoreCreateMutex();

    updata_queue_handle = xQueueCreate(4, sizeof( struct updata_queue_msg));

    __sensor_history_data_restore();
    
    __sensor_history_data_update_init();

    xTaskCreate(esp32_rp2040_comm_task, "esp32_rp2040_comm_task", ESP32_RP2040_COMM_TASK_STACK_SIZE, NULL, 2, NULL);

    xTaskCreate(sensor_history_data_updata_task, "sensor_history_data_updata_task", 1024*4, NULL, 6, NULL);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_TEMP_HISTORY, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_HUMIDITY_HISTORY, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_TVOC_HISTORY, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SENSOR_CO2_HISTORY, 
                                                            __view_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle, 
                                                            VIEW_EVENT_BASE, VIEW_EVENT_SHUTDOWN, 
                                                            __view_event_handler, NULL, NULL));
}


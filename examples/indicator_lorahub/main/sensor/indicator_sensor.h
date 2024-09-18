#ifndef SENSOR_H
#define SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>
#define SENSOR_OK               0
#define SENSOR_ERR_INVALID_TYPE -1
#define SENSOR_ERR_MUTEX_FAIL   -2

// 添加传感器类型
#define SENSOR_TYPE_LIST           \
    X(SENSOR_CO2, "CO2")           \
    X(SENSOR_TVOC, "TVOC")         \
    X(SENSOR_HUMIDITY, "Humidity") \
    X(SENSOR_TEMP, "Temperature")  \
    // 可以继续添加其他传感器类型

// 定义传感器类型
#define X(type, str) type,
typedef enum {
    SENSOR_TYPE_LIST
        ENUM_SENSOR_ALL
} SensorType;
#undef X

extern const char *SensorTypeStrings[];

typedef enum {
    SENSOR_STATUS_OK,
    SENSOR_STATUS_INITED,
    SENSOR_STATUS_WARNING,
    SENSOR_STATUS_ERROR,
} SensorStatus;

// 定义 SensorData 结构
typedef struct {
    float        value;     // 传感器的测量值
    SensorType   type;      // 传感器的类型
    SensorStatus status;    // 传感器的当前状态
    bool         timeValid; // 时间戳是否有效
    uint64_t     timeStamp; // 传感器数据的时间戳
} SensorData;

char *getSensorName(const SensorType type);
float getSensorFloatValue(const SensorType type);
int   getSensorIntValue(const SensorType type);

SensorData *get_current_sensor_data(SensorData *data, SensorType type);

int  UpdateSensorData(const SensorType type, uint8_t *p_data);
void indicator_sensor_init(void);

/* View */
void view_sensor_init();
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SENSOR_H*/
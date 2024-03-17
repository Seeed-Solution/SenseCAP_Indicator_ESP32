#include "app_keypoints.h"

void draw_one_point(lv_obj_t* parent, const keypoint_t point, lv_color_t color) {}

#include "cJSON.h"
#include "esp_log.h"
#include <stdlib.h>

#define TAG "KEYPOINTS_PROCESS"

// 创建关键点数组
keypoint_t* create_keypoints_array(size_t count) {
    return (keypoint_t*)malloc(sizeof(keypoint_t) * count);
}

// 释放关键点数组
void free_keypoints_array(keypoint_t* array) {
    free(array);
}

// 初始化keypoints_t结构体
void init_keypoints(keypoints_t* kp, size_t count) {
    kp->points = create_keypoints_array(count);
    kp->points_count = count;
    // 此处可以初始化boxes_t等其他成员
}

// 释放keypoints_t结构体
void free_keypoints(keypoints_t* kp) {
    free_keypoints_array(kp->points);
    // 如果有其他需要释放的资源，也在这里处理
}

void print_keypoints(const keypoints_t* keypoints, size_t count) {
    for (size_t i = 0; i < count; i++) {
        const keypoints_t* kp = &keypoints[i];

        // 打印boxes_t信息，根据你的定义调整
        ESP_LOGI(TAG, "Box: [%d, %d, %d, %d, %d, %d]", kp->box.x, kp->box.y, kp->box.w, kp->box.h, kp->box.score,
                 kp->box.target);
        // 打印所有关键点
        for (size_t j = 0; j < kp->points_count; j++) {
            const keypoint_t* point = &kp->points[j];
            ESP_LOGI(TAG, "Point: [%d, %d]", point->x, point->y);
        }
    }
}

// void process_and_draw_keypoints(cJSON* receivedJson, lv_obj_t* canvas) {
//     cJSON* keypoints_json = cJSON_GetObjectItem(receivedJson, "keypoints");
//     if (!cJSON_IsArray(keypoints_json)) {
//         // ESP_LOGE(TAG, "keypoints is not an array");
//         return;
//     }

//     size_t keypoints_count = cJSON_GetArraySize(keypoints_json); // 有多少个人
//     keypoints_t* keypoints_array = (keypoints_t*)malloc(sizeof(keypoints_t) * keypoints_count);

//     int index = 0;
//     cJSON* keypoint_json;
//     cJSON_ArrayForEach(keypoint_json, keypoints_json) {
//         keypoints_t* current_keypoint = &keypoints_array[index++];
//         cJSON* box_json = cJSON_GetObjectItem(keypoint_json, "box");
//         if (cJSON_IsArray(box_json)) {
//             // 填充boxes_t数据
//             // 注意，这里需要你根据你的boxes_t结构体来适配这部分代码
//             for (int i = 0; i < cJSON_GetArraySize(box_json); i++) {
//                 current_keypoint->box.boxArray[i] = cJSON_GetArrayItem(box_json, i)->valueint;
//             }
//         }
//         draw_one_box(canvas, current_keypoint->box, lv_color_make(113, 235, 52));
//         cJSON* points_json = cJSON_GetObjectItem(keypoint_json, "points");
//         if (cJSON_IsArray(points_json)) {
//             size_t points_count = cJSON_GetArraySize(points_json);
//             current_keypoint->points = create_keypoints_array(points_count);
//             current_keypoint->points_count = points_count;

//             int point_index = 0;
//             cJSON* point_json;
//             cJSON_ArrayForEach(point_json, points_json) {
//                 if (point_index < points_count) {
//                     current_keypoint->points[point_index].x = cJSON_GetArrayItem(point_json, 0)->valueint;
//                     current_keypoint->points[point_index].y = cJSON_GetArrayItem(point_json, 1)->valueint;
//                     point_index++;
//                 }
//             }
//             lv_port_sem_take();
//             draw_keypoints(canvas, current_keypoint);
//             lv_port_sem_give();
//         }
//     }

//     // 你可以在这里添加调用绘制函数的代码，使用keypoints_array中的数据进行绘制
//     // print_keypoints(keypoints_array, keypoints_count);

//     // draw_one_box(canvas, keypoints_array[0].box, lv_color_make(113, 235, 52));
//     // draw_keypoints(canvas, keypoints_array, keypoints_count); // boxes and keypoints

//     // 清理
//     for (int i = 0; i < keypoints_count; i++) {
//         free_keypoints_array(keypoints_array[i].points);
//     }
//     free(keypoints_array);
// }

// bool get_keypoints(cJSON* receivedJson, keypoints_t** keypoints_array, int* keypoints_count) {
//     cJSON* keypoints_json = cJSON_GetObjectItem(receivedJson, "keypoints");
//     if (!cJSON_IsArray(keypoints_json)) {
//         // ESP_LOGE(TAG, "keypoints is not an array");
//         return false;
//     }

//     int count = cJSON_GetArraySize(keypoints_json); // 有多少个人
//     *keypoints_array = (keypoints_t*)malloc(sizeof(keypoints_t) * count);
//     if (*keypoints_array == NULL) {
//         ESP_LOGE(TAG, "Failed to allocate memory for keypoints_array");
//         return false;
//     }
//     *keypoints_count = count;

//     int index = 0;
//     cJSON* keypoint_json;
//     cJSON_ArrayForEach(keypoint_json, keypoints_json) {
//         keypoints_t* current_keypoint = &(*keypoints_array)[index++];
//         cJSON* box_json = cJSON_GetObjectItem(keypoint_json, "box");
//         if (cJSON_IsArray(box_json)) {
//             // 填充boxes_t数据
//             // 注意，这里需要你根据你的boxes_t结构体来适配这部分代码
//             int boxes_count = cJSON_GetArraySize(box_json);
//             for (int i = 0; i < boxes_count; i++) {
//                 current_keypoint->box.boxArray[i] = cJSON_GetArrayItem(box_json, i)->valueint;
//             }
//         }
//         // draw_one_box(canvas, current_keypoint->box, lv_color_make(113, 235, 52));

//         cJSON* points_json = cJSON_GetObjectItem(keypoint_json, "points");
//         if (cJSON_IsArray(points_json)) {
//             size_t points_count = cJSON_GetArraySize(points_json);
//             current_keypoint->points = create_keypoints_array(points_count);
//             current_keypoint->points_count = points_count;

//             int point_index = 0;
//             cJSON* point_json;
//             cJSON_ArrayForEach(point_json, points_json) {
//                 if (point_index < points_count) {
//                     current_keypoint->points[point_index].x = cJSON_GetArrayItem(point_json, 0)->valueint;
//                     current_keypoint->points[point_index].y = cJSON_GetArrayItem(point_json, 1)->valueint;
//                     point_index++;
//                 }
//             }
//         }
//     }
//     return true;
// }
bool get_keypoints(cJSON* keypoints_json, keypoints_t** keypoints_array, int* keypoints_count) {


    int count = cJSON_GetArraySize(keypoints_json);
    if (count <= 0) {
        ESP_LOGE(TAG, "No keypoints found");
        return false;
    }

    *keypoints_array = (keypoints_t*)malloc(sizeof(keypoints_t) * count);
    if (*keypoints_array == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for keypoints_array");
        return false;
    }
    memset(*keypoints_array, 0, sizeof(keypoints_t) * count); // 初始化内存
    *keypoints_count = count;

    for (int index = 0; index < count; index++) {
        cJSON* keypoint_json = cJSON_GetArrayItem(keypoints_json, index);
        keypoints_t* current_keypoint = &(*keypoints_array)[index];

        cJSON* box_json = cJSON_GetObjectItem(keypoint_json, "box");
        if (cJSON_IsArray(box_json) && cJSON_GetArraySize(box_json) == 6) { // 假设box有4个整数值
            for (int i = 0; i < 4; i++) {
                cJSON* item = cJSON_GetArrayItem(box_json, i);
                if (item)
                    current_keypoint->box.boxArray[i] = item->valueint;
            }
        }

        cJSON* points_json = cJSON_GetObjectItem(keypoint_json, "points");
        if (cJSON_IsArray(points_json)) {
            size_t points_count = cJSON_GetArraySize(points_json);
            if (points_count > 0) {
                current_keypoint->points = create_keypoints_array(points_count);
                current_keypoint->points_count = points_count;

                for (int point_index = 0; point_index < points_count; point_index++) {
                    cJSON* point = cJSON_GetArrayItem(points_json, point_index);
                    if (point && cJSON_GetArraySize(point) == 2) { // 假设每个点是一个有两个整数值的数组
                        current_keypoint->points[point_index].x = cJSON_GetArrayItem(point, 0)->valueint;
                        current_keypoint->points[point_index].y = cJSON_GetArrayItem(point, 1)->valueint;
                    }
                }
            }
        }
    }
    return true;
}


static lv_draw_line_dsc_t line_dsc;
static lv_draw_rect_dsc_t rect_dsc;
// 人体骨骼连接关系，每对数字代表需要连接的关键点索引
static const int skeleton_num[17][2] = {{0, 1}, {0, 2},  {1, 3},   {2, 4},   {3, 5},   {4, 6},
                                        {5, 6}, {5, 11}, {6, 12},  {11, 12}, {5, 7},   {7, 9},
                                        {6, 8}, {8, 10}, {11, 13}, {13, 15}, {12, 14}, {14, 16}};

void draw_keypoints(lv_obj_t* canvas, const keypoints_t* keypoints) {

    // 定义颜色
    const lv_color_t color_head = lv_color_make(255, 0, 0); // 红色，头部
    const lv_color_t color_body = lv_color_make(0, 255, 0); // 绿色，身体
    const lv_color_t color_legs = lv_color_make(0, 0, 255); // 蓝色，腿部

    keypoints_t* kp = (keypoints_t*)keypoints;

    // 绘制关键点
    switch (kp->points_count) {
    case 17: {
        for (int idx = 0; idx < 17; idx++) { // pints.length == 17
            int x_sta = kp->points[skeleton_num[idx][0]].x;
            int y_sta = kp->points[skeleton_num[idx][0]].y;

            int x_end = kp->points[skeleton_num[idx][1]].x;
            int y_end = kp->points[skeleton_num[idx][1]].y;

            // 根据连接关系选择颜色
            if (idx == 0 || idx == 1 || idx == 2 || idx == 3) { // 0,1,2,3,4
                line_dsc.color = color_head;                    // 头部
            } else if (idx >= 5 && idx <= 12) {
                line_dsc.color = color_body; // 身体
            } else {
                line_dsc.color = color_legs; // 腿部
            }

            lv_point_t points[2] = {{x_sta, y_sta}, {x_end, y_end}};
            lv_canvas_draw_line(canvas, points, 2, &line_dsc);
        }

        for (int idx = 0; idx < 17; idx++) {
            int x_sta = kp->points[idx].x;
            int y_sta = kp->points[idx].y;

            lv_canvas_draw_rect(canvas, x_sta, y_sta, 2, 2, &rect_dsc);
        }
        // lv_point_t points_left_eyes[2] = {{x_sta, y_sta}, {x_end, y_end}};
        // lv_canvas_draw_line(canvas, points, 2, &line_dsc);

    } break;
    case 21: {
        ESP_LOGI(TAG, "21 keypoints");
    } break;
    default:
        break;
    }

    // for (size_t j = 0; j < kp->points_count; j++) {
    //     const keypoint_t* point = &kp->points[j];
    //     draw_one_point(parent, *point, lv_color_make(113, 235, 52));
    // }
}
/**
 * @brief 绘制所有一张图片中 People 的关键点
 *
 * @param parent
 * @param keypoints
 * @param count
 */
void draw_keypoints_array(lv_obj_t* canvas, const keypoints_t* keypoints, size_t count) {
    for (size_t i = 0; i < count; i++) { // 遍历所有人
        draw_keypoints(canvas, &keypoints[i]);
    }
}

void init_keypoints_app() {

    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_palette_main(LV_PALETTE_RED);
    line_dsc.width = 4;
    line_dsc.round_end = 1;
    line_dsc.round_start = 1;

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_color = lv_palette_main(LV_PALETTE_PURPLE);
    rect_dsc.border_width = 2;
    rect_dsc.radius = 1;
}
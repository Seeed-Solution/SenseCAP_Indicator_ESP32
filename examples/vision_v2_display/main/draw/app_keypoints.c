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
            ESP_LOGI(TAG, "Point[%d]: [%d, %d]", j, point->x, point->y);
        }
    }
}

bool ParseJsonKeypoints(cJSON* keypoints_json, keypoints_t** keypoints_array, int* keypoints_count) {

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
    memset(*keypoints_array, 0, sizeof(keypoints_t) * count); // Initializes memory
    *keypoints_count = count;

    for (int index = 0; index < count; index++) { // Every keypoint
        cJSON* keypointJson = cJSON_GetArrayItem(keypoints_json, index);
        keypoints_t* current_keypoint = &(*keypoints_array)[index];

        cJSON* boxJson = cJSON_GetObjectItem(keypointJson, "box");
        if (cJSON_IsArray(boxJson) && cJSON_GetArraySize(boxJson) == 6) {
            for (int i = 0; i < 6; i++) { // x, y, w, h, score, target
                cJSON* item = cJSON_GetArrayItem(boxJson, i);
                if (item)
                    current_keypoint->box.boxArray[i] = item->valueint;
            }
        }

        cJSON* pointsJson = cJSON_GetObjectItem(keypointJson, "points");
        if (cJSON_IsArray(pointsJson)) {
            size_t points_count = cJSON_GetArraySize(pointsJson);
            if (points_count > 0) {
                current_keypoint->points = create_keypoints_array(points_count);
                current_keypoint->points_count = points_count;

                for (int point_index = 0; point_index < points_count; point_index++) {
                    cJSON* point = cJSON_GetArrayItem(pointsJson, point_index);
                    // size_t point_size = cJSON_GetArraySize(point);
                    // for (int i = 0; i < 2; i++) {
                    // for (int i = 0; i < 4; i++) {
                    for (int i = 0; i < 3; i++) {
                        cJSON* item = cJSON_GetArrayItem(point, i);
                        if (item) {
                            // ESP_LOGI(TAG, "point[%d]: %d", i, item->valueint);
                            current_keypoint->points[point_index].point[i] = item->valueint;
                        }
                    }
                }
            }
        }
    }
    return true;
}

static lv_draw_line_dsc_t line_dsc;
static lv_draw_rect_dsc_t rect_dsc;
// 人体骨骼连接关系，每对数字代表需要连接的关键点索引, 并按顺序画线
static const int skeleton_num[][2] = {
    // 鼻子 -> 左眼 -> 左耳
    {0, 1}, // nose to left eye
    {1, 3}, // left eye to left ear

    // 鼻子 -> 右眼 -> 右耳
    {0, 2}, // nose to right eye
    {2, 4}, // right eye to right ear

    // 鼻子 -> 脖子（虚拟点，通常是左肩和右肩中点）-> 中脊柱（虚拟点，位于左髋和右髋中点）
    {0, 5},  // nose to neck
    {5, 11}, // neck to mid spine

    // 左肩 -> 左肘 -> 左手腕
    {5, 7}, // left shoulder to left elbow
    {5, 7}, // left shoulder to left elbow
    {7, 9}, // left elbow to left wrist

    {5, 6}, // left shoulder to right shoulder

    // 右肩 -> 右肘 -> 右手腕
    {6, 8},  // right shoulder to right elbow
    {8, 10}, // right elbow to right wrist

    // 左髋 -> 左膝 -> 左脚踝
    {11, 13}, // left hip to left knee
    {13, 15}, // left knee to left ankle

    // 右髋 -> 右膝 -> 右脚踝
    {12, 14}, // right hip to right knee
    {14, 16}, // right knee to right ankle
};

const int limit_score = 5; // 限制关键点的最低分数
// keypoints is not keypoints_array
void draw_keypoints(lv_obj_t* canvas, const keypoints_t* keypoints) {

    // 定义颜色
    const lv_color_t color_head = lv_color_make(255, 0, 0); // 红色，头部
    const lv_color_t color_body = lv_color_make(0, 255, 0); // 绿色，身体
    const lv_color_t color_legs = lv_color_make(0, 0, 255); // 蓝色，腿部

    keypoints_t* kp = (keypoints_t*)keypoints;

    if (kp->points_count != 17) { // Posture Detection
        ESP_LOGI(TAG, "keypoints count is not 17, %d", kp->points_count);
        return;
    }

    // draw points
    for (int i = 0; i < kp->points_count; i++) {
        const keypoint_t* point = &kp->points[i];
        lv_point_t rect_point = {point->x - 1, point->y - 1};
        // ESP_LOGI("Draw Points", "point[%d]: %d, %d", i, point->x, point->y);
        lv_canvas_draw_rect(canvas, point->x, point->y, 6, 6, &rect_dsc);
    }

    // draw lines
    for (int idx = 0; idx < sizeof(skeleton_num) / sizeof(skeleton_num[0]); idx++) {
        // target 就是 skeleton_num 每一对的索引
        int start_point = skeleton_num[idx][0];
        int end_point = skeleton_num[idx][1];

        // if (kp->points[start_point].score < limit_score || kp->points[end_point].score < limit_score) {
        //     continue;
        // }

        lv_point_t line_points[2] = {{kp->points[start_point].x, kp->points[start_point].y},
                                     {kp->points[end_point].x, kp->points[end_point].y}};

        // 根据连接关系选择颜色
        if (start_point == 0 || end_point == 0) // 鼻子连接线
            line_dsc.color = color_head;
        else if ((start_point >= 5 && start_point <= 6) || (end_point >= 5 && end_point <= 6)) // 身体连接线
            line_dsc.color = color_body;
        else // 腿部连接线
            line_dsc.color = color_legs;

        // if ((start_point == 5 || start_point == 6) && (end_point == 11 || end_point == 12)) { // Shoulder to hip
        if (start_point == 5 && end_point == 11) { // Shoulder to hip
            line_points[0].x = (kp->points[5].x + kp->points[6].x) / 2;
            line_points[0].y = (kp->points[5].y + kp->points[6].y) / 2;
            line_points[1].x = (kp->points[11].x + kp->points[12].x) / 2;
            line_points[1].y = (kp->points[11].y + kp->points[12].y) / 2;
        } else if (start_point == 0 && end_point == 5) { // Nose to neck
            line_points[1].x = (kp->points[5].x + kp->points[6].x) / 2;
            line_points[1].y = (kp->points[5].y + kp->points[6].y) / 2;
        } else {
        }
        lv_canvas_draw_line(canvas, line_points, 2, &line_dsc);
    }
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
    line_dsc.width = 2;
    line_dsc.round_end = 1;
    line_dsc.round_start = 1;

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_color = lv_palette_main(LV_PALETTE_PURPLE);
    rect_dsc.border_width = 1;
    rect_dsc.radius = 4;
}
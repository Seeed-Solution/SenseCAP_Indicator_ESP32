#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "lv_port.h"
#include "nvs_flash.h"


#include "lv_port_fs.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>


static const char *TAG = "app_main";

#define VERSION   "v1.0.0"

#define FILES_CNT_MAX   20

static char file_names[FILES_CNT_MAX][32];

int get_file_list(char * path)
{
    int index = 0 ;
    DIR * dir = opendir(path);
    struct dirent *entry;
    
    printf("--- list ---\r\n");
    if( dir) {
        while(( entry = readdir(dir)) != NULL) {

            printf("%s\r\n", entry->d_name);
            strcpy(&file_names[index][0], entry->d_name);
            index++;
            if(index >= FILES_CNT_MAX ) {
                break;
            } 
        }
        closedir(dir);
    }
    return index;
}



void app_main(void)
{
    ESP_LOGI("", "Version: %s %s %s", VERSION, __DATE__, __TIME__);

    ESP_ERROR_CHECK(bsp_board_init());
    
    lv_port_init();
    lv_port_fs_init();
    
    int cnt = get_file_list("/spiffs/");
    char file_path[48];

    lv_obj_t * img = lv_img_create(lv_scr_act());
    while (1) {
        for(int i = 0; i < cnt; i++ ) {
            ESP_LOGI(""," Display file:%s...", (char*) &file_names[i][0] );
            memset(file_path, 0, sizeof(file_path));
            sprintf(file_path, "S:%s", (char*) &file_names[i][0]);
            lv_img_set_src(img, (char*) file_path);  //eg: S:test.png.
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

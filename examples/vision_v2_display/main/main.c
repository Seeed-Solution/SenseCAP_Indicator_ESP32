// Copyright 2015-2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "bsp_board.h"
#include "lv_port.h"
#include "ui/ui.h"

#include "main.h"
#include "esp32_rp2040.h"


#define VERSION "v1.0.0"
#define LOG_MEM_INFO 0
#define SENSECAP "\n\
   _____                      _________    ____         \n\
  / ___/___  ____  ________  / ____/   |  / __ \\       \n\
  \\__ \\/ _ \\/ __ \\/ ___/ _ \\/ /   / /| | / /_/ /   \n\
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/         \n\
/____/\\___/_/ /_/____/\\___/\\____/_/  |_/_/           \n\
--------------------------------------------------------\n\
 Version: %s %s %s\n\
--------------------------------------------------------\n\
"


static const char *TAG = "app_main";

// const unsigned char base64_jpeg[] = 
//"/9j/4AAQSkZJRgABAQEASABIAAD/2wBDAA0JCgsKCA0LCgsODg0PEyAVExISEyccHhcgLikxMC4pLSwzOko+MzZGNywtQFdBRkxOUlNSMj5aYVpQYEpRUk//2wBDAQ4ODhMREyYVFSZPNS01T09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT09PT0//wAARCADwAPADASEAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwDirqQuy5HIHNQUALXT6Nps9xpAbyC0DnLHOMkHj9aqKuSzr7e/kvVbTdeYQTQrkzdfNB56DAHGK19KsbC4s4pntx5gGCxJ5x3/AEp6oDYfHksqntivPNc8LzXd40ocjPtn096SVxswpPCt4nfP4D296oS6Jex/8s8/iPam4hcpSWdxGfniIqAg+lTYC7bSyABQvFWZWITJoYFX3qvM3FQMrdTTl6CrEWbdMmr68LVITJFGasJVEgxHrUZPNACGkxQApooEKRRjmgDHvOJfwqAVkaigV2Ok61NYaHDEu1UibOf73J9qpOwjTj+2+NLwSyKI7OE4RB/PNdva2P2e2SEP90dcUbIOuhIYH7GmNFKO1IZXlVsfMtUbiOLacx/rRdhYwdUW3ERwuDWCLKFjuIouIkW1hXp1FZl8oRiBRIaKDcCqczckCoQDBTkGWqxF6BeKsjnvVIRYiTjNTdBVEjGphoGJ2pcUCA9aXFAC9qD0oEYty25w1QiskajhV6RztRT90CmI9D+HEmdPkTbgA9cdevtXa0MEFFIYYFRPbxOMMtAGTqOkW0ifMMfn7e9clf28cLkKeBVdBWMcSHzG5rPvX3PioYyjK2Fqkxy3tQgHVJCvNUhF5BxVmJcmqEW1GBSMeelUIjY8Ug60gFx+VHamIKXH50CA4xSHmmBmS2M7FPKiY5FCaVfP0gPPuP8APesOZGpYi0LUnIxbHn3Ht/jV5fD2pykH7OQPqPb/ABpe0igtc9F8G6XJpujoJ1CyvyR6V0FW3cSCikMKKAKV9NGqENzXC61MpkbbTvoOxz+Cq88ZrPmOZCahgU5296rd6aEx461bhXjpVoTLUYJNX4EwvSqRI5jjimFqAGZzQKAHDpR1piF2jtR05oAD19qQjjNAHoW3Toxw4yP9n/PpSpdaeg5cf98/59K4uSPVnUqcuxZivbBnCRfMx6fKf89q1Y44wBiMCrjTg9RTi47k1FamQUUAFB6UAc/q6zLnaTXD3xkM530WGZ90+Bis5z8pqWBQmOTioxVITJoly1XYxgVSJZbt0yavAYFWIjfOaibJFIBAOM1IBTAKcoyKBDiBxxTGX0oATbinYoEUf7QnOaUX81c/Ijs9uzs/AURuJ3uZRnsvH1rvqtKysZTm5u7CigkKKACigCrfRGSLjrXB6zA0cjFqroByty+XIqnM2AazGUWOTQBVCLcCcA1ciTc3FWiWX4o9oqRsYpiIabigBV+lSYpiEIpyigBxFIRQIRuKaeKAMqxKO7LN06ire22HQbvzrM0O88ChVtRsTb/k11D3JBIHFKTHET7Xx0o+2D0qbjF+2p3FKbyOi4WD7ZF60v2uL1p3CwG8hA+9XI+KrmIo20j86dxWPP5W3SMRVO4bjFSMrDrUiDcw4q0SXok9q0LaPoatEstE4U8VEzc0AMowaEA4A5p447UxDlUEUpUigBDkGkJNAhpJxTWJxQBXNgtvCt0h3KR93H4VIokkGUj7Vmtrmnkd14KR1tRvXHX+tbkw+alIcSBsimHNQUNLGo3cgUAQPKR3qvLcFe9IClNfOqnDVzWrXzzsRmhAZLdDVOQbqpEkQGDVqBOc1aEzRt4skVeUBVqyQcnFQk5NAAKdjjpQIeKdjnOKYEi8ClbpzQIYRSMO4oAY+MVE560DNPQ7iRNKie3IFwfkBK56nHcVsnwhqF0ytPd7QR02j296lLuabnReH9FOkwBWm3n/AHcVbnQbyPeola+gLzIGTiomWoGRlahk4oGVpDVC4PXFIDKvHIQmubnJaU0IRBMcLiqZY561aEKnJq/bx9MVaJZqQIAOnNSyHHWqERbuKZmgByjvTsUCHqKeFzTAcv6Up6UANPWmPQBC5xUDNkUgNTwSpnuo4CPlVs9PrXq6jgVL2NEOqCWMs2ahgRm3Y9qjNu2OlKwyB4j6VUmTFIZSmHFZ0/ekBlXqkxtXOuPnOaYinctVcZNUhMmgTc1atumB0rREsuocCmStQIg3U4dPamBKop4HFAiQCloAVTilPSmBG3WmOaQEEpzULHikB3nhvQodO1PMLbgRyMdOK7KlI0QUtSAUUAFMeGN/vLmgCrNplvKOF2/nXOavprW0ZIP+ePek0By1zKfLIPWsOTqWqRmfMcsaYBVoRdtlwKvxnsKtEsnDYWoHbmgQijmp1HtTQEiinUCHDpmg8UwFpTknigCKTINRO3rSAhc570w8UhnpHhK9/tOI3fk+Vnouc4698V01Sy/QWikAUUAFFACEgDJNcx4kuiUZU6VLGjgLyUsxFUJjtSkBnHls0+NctVoll1BircAwMmrRLHyntUJ60APSrCimIkA/+vTgMmgBwWlxzzTAGAppODigCJzk1Xfk1IyNulNbkc0Ael+CIPJ0WPIwSK6SpZYtFIAooAazhahe5x0qWx2KNzdEgjNc/qb5iYk0gOJuPmnaqV1kjAoEUwpzzU8K8/WtEJltRkgVZX5VqyRrHNIoyaQEiDmrCjApoQ8L3p468UALSkgigBrGombJoAiduajY80hjG5+lRvwKAPYNEtWtNNjhcYZRzWhUy3KQUjcCkMjLmmmTqM1BRE7ZqCQ0gKFw3Wuf1aQrC1MDlCclm9azrhyZOKFuSM28VahXitEJlqId6cxx9KoRGTzT0HrQImQVMtMRKBxS/hTATPrQTikBEz4qJmOaTGQFuaTdmgBCaYxoA9wAwKWoLCkbpQBEQSOlMMZ9KgojdT6YqvJmiwjPuPXmue1gAwE5oA5iXCxmszG6TNOIicocDFWY4/lFVDYTJRwvWmNVkgoqVBQBOop6jnFMRKOlGOeKYAT2qJzikBCxyc1Ex5pMZEfek70AGcHHrSHmgD23z4/736UedH61z+1ia8rFEqHoacCD0pxqxk7ITTQtFaCCjA9KAKl99nit3eUAcV51qN0r71U8VMmNIwL5tqYqjAu5qUdgZoJEMZqUgAcVpDYl7kZNNpiHKOamVaYiYDPBp4A6UxEo6Y7U3PpQA0njioWPPNDAjPNRkikMiIoyKAEPrTSeKQHqiXdseqD86nSWzPUY/A1k6cGb6otRLbv9x/0q2iBFwKmNFRd0S22OorUkKaXVeppXAo6jNA9s6tzxXmd3j7RKFHGamTHYwbrczH0qSzTNH2Q6mnKmxBUBNVT2JluJigAk8VZJPHCx7VOIGHalzILDvLOM0oU5wapO4DgMUw8UxEbN+NQsaQyMsfwpjEAUgImamg5oGGaTNAjRTV5Qfv8A8qv22u3G4bXyfw/wrCzR6CnBmzB4kmjIEsWfx/8Are4rsLC4+16ck6jbvBIFWm+pnWjFRujOkmmVj81KLuUD71TYxuBu5COWqNrhscmkIztQuGETc9q4ySTLyN60AZE2S5x0NaOlQFyOKb+EXUv6mmxQKzaql8Ipbh7VpWVp5hBIp1JWRK1NqCxUKOKebRc8CuP2juaqAySzGOlUpbYrzWtOoTKNio/FRO2a6k7mRExxUL+tMZGW4qNjgc0hkROaQZC0AOGMUnc0CPSrnwHpUh/0ZTAPqzenqfasO78B3tud9ldecfTYF9PU/WqvGW4ax2M8T6tpBK3dtkf7wP8AL6ivSNDnNzo9vMV271zj05qHFo05+ZWJXskc5ziom00MP9Z+lTYVynPYNH/y0qlKhQElqlpDMPVLzZEykda5vAY4Y4BpAUpkCtxW5oSZTcRSl8ILcl1ocisiqo/CKe46JNzgCuo06DEQ46VNd2QRWpoopC9KQ9elcRuhTg5yM1BPEGX0pxdmEkZFzDyazpDjgV305XRzyViFiO9Qsa0JGNwOKibJoGNI4pTigQhzSZoGe5rMjDINIZ4x1bpXP7eBpysz7zVLJFKvh/bkf56UyLV7eO1HlJgDoM0Rr8zstg9nbUtDUoCgbdgntUc2rQqhK8ntV8wrGHeai0jHDVnyXDMeTUjMrWf9WOazrWJZZgGGQKtbkvYiurYJKVAwK1tJGyADFZz+FjW4/VULR8CsNgSaqg/dCe5oabB5jA4rqLNNqAVjiHrYIIsEd8VA3U5NcyNgBpHIx81MZRuU3LntWHdDB4rqosxmimepzTCcjArqMyNjjimNyaAE780jUAJnNBFAj0b7e7ZECk/596hlF1KCZJNg/CvPhQctWdLnYqS/Z4gTJJuNZtxqcceVi6V0WitESk2dPoNoup2Kzrckc8jZ71fk0FyOLjJ/3f8A69bJJmbumZn9kSO5Uycj2pBosin5pP0o5UJtmPr9uIEVd2c1k2RxLkgULcHsWzb+bK5PTFS2S4TA6CsJPRotbjLmfzG2VmyREPzWlJWQps2NKj2gHFb0I44rlrP3i4LQmcfLxVOXO6sUWMB4wKQ/rVAyKb7nWsG+YBjitqO5nIzyeCaYTnpXaZCAEnmkPDYoAYwO7NNb0oARjgUmeKBHc3OtxRApAo9KxbvVpZSctgVzuVzrhS7mVNd5Jy2TVOS4LHinGNypyUUdj4H1S5t1aHy/3TdDx7+3vXbDUC45+WtLNHPdMoGUJdOc5pk92TnHf3oEcv4kkJgDDqDWRbt+9xTRLN5k8mzEmOTTLVD5LN61g1o2WuhmTZWbcelWJYhJGrAV0R2Ie5oWS7YxWnC1efU3N47FknK4NUpxg1mhjFBPAGKGAHQVQMq3TYQ5rn7x8uQK6KK1M5lM+/Sm8A8V1mQdTmkHU0CG9+aY2SaBjCaaSO1AizJeE8VApmuJAkSszHoAKyhTbZ11KySNe08J6rcoHki8sH3Bz096tW/h97dwZIcke/0ro5OU5Ofm3Ne1PkDAXbWrBODxWTv1LTQkzYuAezVBOyn6+tIDntekzZPz3FZNi+5wcVSEzrLld2mRgd80QKBZkY5rNRvF+o76ohnsDJa7wOaq2XJ8th0pxldMRqrHsXinwthsVxSd2dC2Linjj86rygFvWs1uA0DjmmyEBTiqEzIvp1wRmsWVgxJrrooykQnrScA10EAo7mkOMZoERbqYWHWgZEzZNMLUgO48M+C7e9iW7vpvMT/nlgj9c+1d1p+lWOmxCOzgWNR7k/zqudWtEXK27yLmB6VG1vGTnFSm0VZEf2KAnLICajfS7R+fK59cmnzCsZGr2H2WNZlfKg9MfSsqZXIyOQaVhmFrEbtayZHasnTBnaaEJnVRyF4VQ9BVyBMLg96tRsmJs2La0DWxBHWubu7b7NfcDgn+lckH70kaNaI01TdCDVXGH9TXM9zZbFtD8uCaCDipERkAcms+9uNqkA1cVdiZgXM29jzVVjkV3QVkYvUaBzzSHGCaskTfgVE74oGQs1MZ+KQERamluaBnaabqtzpsu6B/l/iXA5/Su80fW7bVY/k+SYfej5OOvfHtXNCXK7M0aualFdBAUHgc0AY2tMZ7Z0HQVgwvutxnmmJkd3AslnICMnFcppygRrxyDVIR0lqMqKvqPkz6VZB0On/NagmsrXbcbw3X3/KvOTtVZ0fZK9q37vaeajkT5s5rOatIuOxKg+X2p4A6k1AMpXk2wHFc9eTMxPNdFFESZnFuaiZq7DIbv75pjSDFICJpMjFRs/FAyMtUbNQAzPFITSA6kEN0NPhmlt5lkjbDqeDWUo3LTPS9B1L+07BZW++OG/M+1aVaRd0SNLgdTUE0u/5VqkgKc6/uyPWuZgyskkPcGmxFxI/3RHTiuLgBSeWP+61NEnQWROwVoKMLn1qxG/pR/wBF4pdQtTcRZXgivMqO1Vs2WxjiBlUg9RVEz7JsMeK2kuZXGnYvR4ZQcZpZX2J6VyPctmJeNvY81k3S4FdFN2IaMqWTBqu0ma6+hmRl+OtRtJSAaX9DTS1ADSfemmgBKSgDViuWQ8nir8NysnB61BpJHoPghVGnysDklhn2610UjBVyatEFWVwwJJ5qo8zqSFqgIWvNhG5dw9SawruZP7U3pwH/AMKQGj5ke35W7Vw9wfL1e5X3H8qdyTYspOBWtCxIq0SbmksPLKitPnHNedXX7xmsfhOev5PKusY4Pv7Vk6kh/wBYvHtW9Fe6gkJZXg24LU65uAw4Oa56kbSLi7ozJZBk1nXcmQaqI2Ys7/MarljXUtjJjSfSm96YB+NJTEITTTSAKSmBYWU1YjcZyDUGt7nofw/ffHIQ3fkY+tdo6FxgnirRmynLBIBtUbv0qhMkiHJTB+tVuK5Qmfnp1rH1I42sB0qQK63ThaxdQkJ1QuT94f0oQM1LFsge9bMB6VoiDf0baWbPUVr9q4MQvfZpDY5XxAdk+RWeswnj2kVvR+BBLcqeS0cnHSmTSEdTWVXVlwRSlc1TuDkdamI2ZE/3qg710rYzYhptMQdqSmAEmkNACUlIC49q6HpketNAKmgpprc7L4fziO+cBsE9sex9q9K8xeSTTFuQvdwx571n3WpkqQgAoCxhXVyXYkms27ctGRSAzlfK4rN1I4nifjvTQjRsH4Fb9sc4rREM2tGP+lZznNbp4FcWJ+I0hscn4jlzKea55JykgxWlD+GOe5eMgMeazLiTJrF7lrYqs3NQzHimgZlT9ahPeuhbGbGmm1QhOKUkUgE4pOMUAIaSmBpNdE9aajhjjbmpSsbTmpHR+HNPnjvEuSu1R+Oa7aS9GeG59K0asYrUqtdBic96iKl+h4pDKdxDn+KqEiMTgCkBmspWUowxVTV4SturHsaBEmnSdM10Vs/yirRDNawl23MZ7Z5rpJ2AiJNcmK6GlM4/WzuJxXP43SY61dH4B1PiLpBWKs6c81j1LWxWJ5qOU/LVCZlzHmoq3WxA0004piENFMBKQ0AFJxQA/9kAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

ESP_EVENT_DEFINE_BASE(VIEW_EVENT_BASE);
esp_event_loop_handle_t view_event_handle;

extern void app_view_init(void);
void app_main(void)
{
    ESP_LOGI(TAG, "system start");

    ESP_LOGI("", SENSECAP, VERSION, __DATE__, __TIME__);

    ESP_ERROR_CHECK(bsp_board_init());
    lv_port_init();

    esp_event_loop_args_t view_event_task_args = {
        .queue_size      = 10,
        .task_name       = "view_event_task",
        .task_priority   = uxTaskPriorityGet(NULL),
        .task_stack_size = 10240,
        .task_core_id    = tskNO_AFFINITY};
    ESP_ERROR_CHECK(esp_event_loop_create(&view_event_task_args, &view_event_handle));


#if CONFIG_LCD_AVOID_TEAR
    ESP_LOGI(TAG, "Avoid lcd tearing effect");
#if CONFIG_LCD_LVGL_FULL_REFRESH
    ESP_LOGI(TAG, "LVGL full-refresh");
#elif CONFIG_LCD_LVGL_DIRECT_MODE
    ESP_LOGI(TAG, "LVGL direct-mode");
#endif
#endif

    lv_port_sem_take();
    /* (must be 480*800, set LCD_EVB_SCREEN_ROTATION_90 in menuconfig)*/
    ui_init();

    lv_port_sem_give();

    ESP_LOGI(TAG, "Out of ui_init()");

    app_view_init();
    
    esp32_rp2040_init();

    // xTaskCreate(&__indicator_image_task, "__indicator_image_task", 1024 * 10, NULL, 50, NULL);
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

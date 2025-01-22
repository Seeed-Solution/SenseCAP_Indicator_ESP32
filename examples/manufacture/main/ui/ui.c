#include "ui.h"
#include "ui_helpers.h"
#include <stdlib.h>
#include "esp_log.h"

#define TEST_MSG  "TEST MSG: "

lv_obj_t *ui_screen_start;
lv_obj_t *ui_screen_color;
lv_obj_t *ui_screen_color_change;
lv_obj_t *ui_screen_touch;
lv_obj_t *ui_screen_end;

static lv_timer_t * color_change_timer =NULL;

static int color_index = 0;
static uint32_t colors[] = { 0x000000, 0x0000FF, 0x00FF00, 0xFF0000, 0xFFFFFF};


void timer_cb(lv_timer_t * timer)
{
    static int color_index = 0;
    static uint32_t colors[] = { 0x0000FF, 0x00FF00, 0xFF0000};
    
    color_index++;
    if( color_index >= 3) {
        color_index=0;
    }
    lv_obj_set_style_bg_color( ui_screen_color_change, lv_color_hex(colors[color_index]), LV_PART_MAIN | LV_STATE_DEFAULT ); 
}

void ui_event_screen_start( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT  ) {
       _ui_screen_change( ui_screen_color, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }

    if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT  ) {
        color_change_timer = lv_timer_create(timer_cb, 1000, NULL);
       _ui_screen_change( ui_screen_color_change, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
}

void ui_event_screen_color_change( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT  ) {
        if( color_change_timer != NULL ) {
            lv_timer_del(color_change_timer);
            color_change_timer=NULL;
        }
       _ui_screen_change( ui_screen_start, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}

void ui_event_screen_color( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

    if ( event_code == LV_EVENT_GESTURE  ) {

    	if( lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT ) {
    		color_index++;
    	}

        if ( lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT  ) {
            color_index--;
        }
        if( color_index < 0 ){
            color_index=0;
            _ui_screen_change( ui_screen_start, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
            return;
        }
        if( color_index >= sizeof(colors)/sizeof(uint32_t) ) {
        	color_index = sizeof(colors)/sizeof(uint32_t) -1 ;
            _ui_screen_change( ui_screen_touch, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
            return;
        }
        LV_LOG("color_index: %d\n", color_index);
        lv_obj_set_style_bg_color( ui_screen_color, lv_color_hex(colors[color_index]), LV_PART_MAIN | LV_STATE_DEFAULT );
    }
}

static uint16_t touch_state = 0;
static const char * btnm_map[] = {"1", "2", "3", "4", "\n",
                                  "5", "6", "7", "8", "\n",
                                  "9", "10", "11", "12","\n",
                                  "13", "14", "15", "16", ""
                                 };

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);
        int index = atoi(txt);
        
        ESP_LOGI("UI",  TEST_MSG "btn %d click", index);
        touch_state |= 1 << (index -1);

        LV_LOG("touch_state: %d \n", touch_state);

        if(touch_state == 0xffff ) {
            ESP_LOGI("UI",  TEST_MSG "lcd touch ok");
        	//_ui_screen_change( ui_screen_end, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
        }
    }


}
void ui_event_screen_touch( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);
    if ( event_code == LV_EVENT_GESTURE   && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT ) { 
         _ui_screen_change( ui_screen_color, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}

void ui_init(void)
{
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp,  lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_RED), false, &lv_font_montserrat_48);

    // start
    ui_screen_start = lv_obj_create(NULL);
    lv_obj_t * label = lv_label_create(ui_screen_start);
    lv_label_set_text(label ,"START TEST");
    lv_obj_set_align( label, LV_ALIGN_CENTER );
    lv_obj_add_event_cb(ui_screen_start, ui_event_screen_start, LV_EVENT_ALL, NULL);

    //color test change 
    ui_screen_color_change = lv_obj_create(NULL);
    lv_obj_set_style_bg_color( ui_screen_color_change, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_add_event_cb(ui_screen_color_change, ui_event_screen_color_change, LV_EVENT_ALL, NULL);

    // color test
    ui_screen_color = lv_obj_create(NULL);
    lv_obj_set_style_bg_color( ui_screen_color, lv_color_hex(colors[color_index]), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_add_event_cb(ui_screen_color, ui_event_screen_color, LV_EVENT_ALL, NULL);

    // touch test
    ui_screen_touch = lv_obj_create(NULL);
    lv_obj_t * btnm1 = lv_btnmatrix_create(ui_screen_touch);
    lv_obj_set_width(  btnm1, lv_pct(100));
    lv_obj_set_height( btnm1, lv_pct(100));

    lv_obj_set_style_pad_row(btnm1, 1, 0);
    lv_obj_set_style_pad_column(btnm1, 1, 0);
    lv_obj_set_style_pad_bottom(btnm1,1, 0);
    lv_obj_set_style_pad_top(btnm1,1, 0);
    lv_obj_set_style_pad_left(btnm1,1, 0);
    lv_obj_set_style_pad_right(btnm1,1, 0);

    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);

    lv_btnmatrix_set_map(btnm1, btnm_map);
    for(int i = 0; i < 16; i++) {
        lv_btnmatrix_set_btn_ctrl(btnm1, i, LV_BTNMATRIX_CTRL_CHECKABLE);
    }
    lv_obj_add_event_cb(ui_screen_touch, ui_event_screen_touch, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);


    // end
    ui_screen_end = lv_obj_create(NULL);
    lv_obj_t * label2 = lv_label_create(ui_screen_end);
    lv_label_set_text(label2 ,"END TEST");
    lv_obj_set_align( label2, LV_ALIGN_CENTER );

    lv_disp_load_scr(ui_screen_start);
}


/**
 * @file terminal.c
 * @date  23 November 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#include "esp_log.h"
#include "esp_event.h"
#include "view_data.h"
#include "indicator_terminal.h"

static const char *TAG          = "terminal";
static const char *TAG_TERMINAL = "indicator>";

static struct indicator_terminal __g_terminal;

#define COLOR_INFO  0x66BB6A // green
#define COLOR_WARN  0xFDD835 // Yellow
#define COLOR_ERROR 0xE91E63 // Pink

#ifdef MAX_TERMINAL_LOG_COUNT
    #error "MAX_TERMINAL_LOG_COUNT (ms)is already defined"
#else
    #define MAX_TERMINAL_LOG_COUNT 40
#endif

#ifdef MAX_TERMINAL_SCROLL_TIME
    #error "MAX_TERMINAL_SCROLL_TIME (ms) is already defined"
#else
    #define MAX_TERMINAL_SCROLL_TIME 300
#endif

/* For more interactive UX*/
#define DOUBLE_CLICK_INTERVAL_MAX 500
#define DOUBLE_CLICK_INTERVAL_MIN 20 // debounce time


static void createNewLine(lv_obj_t *container, const char *logLine, const lv_color_t *color);

void addLogLineImpl(struct indicator_terminal *terminal, const char *logLine, lv_color_t color);
void clearLogImpl(struct indicator_terminal *terminal);
void scrollToBottomImpl(struct indicator_terminal *terminal);
void scrollDisableImpl(struct indicator_terminal *terminal);
void scrollEnableImpl(struct indicator_terminal *terminal);

void clearLogImpl(struct indicator_terminal *terminal)
{
    ESP_LOGI(TAG, "clearLogImpl");
    lv_obj_clean(terminal->container);
}

void scrollToBottomImpl(struct indicator_terminal *terminal)
{
    if (terminal->auto_scroll) {
        lv_obj_t *last_child = lv_obj_get_child(terminal->container, lv_obj_get_child_cnt(terminal->container) - 1);
        if (last_child) {
            lv_obj_scroll_to_view(last_child, LV_ANIM_ON);
        }
    }
}

void scrollDisableImpl(struct indicator_terminal *terminal)
{
    ESP_LOGI(TAG, "scrollDisableImpl");
    terminal->auto_scroll = false;
}
void scrollEnableImpl(struct indicator_terminal *terminal)
{
    ESP_LOGI(TAG, "scrollEnableImpl");
    terminal->auto_scroll = true;
}

void addLogLineImpl(struct indicator_terminal *terminal, const char *logLine, lv_color_t color)
{
    LV_ASSERT_OBJ(terminal->container, MY_CLASS);
    LV_ASSERT_NULL(logLine);

    if ((lv_obj_get_child_cnt(terminal->container) >= MAX_TERMINAL_LOG_COUNT)) // if (lv_obj_get_child_cnt(container) >= MAX_LOG_COUNT)
    {
        lv_obj_t *firstChild = lv_obj_get_child(terminal->container, NULL);
        if (firstChild != NULL) {
            lv_obj_del(firstChild);
        }
    }

    createNewLine(terminal->container, logLine, &color);
    if (terminal->scrollable && terminal->user_scrolling && terminal->auto_scroll) {
        terminal->scrollDisable(terminal);
    }
    // else if(!terminal->user_scrolling && !terminal->auto_scroll)
    // {
    //     terminal->scrollEnable(terminal);
    // }
    terminal->scrollToBottom(terminal);
    return;
}

/******************** EVENTS **************************/

bool isDoubleClick(indicator_terminal_t *terminal)
{
    uint32_t current_time = lv_tick_get();
    if ((current_time - terminal->last_click_time) >= DOUBLE_CLICK_INTERVAL_MIN && (current_time - terminal->last_click_time) <= DOUBLE_CLICK_INTERVAL_MAX) {
        terminal->last_click_time = 0; // 重置
        return true;
    }
    terminal->last_click_time = current_time;
    return false;
}

void terminal_events_add_handler(lv_event_t *e)
{
    static uint32_t press_time     = 0;

    lv_event_code_t       code     = lv_event_get_code(e);
    indicator_terminal_t *terminal = (indicator_terminal_t *)lv_event_get_user_data(e);

    if (terminal == NULL)
        return;

    switch (code) {
        case LV_EVENT_PRESSED:
            if (!terminal->auto_scroll && isDoubleClick(terminal)) {
                ESP_LOGI(TAG, "double clicked & scroll disable");
                lv_port_sem_take();
                terminal->scrollEnable(terminal);
                terminal->scrollToBottom(terminal);
                lv_port_sem_give();
            }
            // press_time = lv_tick_get();
            break;
        case LV_EVENT_PRESSING:
            terminal->user_scrolling = true;
            break;
        case LV_EVENT_RELEASED:
            // if (terminal->auto_scroll && (lv_tick_get() - press_time > MAX_TERMINAL_SCROLL_TIME))
            // {
            //     terminal->scrollDisable(terminal);
            // }
            terminal->user_scrolling = false;
            break;
        case LV_EVENT_SCROLL_BEGIN:
            terminal->scrollable = true;
            break;
        default:
            break;
    }
}

void doubleClickedTerminal(struct indicator_terminal *terminal)
{
    static uint32_t last_click_time = 0;

    uint32_t pre_time               = lv_tick_get();
    if (pre_time - last_click_time < 500) {
        ESP_LOGI(TAG, "double clicked");
        terminal->auto_scroll = true;
        terminal->scrollToBottom(terminal);
    }
    last_click_time = pre_time;
}

static void createNewLine(lv_obj_t *container, const char *logLine, const lv_color_t *color)
{
    static lv_coord_t y_offset = 0; // used for calculating the vertical position of the next label
    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, logLine);
    lv_obj_set_style_text_color(label, *color, 0); //  Text color
    lv_obj_set_pos(label, 0, y_offset);            // popsition of the label
    y_offset += lv_obj_get_height(label) + 30;     // update the position of the next label
}

static void __view_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == VIEW_EVENT_LORAWAN_LOG) {
        struct view_data_lorawa_log *log_info = (struct view_data_lorawa_log *)event_data;
        lv_color_t                   color    = lv_color_white();

        char log_line[128];
        memset(log_line, 0, sizeof(log_line));
        strcpy(log_line, log_info->data);
        switch (log_info->level) {
            case LORAWAN_LOG_LEVEL_INFO:
                color = lv_color_hex(COLOR_INFO);
                break;
            case LORAWAN_LOG_LEVEL_WARN:
                color = lv_color_hex(COLOR_WARN);
                break;
            case LORAWAN_LOG_LEVEL_ERROR:
                color = lv_color_hex(COLOR_ERROR);
                break;
            default:
                break;
        }
        ESP_LOGI(TAG, "log_info->data %s", log_line);
        lv_port_sem_take();
        __g_terminal.addLogLine(&__g_terminal, log_line, color);
        lv_port_sem_give();
    }
}


void __init_terminal(indicator_terminal_t *terminal, lv_obj_t *container)
{
    if (!terminal || !container)
        return;


    terminal->container       = container;
    terminal->auto_scroll     = true;
    terminal->scrollable      = false;
    terminal->last_click_time = 0;

    terminal->addLogLine      = addLogLineImpl;
    terminal->clearLog        = clearLogImpl;
    terminal->scrollToBottom  = scrollToBottomImpl;
    terminal->scrollDisable   = scrollDisableImpl;
    terminal->scrollEnable    = scrollEnableImpl;

    lv_obj_add_event_cb(ui_PnlConsole, terminal_events_add_handler, LV_EVENT_ALL, terminal);
}

void indicator_terminal_init(void)
{
    __init_terminal(&__g_terminal, Panel_Container);

    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(view_event_handle,
                                                             VIEW_EVENT_BASE, VIEW_EVENT_LORAWAN_LOG,
                                                             __view_event_handler, NULL, NULL));
}

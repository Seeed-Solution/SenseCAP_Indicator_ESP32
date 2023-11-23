#include "timer.h"

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *context ) )
{

    obj->ReloadValue = 0;
    obj->IsStarted = false;
    obj->timer_args.callback = callback;
    obj->timer_args.arg = NULL;
    obj->timer_args.name = "lorawan";
    ESP_ERROR_CHECK(esp_timer_create(&obj->timer_args, &obj->timer_handle));
}

void TimerInit2( TimerEvent_t *obj, void ( *callback )( void *context ),  void *context)
{

    obj->ReloadValue = 0;
    obj->IsStarted = false;
    obj->timer_args.callback = callback;
    obj->timer_args.arg = context;
    obj->timer_args.name = "lorawan";
    ESP_ERROR_CHECK(esp_timer_create(&obj->timer_args, &obj->timer_handle));
}

void TimerStart( TimerEvent_t *obj )
{
    obj->IsStarted = true;
    ESP_ERROR_CHECK(esp_timer_start_once(obj->timer_handle, obj->ReloadValue));
}

bool TimerIsStarted( TimerEvent_t *obj )
{
    return obj->IsStarted;
}

void TimerStop( TimerEvent_t *obj )
{
    obj->IsStarted = false;
    esp_timer_stop(obj->timer_handle);
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop( obj );
    TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    uint64_t ticks = value * 1000; //us

    TimerStop( obj );

    obj->ReloadValue = ticks;
}

TimerTime_t TimerGetCurrentTime( void )
{
    int64_t now = esp_timer_get_time( );
    return (TimerTime_t)(now/1000);
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )
{
    if ( past == 0 )
    {
        return 0;
    }
    uint32_t now = TimerGetCurrentTime( );
    return (now-past);
}



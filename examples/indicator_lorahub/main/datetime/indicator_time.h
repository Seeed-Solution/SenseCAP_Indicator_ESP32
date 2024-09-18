/**
 * @file indicator_time.h
 * @date  14 December 2023

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2023, Seeed Studio
 */

#ifndef INDICATOR_TIME_H
#define INDICATOR_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


// bool     isTimeValid(void);
// uint64_t getCurrentTimestampMs(void);
// void     printFormattedMsTime(uint64_t timestamp);
bool isTimeValid(void);


void time_model_init(void);
void printCurrentTime(void);
uint64_t getTimestampMs(void);
uint64_t getTimestampSec(void);

int indicator_time_net_zone_set( char *p);


/* View */
void view_datetime_init(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*INDICATOR_TIME_H*/
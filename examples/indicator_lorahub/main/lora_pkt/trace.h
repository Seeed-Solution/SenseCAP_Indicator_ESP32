/*
   _____                      _________    ____
  / ___/___  ____  ________  / ____/   |  / __ \
  \__ \/ _ \/ __ \/ ___/ _ \/ /   / /| | / /_/ /
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/
/____/\___/_/ /_/____/\___/\____/_/  |_/_/
  (C)2024 Seeed Studio

Description:
    LoRa concentrator : Packet Forwarder trace helpers

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef _LORA_PKTFWD_TRACE_H
#define _LORA_PKTFWD_TRACE_H

#define DEBUG_PKT_FWD 0
#define DEBUG_JIT 0
#define DEBUG_JIT_ERROR 1
#define DEBUG_TIMERSYNC 0
#define DEBUG_BEACON 0
#define DEBUG_LOG 1

#define MSG_DEBUG( FLAG, fmt, ... )                                                                 \
    do                                                                                              \
    {                                                                                               \
        if( FLAG )                                                                                  \
            fprintf( stdout, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__ ); \
    } while( 0 )
#define MSG_PRINTF( FLAG, fmt, ... )               \
    do                                             \
    {                                              \
        if( FLAG )                                 \
            fprintf( stdout, fmt, ##__VA_ARGS__ ); \
    } while( 0 )

#endif
/* --- EOF ------------------------------------------------------------------ */

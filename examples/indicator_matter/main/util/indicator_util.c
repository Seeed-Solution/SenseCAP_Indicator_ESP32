#include "indicator_util.h"

int wifi_rssi_level_get(int rssi)
{
	//    0    rssi<=-100
	//    1    (-100, -88]
	//    2    (-88, -77]
	//    3    (-66, -55]
	//    4    rssi>=-55
    if( rssi > -66 ) {
    	return 3;
    } else if( rssi > -88) {
    	return 2;
    } else {
    	return 1;
    }
}
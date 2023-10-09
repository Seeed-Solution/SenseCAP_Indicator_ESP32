#include "indicator_util.h"
#include <arpa/inet.h>
#include <netdb.h>

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

bool isValidIP(const char *input)
{
    struct in_addr  ipv4;
    struct in6_addr ipv6;

    // Try to parse input as an IPv4 address
    if (inet_pton(AF_INET, input, &ipv4) == 1) {
        return true; // Successfully parsed as IPv4
    }

    // // Try to parse input as an IPv6 address
    // if (inet_pton(AF_INET6, input, &ipv6) == 1) {
    //     return true; // Successfully parsed as IPv6
    // }

    return false; // Not a valid IP address
}

bool isValidDomain(const char *input)
{
    struct hostent *host_info = gethostbyname(input);

    if (host_info == NULL) {
        return false; // Unable to resolve the domain, not a valid domain name.
    }

    return true; // Successfully resolved the domain, it's a valid domain name.
}


#include "wdt_ss_api.h"
#define WDT_SS_API_VERSION_MAJOR 1
#define WDT_SS_API_VERSION_MINOR 0
#define WDT_SS_API_VERSION_PATCH 0

#define WDT_SS_API_VERSION \
    ((WDT_SS_API_VERSION_MAJOR << 16) | (WDT_SS_API_VERSION_MINOR << 8) | WDT_SS_API_VERSION_PATCH)

int wdt_ss_get_api_version(void)
{
    return WDT_SS_API_VERSION;
}








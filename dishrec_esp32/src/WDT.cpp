#include "WDT.h"

void disable_wdt()
{
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
}

inline void force_reset_wdt_0()
{
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
}

inline void force_reset_wdt_1()
{
    TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_feed = 1;
    TIMERG1.wdt_wprotect = 0;
}

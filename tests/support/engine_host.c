#include "hal.h"
#include "check.h"
/* A test may call core APIs directly, but must never enter the firmware boot
   loop without supplying an explicit simulated driver. */
bool driver_init(void)
{
    CHECK(false);
    return false;
}

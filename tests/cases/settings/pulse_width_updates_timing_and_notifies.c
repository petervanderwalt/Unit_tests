#include "support/settings_host.h"
#include "check.h"

int main(void)
{
    prepare_settings_store();
    hal.step_us_min = 2.0f;
    settings.version.id = SETTINGS_VERSION;
    hal.nvs.put_byte(0, SETTINGS_VERSION);
    settings_write_global();
    settings_init();
    change_callbacks = 0;
    CHECK(store_setting(Setting_PulseMicroseconds, "8.5") == Status_OK);
    NEAR(settings.steppers.pulse_microseconds, 8.5f);
    CHECK(change_callbacks == 1);
    return EXIT_SUCCESS;
}

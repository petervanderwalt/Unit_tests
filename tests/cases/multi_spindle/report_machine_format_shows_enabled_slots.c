#include "support/multi_spindle_host.h"
#include "report.h"
#include "check.h"

int main(void)
{
    prepare_multi_spindle();
    CHECK(spindle_enable(1) == 1);
    CHECK(spindle_is_enabled(1) && spindle_get(1)->id == 1);
    CHECK(report_spindles(true) == Status_OK);
    fprintf(stderr, "%s", engine_output);
    CHECK(strcmp(engine_output, "[SPINDLE:0|0|1|DR|primary|1000.0,12000.0]" ASCII_EOL
                               "[SPINDLE:1|1|1|DR|secondary|1000.0,12000.0]" ASCII_EOL
                               "[SPINDLE:2|-|1|DR|auxiliary|1000.0,12000.0]" ASCII_EOL) == 0);
    return EXIT_SUCCESS;
}

#include "support/mpg_stream_host.h"
#include "check.h"

int main(void)
{
    prepare_mpg_stream();
    CHECK(stream_mpg_register(&pendant_device, false, NULL));
    CHECK(!settings.flags.report_inches);
    pendant_send_text("$13\n$G\n");
    engine_execute_tasks(STATE_IDLE);
    fprintf(stderr, "observed pendant output: %s\n", pendant_output);
    CHECK(strstr(pendant_output, "$13=0" ASCII_EOL) != NULL);
    CHECK(strstr(pendant_output, "[GC:G0 G54 G17 G21 G90") != NULL);
    return EXIT_SUCCESS;
}

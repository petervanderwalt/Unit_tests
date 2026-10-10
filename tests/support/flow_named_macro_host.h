#pragma once
#include "support/flow_host.h"
#include "support/report_host.h"
#include "fs_embedded.h"
#include "ngc_params.h"
#include "stream_file.h"
#include "stream.h"
static const embedded_file_t flow_macro_file = {.name = "fixture.macro", .size = 4, .data = {'G', '2', '1', '\n'}};
static const embedded_file_t *flow_macro_files[] = {&flow_macro_file, NULL};
static ngc_string_id_t named_macro_label;
static inline void prepare_named_macro(void)
{
    prepare_report();
    hal.stream.read = stream_get_null;
    fs_embedded_mount(flow_macro_files);
    char name[] = "fixture";
    named_macro_label = ngc_string_param_set_name(name);
    CHECK(named_macro_label > NGC_MAX_PARAM_ID);
    flow_skip = false;
}

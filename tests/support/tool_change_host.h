#pragma once
#include "support/motion_program_host.h"
#include "tool_change.h"
#include "report.h"
static unsigned select_calls, reset_calls, homing_calls;
static bool selected_next, homing_success;
static tool_data_t *selected_tool;
static axes_signals_t homing_axes;
static atc_status_t no_atc(void) { return ATC_None; }
static bool suspend_tool_stream(bool value) { return value; }
static void original_select(tool_data_t *tool, bool next)
{
    selected_tool = tool;
    selected_next = next;
    select_calls++;
}
static void original_reset(void) { reset_calls++; }
static void original_homing(axes_signals_t axes, bool success)
{
    homing_axes = axes;
    homing_success = success;
    homing_calls++;
}
static control_signals_t tool_control_state(void) { return (control_signals_t){0}; }
static inline void prepare_tool_change(void)
{
    prepare_motion_program();
    hal.tool.atc_get_state = no_atc;
    hal.tool.select = original_select;
    hal.stream.suspend_read = suspend_tool_stream;
    hal.driver_reset = original_reset;
    hal.control.get_state = tool_control_state;
    grbl.on_homing_completed = original_homing;
    settings.tool_change.mode = ToolChange_Manual;
    tc_init();
    CHECK(hal.tool.change != NULL);
}

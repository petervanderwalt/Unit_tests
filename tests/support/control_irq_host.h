#pragma once
#include "support/system_command_host.h"
#include "support/stepper_host.h"
#include "probe.h"
static bool irq_probe_input;
static bool irq_read_probe(void *context) { CHECK(context == &irq_probe_input); return irq_probe_input; }
static inline void prepare_control_irq(void)
{
    prepare_system_command();
    sys.driver_started = true;
    CHECK(probe_add(Probe_Default, 0, IRQ_Mode_None, &irq_probe_input, irq_read_probe));
    hal.driver_cap.probe = true;
}
static inline void prepare_control_irq_cycle(void)
{
    prepare_control_irq();
    hal.f_step_timer = 1000000;
    hal.stepper.enable = enable_motors;
    hal.stepper.go_idle = idle_driver;
    hal.stepper.wake_up = wake_driver;
    hal.stepper.cycles_per_tick = timer_cycles;
    hal.stepper.pulse_start = pulse_driver;
    st_reset();
    char block[] = "G1X1F100";
    CHECK(gc_execute_block(block) == Status_OK);
    CHECK(plan_get_current_block() != NULL);
    state_set(STATE_CYCLE);
    CHECK(state_get() == STATE_CYCLE);
}

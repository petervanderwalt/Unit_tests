#pragma once
#include "support/engine_host.h"
#include "protocol.h"
#include "grbllib.h"
#include "state_machine.h"
#include "nvs.h"
#include "check.h"
#include <string.h>
static unsigned boot_reads, boot_setup_calls, boot_release_calls;
static const char *boot_program = "$G\n";
static bool boot_setup_success = true, boot_init_success = true;
static bool boot_force_alarm;
static const char *boot_startup_program = "";
static bool setup(settings_t *s) { CHECK(s == &settings); boot_setup_calls++; return boot_setup_success; }
static bool release_driver(void) { boot_release_calls++; return false; }
static control_signals_t controls(void) { return (control_signals_t){0}; }
static limit_signals_t limits(void) { return (limit_signals_t){0}; }
static void enable_limits(bool on, axes_signals_t homing) { (void)on; (void)homing; }
static void enable_steps(axes_signals_t axes, bool hold) { (void)axes; (void)hold; }
static void idle(bool clear) { (void)clear; }
static void wake(void) { CHECK(false); }
static void cycles(uint32_t n) { CHECK(n > 0); }
static void pulse(stepper_t *s) { (void)s; CHECK(false); }
static void coolant_set(coolant_state_t s) { CHECK(!s.mask); }
static coolant_state_t coolant_get(void) { return (coolant_state_t){0}; }
static void reset_read(void) { }
static uint16_t count(void) { return 0; }
static bool connected(void) { return true; }
static bool write_char(uint8_t c) { char s[2] = {(char)c, 0}; hal.stream.write(s); return true; }
static int32_t read_char(void)
{
    CHECK(boot_reads < 1000);
    if(boot_program[boot_reads]) return (uint8_t)boot_program[boot_reads++];
    protocol_enqueue_realtime_command(CMD_EXIT);
    return SERIAL_NO_DATA;
}
/* Wrap the board entry point only; all core initialization remains real. */
bool __wrap_driver_init(void)
{
    grbl_t core_grbl = grbl; system_t core_sys = sys; grbl_hal_t core_hal = hal;
    engine_prepare(); grbl = core_grbl; sys = core_sys;
    hal.version = core_hal.version; hal.driver_reset = core_hal.driver_reset;
    hal.nvs.size = core_hal.nvs.size; hal.step_us_min = core_hal.step_us_min;
    hal.tool.atc_get_state = core_hal.tool.atc_get_state;
    hal.control.interrupt_callback = core_hal.control.interrupt_callback;
    hal.limits.interrupt_callback = core_hal.limits.interrupt_callback;
    hal.stepper.interrupt_callback = core_hal.stepper.interrupt_callback;
    hal.stream_blocking_callback = core_hal.stream_blocking_callback;
    hal.coolant_cap = core_hal.coolant_cap;
    hal.driver_cap.step_pulse_delay = true; hal.driver_setup = setup; hal.driver_release = release_driver;
    hal.control.get_state = controls; hal.limits.get_state = limits; hal.limits.enable = enable_limits;
    hal.f_step_timer = 1000000; hal.stepper.enable = enable_steps; hal.stepper.go_idle = idle;
    hal.stepper.wake_up = wake; hal.stepper.cycles_per_tick = cycles; hal.stepper.pulse_start = pulse;
    hal.coolant.set_state = coolant_set; hal.coolant.get_state = coolant_get;
    hal.stream.read = read_char; hal.stream.reset_read_buffer = reset_read; hal.stream.get_tx_buffer_count = count;
    hal.stream.get_rx_buffer_free = count; hal.stream.is_connected = connected; hal.stream.write_char = write_char;
    settings.flags.force_initialization_alarm = boot_force_alarm;
    settings.version.id = SETTINGS_VERSION; settings.version.build = GRBL_BUILD - 20000000UL;
    hal.nvs.put_byte(0, SETTINGS_VERSION); settings_write_global();
    coord_system_data_t coordinates = {0}; for(unsigned i = 0; i < N_CoordinateSystems; i++) settings_write_coord_data((coord_system_id_t)i, &coordinates);
    stored_line_t empty = {0}; for(unsigned i = 0; i < N_STARTUP_LINE; i++) settings_write_startup_line(i, empty);
    settings_write_build_info(empty);
    stored_line_t startup = {0};
    CHECK(strlen(boot_startup_program) < sizeof(startup));
    strcpy(startup, boot_startup_program);
    settings_write_startup_line(0, startup);
    return boot_init_success;
}

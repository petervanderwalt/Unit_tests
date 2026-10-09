#pragma once
#include "support/engine_host.h"
#include "ioports.h"
#include "check.h"
#include <string.h>
static xbar_t digital_pins[2][2] = {
    {{.id = 0, .function = Input_Aux0, .cap = {.input = true}},
     {.id = 1, .function = Input_Aux1, .cap = {.input = true}}},
    {{.id = 0, .function = Output_Aux0, .cap = {.output = true}},
     {.id = 1, .function = Output_Aux1, .cap = {.output = true}}}
};
static unsigned output_calls, read_calls;
static uint8_t physical_output, physical_input;
static bool output_value;
static wait_mode_t input_mode;
static float input_timeout;
static xbar_t *pin_info(io_port_direction_t dir, uint8_t port)
{
    return port < 2 ? &digital_pins[dir][port] : NULL;
}
static void pin_description(io_port_direction_t dir, uint8_t port, const char *text)
{
    CHECK(port < 2 && text != NULL);
    digital_pins[dir][port].description = text;
}
static void pin_output(uint8_t port, bool value)
{
    CHECK(port < 2);
    physical_output = port;
    output_value = value;
    output_calls++;
}
static int32_t pin_read(uint8_t port, wait_mode_t mode, float timeout)
{
    CHECK(port < 2);
    physical_input = port;
    input_mode = mode;
    input_timeout = timeout;
    read_calls++;
    return 1;
}
static inline void prepare_ioports(void)
{
    static io_ports_data_t ports = {.in = {.n_ports = 2}, .out = {.n_ports = 2}};
    static io_digital_t digital = {.ports = &ports, .get_pin_info = pin_info,
        .set_pin_description = pin_description, .digital_out = pin_output,
        .wait_on_input = pin_read};
    engine_prepare();
    CHECK(ioports_add_digital(&digital));
}

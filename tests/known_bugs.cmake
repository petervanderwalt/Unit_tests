# Confirmed defects: keep each regression and remove its block when fixed.
# Dates, reproductions, and removal instructions: KNOWN_BUGS.md

if(TEST utf8.ascii)
  set_tests_properties(utf8.ascii PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST nuts_bolts.read_uint_decimal_zeros)
  set_tests_properties(nuts_bolts.read_uint_decimal_zeros PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST nuts_bolts.read_uint_maximum)
  set_tests_properties(nuts_bolts.read_uint_maximum PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST nuts_bolts.datetime_century_leap)
  set_tests_properties(nuts_bolts.datetime_century_leap PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST fs_ram.close_lifetime)
  set_tests_properties(fs_ram.close_lifetime PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST ngc_params.named_builtin_read_only)
  set_tests_properties(ngc_params.named_builtin_read_only PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST fs_embedded.seek_within_file)
  set_tests_properties(fs_embedded.seek_within_file PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST stepper2.finite_polled_move)
  set_tests_properties(stepper2.finite_polled_move PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST stream_json.escaped_quote)
  set_tests_properties(stream_json.escaped_quote PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST fs_device.read_byte_count)
  set_tests_properties(fs_device.read_byte_count PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST gcode.quadratic_nonzero_j_accepted)
  set_tests_properties(gcode.quadratic_nonzero_j_accepted PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST delta.unreachable_cartesian_target_rejected)
  set_tests_properties(delta.unreachable_cartesian_target_rejected PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST rtcp_ac.rotary_segment_endpoint)
  set_tests_properties(rtcp_ac.rotary_segment_endpoint PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST protocol.serial_delete_edits_block)
  set_tests_properties(protocol.serial_delete_edits_block PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST gcode.m66_large_port_rejected)
  set_tests_properties(gcode.m66_large_port_rejected PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST gcode.m68_preserves_fractional_output)
  set_tests_properties(gcode.m68_preserves_fractional_output PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST ngc_expr.name_over_maximum_rejected)
  set_tests_properties(ngc_expr.name_over_maximum_rejected PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST ngc_params.absolute_position_parameter_inches)
  set_tests_properties(ngc_params.absolute_position_parameter_inches PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST multi_spindle.report_machine_format_shows_enabled_slots)
  set_tests_properties(multi_spindle.report_machine_format_shows_enabled_slots PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST multi_spindle.gcode_m5_broadcast_stops_enabled_spindles AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set_tests_properties(multi_spindle.gcode_m5_broadcast_stops_enabled_spindles PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST ngc_params.absolute_position_metric_ignores_display_inches)
  set_tests_properties(ngc_params.absolute_position_metric_ignores_display_inches PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

if(TEST settings.pulse_width_minimum_reports_specific_error)
  set_tests_properties(settings.pulse_width_minimum_reports_specific_error PROPERTIES WILL_FAIL TRUE LABELS known_bug)
endif()

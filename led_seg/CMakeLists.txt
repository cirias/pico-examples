add_executable(led_seg
        led_seg.c
        )

# pull in common dependencies
target_link_libraries(led_seg pico_stdlib)

# # enable usb output, disable uart output
pico_enable_stdio_usb(led_seg 1)
pico_enable_stdio_uart(led_seg 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(led_seg)

# add url via pico_set_program_url
example_auto_set_url(led_seg)

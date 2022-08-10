/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

/* #define MAX_COUNT  */

#define DIGITS 4

#define TICK_100US 10

#define FIRST_DIG_GPIO 2
#define FIRST_SEG_GPIO 6

/*
  Our 7 Segment display has pins as follows:

  --A--
  F   B
  --G--
  E   C
  --D--

  A: 11 - 6 = 5
  B: 13 - 6 = 7
  C:  7 - 6 = 1
  D:  9 - 6 = 3
  E: 10 - 6 = 4
  F: 12 - 6 = 6
  G:  6 - 6 = 0

  By default we are allocating GPIO 2 to segment A, 3 to B etc.
  So, connect GPIO 2 to pin A on the 7 segment LED display etc. Don't forget
  the appropriate resistors, best to use one for each segment!

  Connect button so that pressing the switch connects the GPIO 9 (default) to
  ground (pull down)
*/

int bits[10] = {
  0xfa,  // 0
  0x82,  // 1
  0xb9,  // 2
  0xab,  // 3
  0xc3,  // 4
  0x6b,  // 5
  0x7b,  // 6
  0xa2,  // 7
  0xfb,  // 8
  0xeb   // 9
};

int dividers[4] = {
  1,
  10,
  100,
  1000
};

bool timer_callback(repeating_timer_t *rt);

int main() {
    stdio_init_all();

    // initialize all pins at once
    uint32_t mask = 0x0fu << FIRST_DIG_GPIO | 0xffu << FIRST_SEG_GPIO;
    gpio_init_mask(mask);
    gpio_set_dir_out_masked(mask);

    for (int i = 0; i < 8; i++) {
        // Our bitmap above has a bit set where we need an LED on, BUT, we are pulling low to light
        // so invert our output
        gpio_set_outover(FIRST_SEG_GPIO + i, GPIO_OVERRIDE_INVERT);

        /*
         * // Use the max drive strength available
         * gpio_set_drive_strength(FIRST_SEG_GPIO + i, GPIO_DRIVE_STRENGTH_12MA);
         */
    }

    /*
     * for (int i = 0; i < 4; i++) {
     *     // Use the max drive strength available
     *     gpio_set_drive_strength(FIRST_DIG_GPIO + i, GPIO_DRIVE_STRENGTH_12MA);
     * }
     */

    repeating_timer_t timer;

    int count = 0;

    // negative timeout means exact delay (rather than delay between callbacks)
    if (!add_repeating_timer_us(-100 / DIGITS, timer_callback, &count, &timer)) {
        printf("Failed to add timer\n");
        return 1;
    }

    while (true) {
       tight_loop_contents();
    }
}

bool timer_callback(repeating_timer_t *rt) {
    int* count = (int*)rt->user_data;
    int digit_idx = *count % DIGITS;
    int num = *count / (DIGITS * TICK_100US);
    int digit = (num / dividers[digit_idx]) % 10;

    digit_idx = (digit_idx + DIGITS - 1) % DIGITS;

    gpio_clr_mask(0x0fu << FIRST_DIG_GPIO);
    gpio_put_masked(0xffu << FIRST_SEG_GPIO, bits[digit] << FIRST_SEG_GPIO);
    gpio_put_masked(0x0fu << FIRST_DIG_GPIO, (1u << digit_idx) << FIRST_DIG_GPIO);

    *count = ((*count) + 1) % (DIGITS * TICK_100US * 10000);

    return true; // keep repeating
}

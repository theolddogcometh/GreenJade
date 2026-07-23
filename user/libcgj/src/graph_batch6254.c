/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6254: poll interval microseconds from Hz.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_poll_interval_us_6254(uint32_t hz);
 *     - Soft poll interval in microseconds: 1000000 / hz when hz != 0,
 *       else 0. Integer-only inverse of poll rate. Does not schedule
 *       poll loops.
 *   uint32_t __gj_input_poll_interval_us_6254  (alias)
 *   __libcgj_batch6254_marker = "libcgj-batch6254"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_poll_interval_us_6254 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6254_marker[] = "libcgj-batch6254";

#define B6254_US_PER_S  1000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6254_interval_us(uint32_t u32Hz)
{
	if (u32Hz == 0u)
		return 0u;
	return B6254_US_PER_S / u32Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_poll_interval_us_6254 - poll interval from Hz (us).
 *
 * hz: soft poll/report rate in Hertz
 *
 * Returns 1000000/hz for nonzero rates, else 0. Soft pure-data
 * interval. No parent wires.
 */
uint32_t
gj_input_poll_interval_us_6254(uint32_t hz)
{
	(void)NULL;
	return b6254_interval_us(hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_poll_interval_us_6254(uint32_t hz)
    __attribute__((alias("gj_input_poll_interval_us_6254")));

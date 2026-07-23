/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5205: timer Hz to period nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timer_hz_to_period_ns_u(uint64_t hz);
 *     - Return floor(1000000000 / hz) nanoseconds per cycle.
 *       hz == 0 → 0 (invalid frequency).
 *   uint64_t __gj_timer_hz_to_period_ns_u  (alias)
 *   __libcgj_batch5205_marker = "libcgj-batch5205"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_timer_hz_to_period_ns_u surface only; no multi-def.
 * Distinct from gj_hz_to_period_ns_u (batch3139) and
 * gj_hz_to_period_ns_u2 (batch4377). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5205_marker[] = "libcgj-batch5205";

/* Nanoseconds in one second. */
#define B5205_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5205_hz_to_period_ns(uint64_t u64Hz)
{
	if (u64Hz == 0ull) {
		return 0ull;
	}
	return B5205_NS_PER_S / u64Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timer_hz_to_period_ns_u - convert timer frequency to period (ns).
 *
 * hz: cycles per second
 *
 * Returns floor(1e9 / hz). hz == 0 → 0. Self-contained; no parent wires.
 */
uint64_t
gj_timer_hz_to_period_ns_u(uint64_t u64Hz)
{
	(void)NULL;
	return b5205_hz_to_period_ns(u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timer_hz_to_period_ns_u(uint64_t u64Hz)
    __attribute__((alias("gj_timer_hz_to_period_ns_u")));

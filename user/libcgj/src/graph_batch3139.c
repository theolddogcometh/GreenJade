/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3139: frequency (Hz) to period nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hz_to_period_ns_u(uint64_t hz);
 *     - Return floor(1000000000 / hz) nanoseconds per cycle.
 *       hz == 0 → 0 (invalid frequency).
 *   uint64_t __gj_hz_to_period_ns_u  (alias)
 *   __libcgj_batch3139_marker = "libcgj-batch3139"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Unique gj_hz_to_period_ns_u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3139_marker[] = "libcgj-batch3139";

/* Nanoseconds in one second. */
#define B3139_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3139_hz_to_period_ns(uint64_t u64Hz)
{
	if (u64Hz == 0ull) {
		return 0ull;
	}
	return B3139_NS_PER_S / u64Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hz_to_period_ns_u - convert frequency in Hz to period in nanoseconds.
 *
 * u64Hz: cycles per second
 *
 * Returns floor(1e9 / hz). hz == 0 → 0.
 */
uint64_t
gj_hz_to_period_ns_u(uint64_t u64Hz)
{
	(void)NULL;
	return b3139_hz_to_period_ns(u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hz_to_period_ns_u(uint64_t u64Hz)
    __attribute__((alias("gj_hz_to_period_ns_u")));

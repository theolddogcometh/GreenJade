/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4377: frequency (Hz) to period nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hz_to_period_ns_u2(uint64_t hz);
 *     - Return floor(1000000000 / hz) nanoseconds per cycle.
 *       hz == 0 → 0 (invalid frequency).
 *   uint64_t __gj_hz_to_period_ns_u2  (alias)
 *   __libcgj_batch4377_marker = "libcgj-batch4377"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Note: symbol name also
 * appears in batch3139 with identical semantics — coordinate link set
 * to avoid multi-def if both TUs are archived together. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4377_marker[] = "libcgj-batch4377";

/* Nanoseconds in one second. */
#define B4377_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4377_hz_to_period_ns(uint64_t u64Hz)
{
	if (u64Hz == 0ull) {
		return 0ull;
	}
	return B4377_NS_PER_S / u64Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hz_to_period_ns_u2 - convert frequency in Hz to period in nanoseconds.
 *
 * hz: cycles per second
 *
 * Returns floor(1e9 / hz). hz == 0 → 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_hz_to_period_ns_u2(uint64_t u64Hz)
{
	(void)NULL;
	return b4377_hz_to_period_ns(u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hz_to_period_ns_u2(uint64_t u64Hz)
    __attribute__((alias("gj_hz_to_period_ns_u2")));

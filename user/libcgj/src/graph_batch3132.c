/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3132: microseconds to whole milliseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_us_to_ms_u(uint64_t us);
 *     - Return us / 1000 (truncating toward zero).
 *   uint64_t __gj_time_us_to_ms_u  (alias)
 *   __libcgj_batch3132_marker = "libcgj-batch3132"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_time_us_to_ms (batch1034) — unique _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3132_marker[] = "libcgj-batch3132";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3132_us_to_ms(uint64_t u64Us)
{
	return u64Us / 1000ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_us_to_ms_u - convert microseconds to whole milliseconds.
 *
 * u64Us: microseconds
 *
 * Integer divide by 1000; remainder discarded.
 */
uint64_t
gj_time_us_to_ms_u(uint64_t u64Us)
{
	(void)NULL;
	return b3132_us_to_ms(u64Us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_us_to_ms_u(uint64_t u64Us)
    __attribute__((alias("gj_time_us_to_ms_u")));

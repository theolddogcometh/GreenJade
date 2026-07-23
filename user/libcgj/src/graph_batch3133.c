/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3133: seconds to milliseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_s_to_ms_u(uint64_t s);
 *     - Return s * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_time_s_to_ms_u  (alias)
 *   __libcgj_batch3133_marker = "libcgj-batch3133"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_secs_to_ms (batch484) and gj_sec_to_ms (batch1973) — unique
 * gj_time_s_to_ms_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3133_marker[] = "libcgj-batch3133";

/* Floor(UINT64_MAX / 1000): largest s with s * 1000 still in range. */
#define B3133_MAX_SECS  (UINT64_MAX / 1000ull)

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3133_s_to_ms(uint64_t u64S)
{
	if (u64S > B3133_MAX_SECS) {
		return UINT64_MAX;
	}
	return u64S * 1000ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_s_to_ms_u - convert seconds to milliseconds with saturation.
 *
 * u64S: seconds
 *
 * Returns s * 1000 if the product fits in uint64_t, else UINT64_MAX.
 */
uint64_t
gj_time_s_to_ms_u(uint64_t u64S)
{
	(void)NULL;
	return b3133_s_to_ms(u64S);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_s_to_ms_u(uint64_t u64S)
    __attribute__((alias("gj_time_s_to_ms_u")));

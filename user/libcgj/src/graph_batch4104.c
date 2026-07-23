/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4104: saturating uint64_t subtraction.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_saturate_sub_u(uint64_t a, uint64_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_u64_saturate_sub_u  (alias)
 *   __libcgj_batch4104_marker = "libcgj-batch4104"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Distinct from
 * gj_sat_sub_u64 (batch377) and gj_u64_sat_sub (batch922) — unique
 * gj_u64_saturate_sub_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4104_marker[] = "libcgj-batch4104";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4104_saturate_sub(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ULL;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_saturate_sub_u - saturating unsigned 64-bit subtract.
 *
 * a, b: minuend and subtrahend
 *
 * Returns a - b, or 0 if a < b (underflow). No wraparound.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_saturate_sub_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4104_saturate_sub(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_saturate_sub_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_saturate_sub_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4103: saturating uint64_t addition.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_saturate_add_u(uint64_t a, uint64_t b);
 *     - a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_saturate_add_u  (alias)
 *   __libcgj_batch4103_marker = "libcgj-batch4103"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Distinct from
 * gj_sat_add_u64 (batch377) and gj_u64_sat_add (batch921) — unique
 * gj_u64_saturate_add_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4103_marker[] = "libcgj-batch4103";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4103_saturate_add(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_saturate_add_u - saturating unsigned 64-bit add.
 *
 * a, b: addends
 *
 * Returns a + b, or UINT64_MAX if the true sum would overflow.
 * Detects overflow without wrapping: b > UINT64_MAX - a.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_saturate_add_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4103_saturate_add(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_saturate_add_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_saturate_add_u")));

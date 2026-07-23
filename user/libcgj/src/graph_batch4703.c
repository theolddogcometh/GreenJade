/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4703: saturating uint64_t three-operand add.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_add3_u(uint64_t a, uint64_t b, uint64_t c);
 *     - a + b + c with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_sat_add3_u  (alias)
 *   __libcgj_batch4703_marker = "libcgj-batch4703"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u64_sat_add (batch921) and gj_u32_sat_add3_u (batch4701) —
 * unique gj_u64_sat_add3_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4703_marker[] = "libcgj-batch4703";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b4703_sat_add(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* Saturating a + b + c via successive pairwise sat-add. */
static uint64_t
b4703_sat_add3(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	return b4703_sat_add(b4703_sat_add(u64A, u64B), u64C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_add3_u - unsigned 64-bit three-operand saturating add.
 *
 * a, b, c: addends
 *
 * Returns min(a + b + c, UINT64_MAX) without wrapping. Overflow is
 * detected pairwise without intermediate wrap.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_sat_add3_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b4703_sat_add3(u64A, u64B, u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_add3_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_sat_add3_u")));

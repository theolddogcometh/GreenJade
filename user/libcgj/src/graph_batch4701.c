/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4701: saturating uint32_t three-operand add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_add3_u(uint32_t a, uint32_t b, uint32_t c);
 *     - a + b + c with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_sat_add3_u  (alias)
 *   __libcgj_batch4701_marker = "libcgj-batch4701"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710: u32_sat_add3_u,
 * u32_sat_mul_add_u, u64_sat_add3_u, u32_sat_inc_u, u32_sat_dec_u,
 * u64_sat_inc_u, u64_sat_dec_u, u32_wrapping_neg_u, u32_abs_diff3_u,
 * batch_id_4710). Distinct from gj_u32_sat_add / gj_u32_sat_add_u
 * (batch923/2375) — unique gj_u32_sat_add3_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4701_marker[] = "libcgj-batch4701";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint32_t domain. */
static uint32_t
b4701_sat_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* Saturating a + b + c via successive pairwise sat-add. */
static uint32_t
b4701_sat_add3(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	return b4701_sat_add(b4701_sat_add(u32A, u32B), u32C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_add3_u - unsigned 32-bit three-operand saturating add.
 *
 * a, b, c: addends
 *
 * Returns min(a + b + c, UINT32_MAX) without wrapping. Overflow is
 * detected pairwise without intermediate wrap.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_sat_add3_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b4701_sat_add3(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_add3_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_sat_add3_u")));

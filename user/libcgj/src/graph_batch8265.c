/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8265: saturating uint32_t decrement.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sat_dec_u32_8265(uint32_t v);
 *     - v - 1 with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_sat_dec_u32_8265  (alias)
 *   __libcgj_batch8265_marker = "libcgj-batch8265"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_sat_dec_u32_8265
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8265_marker[] = "libcgj-batch8265";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating unsigned 32-bit decrement. Zero stays zero.
 */
static uint32_t
b8265_sat_dec(uint32_t u32V)
{
	if (u32V == 0u) {
		return 0u;
	}
	return u32V - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_dec_u32_8265 - unsigned -1; clamp to 0 on underflow.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_sat_dec_u32_8265(uint32_t u32V)
{
	(void)NULL;
	return b8265_sat_dec(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sat_dec_u32_8265(uint32_t u32V)
    __attribute__((alias("gj_sat_dec_u32_8265")));

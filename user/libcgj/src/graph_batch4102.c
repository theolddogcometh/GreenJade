/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4102: saturating uint32_t subtraction.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_saturate_sub_u(uint32_t a, uint32_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_u32_saturate_sub_u  (alias)
 *   __libcgj_batch4102_marker = "libcgj-batch4102"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Distinct from
 * gj_u32_sat_sub (batch924) — unique gj_u32_saturate_sub_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4102_marker[] = "libcgj-batch4102";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4102_saturate_sub(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_saturate_sub_u - saturating unsigned 32-bit subtract.
 *
 * a, b: minuend and subtrahend
 *
 * Returns a - b, or 0 if a < b (underflow). No wraparound.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_saturate_sub_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4102_saturate_sub(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_saturate_sub_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_saturate_sub_u")));

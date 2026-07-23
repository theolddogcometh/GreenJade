/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8262: saturating uint32_t subtract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sat_sub_u32_8262(uint32_t a, uint32_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_sat_sub_u32_8262  (alias)
 *   __libcgj_batch8262_marker = "libcgj-batch8262"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_sat_sub_u32_8262
 * surface only; no multi-def. Distinct from gj_sat_sub_u64
 * (batch377). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8262_marker[] = "libcgj-batch8262";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating unsigned 32-bit subtract. Underflow clamps to 0.
 */
static uint32_t
b8262_sat_sub(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_sub_u32_8262 - unsigned subtract; clamp to 0 if a < b
 * (underflow).
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_sat_sub_u32_8262(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8262_sat_sub(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sat_sub_u32_8262(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_sat_sub_u32_8262")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8269: uint32_t absolute difference.
 *
 * Surface (unique symbols):
 *   uint32_t gj_absdiff_u32_8269(uint32_t a, uint32_t b);
 *     - |a - b| as uint32_t (always non-wrapping unsigned distance).
 *   uint32_t __gj_absdiff_u32_8269  (alias)
 *   __libcgj_batch8269_marker = "libcgj-batch8269"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_absdiff_u32_8269
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8269_marker[] = "libcgj-batch8269";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Unsigned absolute difference: larger minus smaller.
 */
static uint32_t
b8269_absdiff(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_absdiff_u32_8269 - |a - b| for uint32_t operands.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_absdiff_u32_8269(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8269_absdiff(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_absdiff_u32_8269(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_absdiff_u32_8269")));

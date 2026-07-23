/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8268: uint32_t maximum of two values.
 *
 * Surface (unique symbols):
 *   uint32_t gj_max_u32_8268(uint32_t a, uint32_t b);
 *     - Larger of a and b (equal → b).
 *   uint32_t __gj_max_u32_8268  (alias)
 *   __libcgj_batch8268_marker = "libcgj-batch8268"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_max_u32_8268 surface
 * only; no multi-def. Distinct from bare gj_max_u32 (batch835).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8268_marker[] = "libcgj-batch8268";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8268_max(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max_u32_8268 - return the greater of two uint32_t values.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_max_u32_8268(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8268_max(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_max_u32_8268(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_max_u32_8268")));

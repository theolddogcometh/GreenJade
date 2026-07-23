/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8267: uint32_t minimum of two values.
 *
 * Surface (unique symbols):
 *   uint32_t gj_min_u32_8267(uint32_t a, uint32_t b);
 *     - Smaller of a and b (equal → b).
 *   uint32_t __gj_min_u32_8267  (alias)
 *   __libcgj_batch8267_marker = "libcgj-batch8267"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_min_u32_8267 surface
 * only; no multi-def. Distinct from bare gj_min_u32 (batch834).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8267_marker[] = "libcgj-batch8267";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8267_min(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_min_u32_8267 - return the lesser of two uint32_t values.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_min_u32_8267(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8267_min(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_min_u32_8267(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_min_u32_8267")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch834: uint32_t minimum of two values.
 *
 * Surface (unique symbols):
 *   uint32_t gj_min_u32(uint32_t a, uint32_t b);
 *     — Smaller of a and b (equal → b).
 *   uint32_t __gj_min_u32  (alias)
 *   __libcgj_batch834_marker = "libcgj-batch834"
 *
 * Distinct from signed min surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch834_marker[] = "libcgj-batch834";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_min_u32 — return the lesser of two uint32_t values.
 */
uint32_t
gj_min_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_min_u32(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_min_u32")));

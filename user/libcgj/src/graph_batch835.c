/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch835: uint32_t maximum of two values.
 *
 * Surface (unique symbols):
 *   uint32_t gj_max_u32(uint32_t a, uint32_t b);
 *     — Larger of a and b (equal → b).
 *   uint32_t __gj_max_u32  (alias)
 *   __libcgj_batch835_marker = "libcgj-batch835"
 *
 * Distinct from gj_min_u32 (batch834) and signed max surfaces —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch835_marker[] = "libcgj-batch835";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max_u32 — return the greater of two uint32_t values.
 */
uint32_t
gj_max_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_max_u32(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_max_u32")));

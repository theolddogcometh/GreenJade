/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch924: saturating uint32_t subtract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_sub(uint32_t a, uint32_t b);
 *     — a - b with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_u32_sat_sub  (alias)
 *   __libcgj_batch924_marker = "libcgj-batch924"

 *
 * Does NOT define gj_sat_sub_u64 / gj_u64_sat_sub — 32-bit surface only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch924_marker[] = "libcgj-batch924";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_sub — unsigned 32-bit subtract; clamp to 0 on underflow.
 */
uint32_t
gj_u32_sat_sub(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_sub(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sat_sub")));

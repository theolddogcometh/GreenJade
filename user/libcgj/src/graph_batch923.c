/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch923: saturating uint32_t add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_add(uint32_t a, uint32_t b);
 *     — a + b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_u32_sat_add  (alias)
 *   __libcgj_batch923_marker = "libcgj-batch923"

 *
 * Does NOT define gj_sat_add_u64 / gj_u64_sat_add — 32-bit surface only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch923_marker[] = "libcgj-batch923";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_add — unsigned 32-bit add; clamp to UINT32_MAX on overflow.
 */
uint32_t
gj_u32_sat_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_add(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sat_add")));

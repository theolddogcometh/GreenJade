/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4422: SIMD-ish scalar uint32_t max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_max_u2(uint32_t a, uint32_t b);
 *     - Greater of a and b (equal → b).
 *   uint32_t __gj_u32_max_u2  (alias)
 *   __libcgj_batch4422_marker = "libcgj-batch4422"
 *
 * Exclusive continuum CREATE-ONLY (4421-4430: u32_min_u, u32_max_u,
 * u64_min_u, u64_max_u, u32_select_u, u32_cond_assign_u, u8_avg_u,
 * u16_avg_u, u32_midpoint_u, batch_id_4430). SIMD-ish scalar unique
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4422_marker[] = "libcgj-batch4422";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4422_max(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_max_u2 - return the greater of two uint32_t values.
 *
 * a, b: operands
 *
 * Returns the larger value; when equal, returns b.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_max_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4422_max(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_max_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_max_u2")));

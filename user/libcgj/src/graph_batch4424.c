/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4424: SIMD-ish scalar uint64_t max.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_max_u2(uint64_t a, uint64_t b);
 *     - Greater of a and b (equal → b).
 *   uint64_t __gj_u64_max_u2  (alias)
 *   __libcgj_batch4424_marker = "libcgj-batch4424"
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

const char __libcgj_batch4424_marker[] = "libcgj-batch4424";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4424_max(uint64_t u64A, uint64_t u64B)
{
	if (u64A > u64B) {
		return u64A;
	}
	return u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_max_u2 - return the greater of two uint64_t values.
 *
 * a, b: operands
 *
 * Returns the larger value; when equal, returns b.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_max_u2(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4424_max(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_max_u2(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_max_u2")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4428: SIMD-ish scalar uint16_t floor average.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_avg_u2(uint16_t a, uint16_t b);
 *     - Floor of (a + b) / 2 with args promoted before add.
 *   uint16_t __gj_u16_avg_u2  (alias)
 *   __libcgj_batch4428_marker = "libcgj-batch4428"
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

const char __libcgj_batch4428_marker[] = "libcgj-batch4428";

/* ---- freestanding helpers ---------------------------------------------- */

/* Promote to uint32_t, floor average via >> 1, narrow back to uint16_t. */
static uint16_t
b4428_avg(uint16_t u16A, uint16_t u16B)
{
	return (uint16_t)(((uint32_t)u16A + (uint32_t)u16B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_avg_u2 - floor((a + b) / 2) with promoted add.
 *
 * a, b: uint16_t operands (promoted to uint32_t for the sum)
 *
 * Returns (uint16_t)(((uint32_t)a + (uint32_t)b) >> 1).
 * Self-contained; no parent wires.
 */
uint16_t
gj_u16_avg_u2(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	return b4428_avg(u16A, u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_avg_u2(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_avg_u2")));

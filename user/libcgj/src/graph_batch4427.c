/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4427: SIMD-ish scalar uint8_t floor average.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_avg_u2(uint8_t a, uint8_t b);
 *     - Floor of (a + b) / 2 with args promoted before add.
 *   uint8_t __gj_u8_avg_u2  (alias)
 *   __libcgj_batch4427_marker = "libcgj-batch4427"
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

const char __libcgj_batch4427_marker[] = "libcgj-batch4427";

/* ---- freestanding helpers ---------------------------------------------- */

/* Promote to uint32_t, floor average via >> 1, narrow back to uint8_t. */
static uint8_t
b4427_avg(uint8_t u8A, uint8_t u8B)
{
	return (uint8_t)(((uint32_t)u8A + (uint32_t)u8B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_avg_u2 - floor((a + b) / 2) with promoted add.
 *
 * a, b: uint8_t operands (promoted to uint32_t for the sum)
 *
 * Returns (uint8_t)(((uint32_t)a + (uint32_t)b) >> 1).
 * Self-contained; no parent wires.
 */
uint8_t
gj_u8_avg_u2(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b4427_avg(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_avg_u2(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_avg_u2")));

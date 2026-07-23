/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4429: SIMD-ish scalar u32 overflow-safe midpoint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_midpoint_u2(uint32_t a, uint32_t b);
 *     - Floor average of a and b without intermediate overflow.
 *   uint32_t __gj_u32_midpoint_u2  (alias)
 *   __libcgj_batch4429_marker = "libcgj-batch4429"
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

const char __libcgj_batch4429_marker[] = "libcgj-batch4429";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint32_t
b4429_mid(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_midpoint_u2 - overflow-safe floor midpoint of two uint32_t values.
 *
 * a, b: endpoints (order does not matter: mid(a,b) == mid(b,a))
 *
 * Returns floor((a + b) / 2) for all inputs, including when a + b
 * would wrap as a plain uint32_t sum.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_midpoint_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4429_mid(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_midpoint_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_midpoint_u2")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4410: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4410(void);
 *     - Returns the compile-time graph batch number for this TU (4410).
 *   uint32_t __gj_batch_id_4410  (alias)
 *   __libcgj_batch4410_marker = "libcgj-batch4410"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410: u32_mul_hi_u2,
 * u64_mul_hi_soft_u, u32_mul_sat_u, u64_mul_sat_u, u32_div_ceil_u2,
 * u64_div_ceil_u3, u32_div_floor_u2, u32_mod_safe_u, u64_mod_safe_u,
 * batch_id_4410). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4410_marker[] = "libcgj-batch4410";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4410_id(void)
{
	return 4410u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4410 - report this TU's graph batch number.
 *
 * Always returns 4410. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4410(void)
{
	(void)NULL;
	return b4410_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4410(void)
    __attribute__((alias("gj_batch_id_4410")));

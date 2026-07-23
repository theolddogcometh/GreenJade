/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4160: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4160(void);
 *     - Returns the compile-time graph batch number for this TU (4160).
 *   uint32_t __gj_batch_id_4160  (alias)
 *   __libcgj_batch4160_marker = "libcgj-batch4160"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160: q16x_mul_u, q16x_div_u,
 * q16x_add_sat_u, q16x_sub_sat_u, q16x_from_u32_u, q16x_to_u32_floor_u,
 * q16x_frac_u, q16x_lerp_u, q16x_clamp01_u, batch_id_4160).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4160_marker[] = "libcgj-batch4160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4160_id(void)
{
	return 4160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4160 - report this TU's graph batch number.
 *
 * Always returns 4160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4160(void)
{
	(void)NULL;
	return b4160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4160(void)
    __attribute__((alias("gj_batch_id_4160")));

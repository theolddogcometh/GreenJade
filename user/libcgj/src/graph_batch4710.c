/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4710: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4710(void);
 *     - Returns the compile-time graph batch number for this TU (4710).
 *   uint32_t __gj_batch_id_4710  (alias)
 *   __libcgj_batch4710_marker = "libcgj-batch4710"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710: u32_sat_add3_u,
 * u32_sat_mul_add_u, u64_sat_add3_u, u32_sat_inc_u, u32_sat_dec_u,
 * u64_sat_inc_u, u64_sat_dec_u, u32_wrapping_neg_u, u32_abs_diff3_u,
 * batch_id_4710). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4710_marker[] = "libcgj-batch4710";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4710_id(void)
{
	return 4710u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4710 - report this TU's graph batch number.
 *
 * Always returns 4710. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4710(void)
{
	(void)NULL;
	return b4710_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4710(void)
    __attribute__((alias("gj_batch_id_4710")));

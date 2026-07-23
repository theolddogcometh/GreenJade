/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4210: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4210(void);
 *     - Returns the compile-time graph batch number for this TU (4210).
 *   uint32_t __gj_batch_id_4210  (alias)
 *   __libcgj_batch4210_marker = "libcgj-batch4210"
 *
 * Exclusive continuum CREATE-ONLY (4201-4210: u32_gcd_u, u64_gcd_u,
 * u32_lcm_u, u32_is_pow2_u, u64_is_pow2_u, u32_next_pow2_u,
 * u32_prev_pow2_u, u32_ilog2_u, u64_ilog2_u, batch_id_4210).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4210_marker[] = "libcgj-batch4210";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4210_id(void)
{
	return 4210u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4210 - report this TU's graph batch number.
 *
 * Always returns 4210. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4210(void)
{
	(void)NULL;
	return b4210_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4210(void)
    __attribute__((alias("gj_batch_id_4210")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4060: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4060(void);
 *     - Returns the compile-time graph batch number for this TU (4060).
 *   uint32_t __gj_batch_id_4060  (alias)
 *   __libcgj_batch4060_marker = "libcgj-batch4060"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave
 * (batches 4051-4059: u32_avg_trunc_u, u64_avg_trunc_u,
 * u32_avg_round_half_u, u64_avg_round_half_u, u32_lerp_unorm_u,
 * u64_lerp_unorm_u, u32_inv_lerp_u, u64_inv_lerp_u, u32_remap_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4060_marker[] = "libcgj-batch4060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4060_id(void)
{
	return 4060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4060 - report this TU's graph batch number.
 *
 * Always returns 4060. No parent wires.
 */
uint32_t
gj_batch_id_4060(void)
{
	(void)NULL;
	return b4060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4060(void)
    __attribute__((alias("gj_batch_id_4060")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3920: milestone 3920 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3920(void);
 *     - Returns the compile-time graph batch number for this TU (3920).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3911-3919: u64_divisible_u / u32_multiple_u /
 *       u64_multiple_u / u32_coprime_u / u64_coprime_u /
 *       u32_phi_euler_u / u32_sigma_sum_u / u32_tau_count_u /
 *       u32_radical_u).
 *   uint32_t __gj_batch_id_3920  (alias)
 *   __libcgj_batch3920_marker = "libcgj-batch3920"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3920_marker[] = "libcgj-batch3920";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3920_id(void)
{
	return 3920u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3920 - report this TU's graph batch number.
 *
 * Always returns 3920.
 */
uint32_t
gj_batch_id_3920(void)
{
	(void)NULL;
	return b3920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3920(void)
    __attribute__((alias("gj_batch_id_3920")));

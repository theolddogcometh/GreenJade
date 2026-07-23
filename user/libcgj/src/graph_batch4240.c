/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4240: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4240(void);
 *     - Returns the compile-time graph batch number for this TU (4240).
 *   uint32_t __gj_batch_id_4240  (alias)
 *   __libcgj_batch4240_marker = "libcgj-batch4240"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4240_marker[] = "libcgj-batch4240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4240_id(void)
{
	return 4240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4240 - report this TU's graph batch number.
 *
 * Always returns 4240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_4240(void)
{
	(void)NULL;
	return b4240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4240(void)
    __attribute__((alias("gj_batch_id_4240")));

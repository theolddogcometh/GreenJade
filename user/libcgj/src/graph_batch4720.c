/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4720: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4720(void);
 *     - Returns the compile-time graph batch number for this TU (4720).
 *   uint32_t __gj_batch_id_4720  (alias)
 *   __libcgj_batch4720_marker = "libcgj-batch4720"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720: hash_u32_seed_u,
 * hash_u64_seed_u, hash_combine_u, hash_mix_final32_u,
 * hash_mix_final64_u, hash_ptr_u, hash_size_u, hash_pair32_u,
 * hash_triple32_u, batch_id_4720). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4720_marker[] = "libcgj-batch4720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4720_id(void)
{
	return 4720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4720 - report this TU's graph batch number.
 *
 * Always returns 4720. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4720(void)
{
	(void)NULL;
	return b4720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4720(void)
    __attribute__((alias("gj_batch_id_4720")));

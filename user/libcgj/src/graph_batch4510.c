/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4510: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4510(void);
 *     - Returns the compile-time graph batch number for this TU (4510).
 *   uint32_t __gj_batch_id_4510  (alias)
 *   __libcgj_batch4510_marker = "libcgj-batch4510"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510: u32_rev_bits_u, u64_rev_bits_u,
 * u32_rev_bytes_u, u64_rev_bytes_u, u32_rev_nibbles_u, u32_swizzle_1357_u,
 * u32_pack_bool4_u, u32_unpack_bool_u, u32_mask_merge_u, batch_id_4510).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4510_marker[] = "libcgj-batch4510";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4510_id(void)
{
	return 4510u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4510 - report this TU's graph batch number.
 *
 * Always returns 4510. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4510(void)
{
	(void)NULL;
	return b4510_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4510(void)
    __attribute__((alias("gj_batch_id_4510")));

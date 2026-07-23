/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2740: milestone 2740 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2740(void);
 *     - Returns the compile-time graph batch number for this TU (2740).
 *   uint32_t __gj_batch_id_2740  (alias)
 *   __libcgj_batch2740_marker = "libcgj-batch2740"
 *
 * Milestone for the bitops exclusive wave (batches 2731-2739:
 * mask_high_u64, popcount_u32_u, popcount_u64_u, ctz_u32_u, ctz_u64_u,
 * clz_u32_u, clz_u64_u, parity_u32_u, parity_u64_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2740_marker[] = "libcgj-batch2740";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2740_id(void)
{
	return 2740u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2740 - report this TU's graph batch number.
 *
 * Always returns 2740. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2740(void)
{
	(void)NULL;
	return b2740_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2740(void)
    __attribute__((alias("gj_batch_id_2740")));

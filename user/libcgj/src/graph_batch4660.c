/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4660: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4660(void);
 *     - Returns the compile-time graph batch number for this TU (4660).
 *   uint32_t __gj_batch_id_4660  (alias)
 *   __libcgj_batch4660_marker = "libcgj-batch4660"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4660_marker[] = "libcgj-batch4660";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4660_id(void)
{
	return 4660u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4660 - report this TU's graph batch number.
 *
 * Always returns 4660. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4660(void)
{
	(void)NULL;
	return b4660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4660(void)
    __attribute__((alias("gj_batch_id_4660")));

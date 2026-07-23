/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3120(void);
 *     - Returns the compile-time graph batch number for this TU (3120).
 *   uint32_t __gj_batch_id_3120  (alias)
 *   __libcgj_batch3120_marker = "libcgj-batch3120"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120): u64_gt_mask_u,
 * u32_min_mask_u, u64_min_mask_u, u32_max_mask_u, u64_max_mask_u,
 * u8_pack4_u, u8_unpack4_u, u16_pack2_u, u16_unpack2_u, batch_id_3120.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3120_marker[] = "libcgj-batch3120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3120_id(void)
{
	return 3120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3120 - report this TU's graph batch number.
 *
 * Always returns 3120. No parent wires.
 */
uint32_t
gj_batch_id_3120(void)
{
	(void)NULL;
	return b3120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3120(void)
    __attribute__((alias("gj_batch_id_3120")));

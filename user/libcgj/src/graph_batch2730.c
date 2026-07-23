/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2730: milestone 2730 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2730(void);
 *     - Returns the compile-time graph batch number for this TU (2730).
 *   uint32_t __gj_batch_id_2730  (alias)
 *   __libcgj_batch2730_marker = "libcgj-batch2730"
 *
 * Milestone for the bitops exclusive wave (batches 2721-2729:
 * bit_test_u32, bit_set_u32, bit_clr_u32, bit_flip_u32, bit_test_u64_u,
 * bit_set_u64_u, bit_clr_u64, bit_flip_u64, mask_low_u64_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2730_marker[] = "libcgj-batch2730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2730_id(void)
{
	return 2730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2730 - report this TU's graph batch number.
 *
 * Always returns 2730. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2730(void)
{
	(void)NULL;
	return b2730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2730(void)
    __attribute__((alias("gj_batch_id_2730")));

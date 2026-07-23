/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3840: milestone 3840 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3840(void);
 *     - Returns the compile-time graph batch number for this TU (3840).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3831-3839: u16_to_le_u / u32_to_be_u / u32_to_le_u /
 *       u64_to_be_u / u64_to_le_u / checksum_sum16_u / checksum_sum32_u /
 *       checksum_xor16_u / checksum_xor32_u).
 *   uint32_t __gj_batch_id_3840  (alias)
 *   __libcgj_batch3840_marker = "libcgj-batch3840"
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

const char __libcgj_batch3840_marker[] = "libcgj-batch3840";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3840_id(void)
{
	return 3840u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3840 — report this TU's graph batch number.
 *
 * Always returns 3840. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_3840(void)
{
	(void)NULL;
	return b3840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3840(void)
    __attribute__((alias("gj_batch_id_3840")));

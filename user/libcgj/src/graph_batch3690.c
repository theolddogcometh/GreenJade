/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3690: milestone 3690 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3690(void);
 *     - Returns the compile-time graph batch number for this TU (3690).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3681-3689: varint_u64_len_u / zigzag_encode_i32_u /
 *       zigzag_decode_u32_u / zigzag_encode_i64_u / zigzag_decode_u64_u /
 *       leb128_u32_len_u / leb128_u64_len_u / sleb128_i32_len_u /
 *       sleb128_i64_len_u).
 *   uint32_t __gj_batch_id_3690  (alias)
 *   __libcgj_batch3690_marker = "libcgj-batch3690"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3690_marker[] = "libcgj-batch3690";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3690_id(void)
{
	return 3690u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3690 — report this TU's graph batch number.
 *
 * Always returns 3690. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3690(void)
{
	(void)NULL;
	return b3690_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3690(void)
    __attribute__((alias("gj_batch_id_3690")));

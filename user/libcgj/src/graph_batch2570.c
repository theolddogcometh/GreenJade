/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2570: milestone 2570 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2570(void);
 *     — Returns the compile-time graph batch number for this TU (2570).
 *       Milestone for the compression exclusive series
 *       (batches 2561–2569: rle_run_len_u / rle_bound_u / rle_encode_u /
 *       rle_decode_u / delta_encode_u32 / delta_decode_u32 /
 *       zigzag_encode_i32 / zigzag_decode_i32 / varint_u64_len).
 *   uint32_t __gj_batch_id_2570  (alias)
 *   __libcgj_batch2570_marker = "libcgj-batch2570"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2570_marker[] = "libcgj-batch2570";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2570_id(void)
{
	return 2570u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2570 — report this TU's graph batch number.
 *
 * Always returns 2570. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2570(void)
{
	(void)NULL;
	return b2570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2570(void)
    __attribute__((alias("gj_batch_id_2570")));

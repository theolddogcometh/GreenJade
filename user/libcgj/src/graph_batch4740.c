/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4740: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4740(void);
 *     - Returns the compile-time graph batch number for this TU (4740).
 *   uint32_t __gj_batch_id_4740  (alias)
 *   __libcgj_batch4740_marker = "libcgj-batch4740"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Protocol framing wave; no parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4740_marker[] = "libcgj-batch4740";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4740_id(void)
{
	return 4740u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4740 - report this TU's graph batch number.
 *
 * Always returns 4740. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4740(void)
{
	(void)NULL;
	return b4740_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4740(void)
    __attribute__((alias("gj_batch_id_4740")));

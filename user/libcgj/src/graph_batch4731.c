/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4731: fixed protocol frame header size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_hdr_size_u(void);
 *     - Returns the fixed on-wire frame header size in bytes (8).
 *   uint32_t __gj_frame_hdr_size_u  (alias)
 *   __libcgj_batch4731_marker = "libcgj-batch4731"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_hdr_size_u surface only; no multi-def. Protocol framing
 * wave; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4731_marker[] = "libcgj-batch4731";

/* Fixed protocol frame header length (bytes). */
#define B4731_HDR_SIZE  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4731_hdr_size(void)
{
	return B4731_HDR_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_hdr_size_u - fixed protocol frame header size in bytes.
 *
 * Always returns 8. Pair with gj_frame_total_u / gj_frame_payload_len_u
 * for length accounting. Self-contained; no parent wires.
 */
uint32_t
gj_frame_hdr_size_u(void)
{
	(void)NULL;
	return b4731_hdr_size();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_hdr_size_u(void)
    __attribute__((alias("gj_frame_hdr_size_u")));

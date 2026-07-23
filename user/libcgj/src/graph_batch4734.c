/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4734: payload length from total frame size.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_payload_len_u(uint32_t total);
 *     - Return total - 8 when total >= 8, else 0 (undersized frame).
 *   uint32_t __gj_frame_payload_len_u  (alias)
 *   __libcgj_batch4734_marker = "libcgj-batch4734"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_payload_len_u surface only; no multi-def. Inverse of
 * gj_frame_total_u (batch4732) when total is valid. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4734_marker[] = "libcgj-batch4734";

/* Fixed protocol frame header length (bytes). */
#define B4734_HDR_SIZE  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4734_payload_len(uint32_t u32Total)
{
	if (u32Total < B4734_HDR_SIZE) {
		return 0u;
	}
	return u32Total - B4734_HDR_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_payload_len_u - payload bytes implied by a total frame length.
 *
 * total: total frame length in bytes (header + payload)
 *
 * Returns total - 8 when total >= 8; returns 0 when total is shorter
 * than the fixed header. Pair with gj_frame_valid_len_u (batch4733).
 * Self-contained; no parent wires.
 */
uint32_t
gj_frame_payload_len_u(uint32_t u32Total)
{
	(void)NULL;
	return b4734_payload_len(u32Total);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_payload_len_u(uint32_t u32Total)
    __attribute__((alias("gj_frame_payload_len_u")));

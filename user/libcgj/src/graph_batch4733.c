/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4733: validate total frame length vs header.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_valid_len_u(uint32_t total);
 *     - Return 1 if total >= 8 (room for the fixed header), else 0.
 *   uint32_t __gj_frame_valid_len_u  (alias)
 *   __libcgj_batch4733_marker = "libcgj-batch4733"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_valid_len_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4733_marker[] = "libcgj-batch4733";

/* Fixed protocol frame header length (bytes). */
#define B4733_HDR_SIZE  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4733_valid_len(uint32_t u32Total)
{
	if (u32Total >= B4733_HDR_SIZE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_valid_len_u - true when total length can hold the header.
 *
 * total: claimed total frame length in bytes
 *
 * Returns 1 if total >= 8, else 0. Gate before payload extraction via
 * gj_frame_payload_len_u (batch4734). Self-contained; no parent wires.
 */
uint32_t
gj_frame_valid_len_u(uint32_t u32Total)
{
	(void)NULL;
	return b4733_valid_len(u32Total);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_valid_len_u(uint32_t u32Total)
    __attribute__((alias("gj_frame_valid_len_u")));

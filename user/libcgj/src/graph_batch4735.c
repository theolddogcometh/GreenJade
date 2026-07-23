/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4735: CRC32 seed for frame integrity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_crc_seed_u(void);
 *     - Returns the standard CRC-32 seed 0xffffffff.
 *   uint32_t __gj_frame_crc_seed_u  (alias)
 *   __libcgj_batch4735_marker = "libcgj-batch4735"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_crc_seed_u surface only; no multi-def. Protocol framing
 * wave; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4735_marker[] = "libcgj-batch4735";

/* Standard CRC-32 initial seed (all ones). */
#define B4735_CRC_SEED  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4735_crc_seed(void)
{
	return B4735_CRC_SEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_crc_seed_u - CRC-32 seed for frame integrity checks.
 *
 * Always returns 0xffffffff. Use as the starting accumulator for a
 * frame CRC-32 over header and/or payload. Self-contained; no parent
 * wires.
 */
uint32_t
gj_frame_crc_seed_u(void)
{
	(void)NULL;
	return b4735_crc_seed();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_crc_seed_u(void)
    __attribute__((alias("gj_frame_crc_seed_u")));

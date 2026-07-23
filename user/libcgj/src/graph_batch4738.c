/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4738: set one bit in a frame flags word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_flags_set_u(uint32_t f, unsigned bit);
 *     - Return f with bit index bit set (0 = LSB). bit >= 32 leaves f
 *       unchanged (no shift UB).
 *   uint32_t __gj_frame_flags_set_u  (alias)
 *   __libcgj_batch4738_marker = "libcgj-batch4738"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_flags_set_u surface only; no multi-def. Distinct from
 * gj_flags_set_u (batch4581, mask OR) — bit-index form. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4738_marker[] = "libcgj-batch4738";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4738_flags_set(uint32_t u32F, unsigned uBit)
{
	if (uBit >= 32u) {
		return u32F;
	}
	return u32F | (1u << uBit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_flags_set_u - set one bit in a frame flags word (value).
 *
 * f:   current flags word
 * bit: bit index to force on (0 = LSB, 31 = MSB)
 *
 * Returns f | (1u << bit) for bit in 0..31; returns f unchanged when
 * bit >= 32. Pair with gj_frame_flags_test_u (batch4739). Self-
 * contained; no parent wires.
 */
uint32_t
gj_frame_flags_set_u(uint32_t u32F, unsigned uBit)
{
	(void)NULL;
	return b4738_flags_set(u32F, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_flags_set_u(uint32_t u32F, unsigned uBit)
    __attribute__((alias("gj_frame_flags_set_u")));

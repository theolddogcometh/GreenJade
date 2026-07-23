/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4739: test one bit in a frame flags word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_flags_test_u(uint32_t f, unsigned bit);
 *     - Return 1 if bit index bit of f is set (0 = LSB), else 0.
 *       bit >= 32 yields 0 (no shift UB).
 *   uint32_t __gj_frame_flags_test_u  (alias)
 *   __libcgj_batch4739_marker = "libcgj-batch4739"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_flags_test_u surface only; no multi-def. Distinct from
 * gj_flags_test_all_u / gj_flags_test_any_u (batch4584/4585, mask
 * forms) and gj_bit_test_u32 (batch2721, int return). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4739_marker[] = "libcgj-batch4739";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4739_flags_test(uint32_t u32F, unsigned uBit)
{
	if (uBit >= 32u) {
		return 0u;
	}
	return ((u32F >> uBit) & 1u) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_flags_test_u - test one bit in a frame flags word.
 *
 * f:   flags word under test
 * bit: bit index (0 = least significant bit)
 *
 * Returns 1 if the selected bit is set and bit < 32; returns 0 if
 * bit >= 32 or the bit is clear. Pair with gj_frame_flags_set_u
 * (batch4738). Self-contained; no parent wires.
 */
uint32_t
gj_frame_flags_test_u(uint32_t u32F, unsigned uBit)
{
	(void)NULL;
	return b4739_flags_test(u32F, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_flags_test_u(uint32_t u32F, unsigned uBit)
    __attribute__((alias("gj_frame_flags_test_u")));

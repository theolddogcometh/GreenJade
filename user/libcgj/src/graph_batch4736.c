/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4736: modular frame sequence successor.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_seq_next_u(uint32_t seq);
 *     - Return seq + 1 with natural uint32_t wrap.
 *   uint32_t __gj_frame_seq_next_u  (alias)
 *   __libcgj_batch4736_marker = "libcgj-batch4736"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_seq_next_u surface only; no multi-def. Distinct from
 * gj_seq_next_u32 (batch2095) — framing-specific symbol. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4736_marker[] = "libcgj-batch4736";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4736_seq_next(uint32_t u32Seq)
{
	return u32Seq + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_seq_next_u - modular successor of a frame sequence number.
 *
 * seq: current sequence number
 *
 * Returns seq + 1 with unsigned wrap (UINT32_MAX → 0). Pair with
 * gj_frame_seq_gap_u (batch4737) for wrap-aware distance. Self-
 * contained; no parent wires.
 */
uint32_t
gj_frame_seq_next_u(uint32_t u32Seq)
{
	(void)NULL;
	return b4736_seq_next(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_seq_next_u(uint32_t u32Seq)
    __attribute__((alias("gj_frame_seq_next_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4737: wrap-aware frame sequence distance.
 *
 * Surface (unique symbols):
 *   uint32_t gj_frame_seq_gap_u(uint32_t a, uint32_t b);
 *     - Forward wrap distance from a to b: (b - a) in uint32_t modular
 *       arithmetic (how many steps of seq_next from a reach b).
 *   uint32_t __gj_frame_seq_gap_u  (alias)
 *   __libcgj_batch4737_marker = "libcgj-batch4737"
 *
 * Exclusive continuum CREATE-ONLY (4731-4740: frame_hdr_size_u,
 * frame_total_u, frame_valid_len_u, frame_payload_len_u,
 * frame_crc_seed_u, frame_seq_next_u, frame_seq_gap_u,
 * frame_flags_set_u, frame_flags_test_u, batch_id_4740). Unique
 * gj_frame_seq_gap_u surface only; no multi-def. Distinct from
 * gj_seq_gap_p (batch2094, boolean gap) — returns distance, not a
 * predicate. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4737_marker[] = "libcgj-batch4737";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4737_seq_gap(uint32_t u32A, uint32_t u32B)
{
	/* Modular forward distance: wraps correctly for all a, b. */
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_frame_seq_gap_u - wrap-aware forward distance between sequence numbers.
 *
 * a: earlier (or reference) sequence number
 * b: later (or observed) sequence number
 *
 * Returns (b - a) mod 2^32 — the number of seq_next steps from a to b.
 * Equal values yield 0; wrap past UINT32_MAX is handled by unsigned
 * subtraction. Pair with gj_frame_seq_next_u (batch4736). Self-
 * contained; no parent wires.
 */
uint32_t
gj_frame_seq_gap_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4737_seq_gap(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_frame_seq_gap_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_frame_seq_gap_u")));

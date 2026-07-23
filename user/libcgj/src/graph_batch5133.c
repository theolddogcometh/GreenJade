/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5133: pure-value seqlock read-end stamp (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_end_u(uint32_t seq);
 *     - Close a pure-value seqlock read section by returning the
 *       re-sampled sequence stamp (identity). Pair with gj_seq_begin_u
 *       (batch5131) and gj_seq_retry_u (batch5132).
 *   uint32_t __gj_seq_end_u  (alias)
 *   __libcgj_batch5133_marker = "libcgj-batch5133"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from pointer seqlock surfaces (batch548 /
 * 1555-1558) — unique gj_seq_end_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5133_marker[] = "libcgj-batch5133";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5133_seq_end(uint32_t u32Seq)
{
	return u32Seq;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_end_u - re-sample identity stamp at pure-value seqlock read end.
 *
 * seq: current sequence counter after the read critical section
 *
 * Returns seq unchanged. Typical use:
 *   start = gj_seq_begin_u(seq0);
 *   ... copy ...
 *   now = gj_seq_end_u(seq1);
 *   if (gj_seq_retry_u(start, now)) retry;
 * No parent wires.
 */
uint32_t
gj_seq_end_u(uint32_t u32Seq)
{
	(void)NULL;
	return b5133_seq_end(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_end_u(uint32_t u32Seq)
    __attribute__((alias("gj_seq_end_u")));

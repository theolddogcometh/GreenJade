/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5134: pure-value seqlock write-begin (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_write_begin_u(uint32_t seq);
 *     - Enter a writer critical section: bump seq by 1 (even → odd on
 *       the normal path). Returns the new sequence value.
 *   uint32_t __gj_seq_write_begin_u  (alias)
 *   __libcgj_batch5134_marker = "libcgj-batch5134"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_seqcnt_begin (batch888) and
 * gj_seqlock_hint_write_begin (batch1555) — unique gj_seq_write_begin_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5134_marker[] = "libcgj-batch5134";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic seqlock: post-increment even → odd to mark write in progress. */
static uint32_t
b5134_write_begin(uint32_t u32Seq)
{
	return u32Seq + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_write_begin_u - bump sequence even→odd (writer enter).
 *
 * seq: idle (even) sequence counter before the write section
 *
 * Returns seq + 1. Pair with gj_seq_write_end_u (batch5135).
 * Pure value — caller stores the result. No parent wires.
 */
uint32_t
gj_seq_write_begin_u(uint32_t u32Seq)
{
	(void)NULL;
	return b5134_write_begin(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_write_begin_u(uint32_t u32Seq)
    __attribute__((alias("gj_seq_write_begin_u")));

/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5135: pure-value seqlock write-end (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_write_end_u(uint32_t seq);
 *     - Leave a writer critical section: bump seq by 1 (odd → even on
 *       the normal path). Returns the new sequence value.
 *   uint32_t __gj_seq_write_end_u  (alias)
 *   __libcgj_batch5135_marker = "libcgj-batch5135"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_seqcnt_end (batch889) and
 * gj_seqlock_hint_write_end (batch1556) — unique gj_seq_write_end_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5135_marker[] = "libcgj-batch5135";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic seqlock: post-increment odd → even to mark write complete. */
static uint32_t
b5135_write_end(uint32_t u32Seq)
{
	return u32Seq + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_write_end_u - bump sequence odd→even (writer leave).
 *
 * seq: active (odd) sequence counter after mutations
 *
 * Returns seq + 1. Pair with gj_seq_write_begin_u (batch5134).
 * Pure value — caller stores the result. No parent wires.
 */
uint32_t
gj_seq_write_end_u(uint32_t u32Seq)
{
	(void)NULL;
	return b5135_write_end(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_write_end_u(uint32_t u32Seq)
    __attribute__((alias("gj_seq_write_end_u")));

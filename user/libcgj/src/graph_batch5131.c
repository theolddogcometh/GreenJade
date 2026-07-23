/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5131: pure-value seqlock read-begin stamp (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_begin_u(uint32_t seq);
 *     - Snapshot a sequence counter for a seqlock-style read section.
 *       Returns seq unchanged (no memory, no spin). Pair with
 *       gj_seq_retry_u (batch5132) / gj_seq_end_u (batch5133).
 *   uint32_t __gj_seq_begin_u  (alias)
 *   __libcgj_batch5131_marker = "libcgj-batch5131"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_seqlock_read_begin (batch548) and
 * gj_seqlock_hint_read_begin (batch1557) — unique gj_seq_begin_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5131_marker[] = "libcgj-batch5131";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5131_seq_begin(uint32_t u32Seq)
{
	return u32Seq;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_begin_u - snapshot seq for a pure-value seqlock read section.
 *
 * seq: current sequence counter word (even = idle, odd = writer active)
 *
 * Returns seq as the start stamp. Caller loads protected data, then
 * checks gj_seq_retry_u(start, now). Self-contained; no parent wires.
 */
uint32_t
gj_seq_begin_u(uint32_t u32Seq)
{
	(void)NULL;
	return b5131_seq_begin(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_begin_u(uint32_t u32Seq)
    __attribute__((alias("gj_seq_begin_u")));

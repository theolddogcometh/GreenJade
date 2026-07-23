/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5132: pure-value seqlock read-retry (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_seq_retry_u(uint32_t start, uint32_t now);
 *     - Return 1 if the read must be retried: ((now - start) & 1) != 0
 *       or now != start; else 0 (consistent snapshot).
 *   uint32_t __gj_seq_retry_u  (alias)
 *   __libcgj_batch5132_marker = "libcgj-batch5132"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_seqlock_read_retry (batch548) and
 * gj_seqlock_hint_read_retry (batch1558) — unique gj_seq_retry_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5132_marker[] = "libcgj-batch5132";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Retry when the delta is odd (writer crossed) or the stamp changed.
 * Equivalent to an inconsistent pure-value re-sample vs start.
 */
static uint32_t
b5132_seq_retry(uint32_t u32Start, uint32_t u32Now)
{
	if (((u32Now - u32Start) & 1u) != 0u) {
		return 1u;
	}
	if (u32Now != u32Start) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_retry_u - 1 if pure-value seqlock read must be retried, else 0.
 *
 * start: stamp from gj_seq_begin_u
 * now:   re-sampled sequence after reading protected data
 *
 * Returns 1 when ((now - start) & 1) != 0 or now != start; else 0.
 * No parent wires.
 */
uint32_t
gj_seq_retry_u(uint32_t u32Start, uint32_t u32Now)
{
	(void)NULL;
	return b5132_seq_retry(u32Start, u32Now);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seq_retry_u(uint32_t u32Start, uint32_t u32Now)
    __attribute__((alias("gj_seq_retry_u")));

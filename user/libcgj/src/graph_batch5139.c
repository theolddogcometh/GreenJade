/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5139: pure-value sequence oddness probe (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rcu_is_odd_u(uint32_t seq);
 *     - Return 1 if seq is odd (writer active / odd GP phase), else 0.
 *   uint32_t __gj_rcu_is_odd_u  (alias)
 *   __libcgj_batch5139_marker = "libcgj-batch5139"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Unique gj_rcu_is_odd_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5139_marker[] = "libcgj-batch5139";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5139_is_odd(uint32_t u32Seq)
{
	if ((u32Seq & 1u) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rcu_is_odd_u - 1 if seq LSB is set (odd), else 0.
 *
 * seq: sequence or grace-period counter
 *
 * Classic seqlock: odd means a writer is mid-update. Pure value probe;
 * no parent wires.
 */
uint32_t
gj_rcu_is_odd_u(uint32_t u32Seq)
{
	(void)NULL;
	return b5139_is_odd(u32Seq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rcu_is_odd_u(uint32_t u32Seq)
    __attribute__((alias("gj_rcu_is_odd_u")));
